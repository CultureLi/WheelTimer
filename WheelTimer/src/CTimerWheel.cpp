#include "CTimerWheel.h"

CTimerNode* CTimerMgr::curExecuteNode = nullptr;
uint32_t CTimerMgr::wheelUidIndex = 0;

void TimerCallBack(void* data)
{
	CTimerNode* pNode = static_cast<CTimerNode*>(data);
	if (pNode && !pNode->Execute())
		delete pNode;
}

CTimerNode::CTimerNode(CTimerWheel* pWheel, uint64_t interval, int repeatTimes, Timer_cb_t* pFun, void* data)
{
	std::cout << "CTimerNode 创建" << std::endl;
	this->interval = interval;
	this->repeatTimes = repeatTimes;
	this->callback = pFun;
	this->userdata = data;
	node.callback = TimerCallBack;
	node.userdata = this;
	uid = pWheel->GenNodeUid();
	this->pWheel = pWheel;
}

CTimerNode::~CTimerNode()
{
	std::cout << "CTimerNode 销毁" << std::endl;
}

bool CTimerNode::Execute()
{
	CTimerMgr::curExecuteNode = this;
	if (callback)
		callback(userdata);
	CTimerMgr::curExecuteNode = nullptr;

	if (pWheel == nullptr)
		return false;

	if (repeatTimes > 0)
		repeatTimes--;

	if (repeatTimes != 0)
		pWheel->AddNode(this, this->interval);
	else
	{
		pWheel->DetachNode(this);
		return false;
	}
	return true;
	
}

void CTimerNode::Detach()
{
	node.Detach();
	repeatTimes = 0;
}

/******************************************** CTimerWheel***************************************/

CTimerWheel::CTimerWheel():nodeUidIndex(0), remainTime(0.0f)
{
	std::cout << "CTimerWheel 创建" << std::endl;
	uid = CTimerMgr::Instance().GenWheelUid();
	CTimerMgr::Instance().wheelMap[uid] = this;
}

CTimerWheel::~CTimerWheel()
{
	std::cout << "CTimerWheel 销毁" << std::endl;
	std::map<uint64_t, CTimerNode*>::iterator iter;
	for (iter = nodeMap.begin();iter!= nodeMap.end();++iter)
	{
		(iter->second)->Detach();
		delete iter->second;
	}
	nodeMap.clear();
	CTimerMgr::Instance().wheelMap.erase(uid);

}

uint64_t CTimerWheel::CreateTimerNode(uint64_t interval, int repeatTimes, Timer_cb_t* pFun, void* data)
{
	CTimerNode* pNode = new CTimerNode(this, interval, repeatTimes, pFun, data);
	if (pNode == nullptr)
		return 0;
	
	uint64_t nodeUid = pNode->GetUid();
	nodeMap[nodeUid] = pNode;
	timer.AddNode(&pNode->node, interval);
	return nodeUid;
}

void CTimerWheel::AddNode(CTimerNode* pNode, uint32_t interval)
{
	timer.AddNode(&pNode->node, interval);
}

void CTimerWheel::DetachNode(CTimerNode* pNode)
{
	if (pNode != nullptr)
	{
		pNode->Detach();
		nodeMap.erase(pNode->GetUid());
	}
}

void CTimerWheel::Update(double delta)
{
	remainTime += delta;
	uint64_t tick = (uint64_t)remainTime;
	if (tick <= 0)
		return;

	remainTime -= tick;
	timer.Update(tick);
}


/******************************************** CTimerMgr***************************************/

void CTimerMgr::DeleteCurExecuteNode()
{
	if (CTimerMgr::curExecuteNode != nullptr && CTimerMgr::curExecuteNode->pWheel != nullptr)
	{
		CTimerMgr::curExecuteNode->pWheel->DetachNode(CTimerMgr::curExecuteNode);
	}
	
}
void CTimerMgr::DeleteTimerWheel(uint32_t)
{

}