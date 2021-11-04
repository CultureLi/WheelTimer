#pragma once
#include <stdint.h>
#include "CTimerWheel.h"

class CTimerWheelMgr
{
private:
	static uint32_t uidIndex;

public:
	static uint32_t CreateTimer(uint16_t interval, Timer_cb_t fun, void* userData);
	

};

