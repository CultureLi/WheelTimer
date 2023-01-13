#pragma once
#include <stdint.h>
#include "Common.h"
#include "CTimerWheelCore.h"
#include <map>

class CTimerWheel;

class CTimerNode
{
private:
	uint64_t uid;

	int repeatTimes; // �ظ�����
	uint32_t interval; // �������
	
	Timer_cb_t* callback = nullptr;
	void* userdata = nullptr;


public:
	CTimerNode(CTimerWheel* pWheel, uint64_t interval, int repeatTimes, Timer_cb_t* pFun, void*data);
	~CTimerNode();
	bool Execute();
	void Detach();
	
	inline uint64_t GetUid() { return uid; }
	CTimerNodeCore node;

	CTimerWheel* pWheel;

};


/******************************************** CTimerWheel***************************************/

class CTimerWheel
{
private:
	CTimerWheelCore timer;
	uint32_t uid;

	std::map<uint64_t, CTimerNode*> nodeMap;
	uint64_t nodeUidIndex;

public:
	CTimerWheel();
	~CTimerWheel();
	uint64_t GenNodeUid();
	uint64_t CreateTimerNode(uint64_t interval, int repeatTimes, Timer_cb_t* pFun, void* data);
	void AddNode(CTimerNode* pNode, uint32_t interval);
	void DeleteNode(uint64_t uid);
	void EraseNodeUid(uint64_t uid) {
		if (nodeMap.find(uid) != nodeMap.end())
			nodeMap.erase(uid);
	}
	void Update(double delta);

	double remainTime; //ÿ�θ��º�ʣ��ʱ��

	uint64_t GetNodeCount() { return nodeMap.size(); }
};



/******************************************** CTimerMgr***************************************/

class CTimerMgr
{
private:
	CTimerMgr() {};
public:
	~CTimerMgr() {};
	CTimerMgr(const CTimerMgr&) = delete; //���ÿ������캯��
	CTimerMgr& operator=(const CTimerMgr&) = delete; //���ø�ֵ�����

	static CTimerMgr& Instance() 
	{
		static CTimerMgr instance;
		return instance;
	}

	inline uint32_t GenWheelUid() { return ++wheelUidIndex; }
	void DeleteCurExecuteNode();
	void DeleteNode(uint64_t);
	CTimerWheel* GetWheelByNodeUid(uint64_t uid);

private:
	static uint32_t wheelUidIndex;
public:
	static uint64_t curExecuteNodeUid;
	std::map<uint32_t, CTimerWheel*> wheelMap;
};


