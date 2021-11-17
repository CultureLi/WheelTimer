#include "CTimerWheel.h"

uint64_t CTimerMgr::curExecuteNodeUid = 0;
uint32_t CTimerMgr::wheelUidIndex = 0;

void TimerCallBack(void* data)
{
	CTimerNode* pNode = static_cast<CTimerNode*>(data);
	if (pNode && !pNode->Execute())
		delete pNode;
}

CTimerNode::CTimerNode(CTimerWheel* pWheel, uint64_t interval, int repeatTimes, Timer_cb_t* pFun, void* data)
{
	
	this->interval = interval;
	this->repeatTimes = repeatTimes;
	this->callback = pFun;
	this->userdata = data;
	node.callback = TimerCallBack;
	node.userdata = this;
	uid = pWheel->GenNodeUid();
	this->pWheel = pWheel;
	//std::cout << "CTimerNode 创建" << (int)data << std::endl;
}

CTimerNode::~CTimerNode()
{
	pWheel->EraseNodeUid(uid);
	//std::cout << "CTimerNode 销毁" << (int)userdata << std::endl;
}

bool CTimerNode::Execute()
{
	CTimerMgr::curExecuteNodeUid = uid;
	if (callback)
		callback(userdata);
	CTimerMgr::curExecuteNodeUid = 0;

	if (pWheel == nullptr)
		return false;

	if (repeatTimes > 0)
		repeatTimes--;

	if (repeatTimes != 0)
		pWheel->AddNode(this, this->interval);
	else
	{
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
	std::cout << "CTimerWheel 创建" << uid << std::endl;
	uid = CTimerMgr::Instance().GenWheelUid();
	CTimerMgr::Instance().wheelMap[uid] = this;
}

CTimerWheel::~CTimerWheel()
{
	std::cout << "CTimerWheel 销毁" << uid<< std::endl;
	std::map<uint64_t, CTimerNode*>::iterator iter;

	std::vector<uint64_t> nodeUidList = extract_keys<uint64_t, CTimerNode*>(nodeMap);
	for(uint64_t nodeUid : nodeUidList)
	{
		DeleteNode(nodeUid);
	}
	nodeMap.clear();
	CTimerMgr::Instance().wheelMap.erase(uid);

}

uint64_t CTimerWheel::GenNodeUid()
{
	nodeUidIndex++;
	return GENUUID64(uid, nodeUidIndex);
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

void CTimerWheel::DeleteNode(uint64_t uid)
{
	std::map<uint64_t, CTimerNode*>::iterator iter = nodeMap.find(uid);
	if (iter == nodeMap.end())
		return;

	CTimerNode* pNode = iter->second;
	if (pNode == nullptr)
		return;

	if (pNode->GetUid() != CTimerMgr::curExecuteNodeUid)
		delete pNode;
	else
		pNode->Detach();

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
	DeleteNode(curExecuteNodeUid);
}

CTimerWheel* CTimerMgr::GetWheelByNodeUid(uint64_t uid)
{
	uint32_t wheelUid = HIGHUUID64(uid);
	std::map<uint32_t, CTimerWheel*>::iterator wheelIter;
	wheelIter = wheelMap.find(wheelUid);
	if (wheelIter == wheelMap.end())
		return nullptr;

	return wheelIter->second;	
}

void CTimerMgr::DeleteNode(uint64_t uid)
{
	CTimerWheel* pWheel = GetWheelByNodeUid(uid);
	if(pWheel)
		pWheel->DeleteNode(uid);
}