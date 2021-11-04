#include "CTimerWheelMgr.h"

uint32_t CTimerWheelMgr::CreateTimer(uint16_t interval, Timer_cb_t fun, void* userData)
{
	CTimerWheel* timer = new CTimerWheel(20, 0);
	TimerNode* node = new TimerNode(fun, userData);
	timer->AddNode(node, -1);
	uint32_t uid = ++uidIndex;


}