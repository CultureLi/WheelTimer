#pragma once
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "IntrusiveList.h"

/*
* Use Five Wheels
*/

 // The first wheel bits
#define TVR_BITS 8
#define TVR_SIZE (1 << TVR_BITS)
#define TVR_MASK (TVR_SIZE - 1)

// Other wheel bits
#define TVN_BITS 6
#define TVN_SIZE (1 << TVN_BITS)
#define TVN_MASK (TVN_SIZE - 1)

// Calc the index in first wheel
#define FIRST_INDEX(v) ((v) & TVR_MASK)
// Calc the index in other wheel
#define NTH_INDEX(v, n) (((v) >> (TVR_BITS + (n) * TVN_BITS)) & TVN_MASK)

// callback
typedef void (*Timer_cb_t)(void*);


class TimerNode
{
public:
    list_head lst;
    void* userdata;              
    Timer_cb_t callback;
    uint32_t expire;  // expire time
    uint32_t uid;

    TimerNode(Timer_cb_t cb, void* ud)
    {
        callback = cb;
        userdata = ud;
        expire = 0;
        INIT_LIST_HEAD(&lst);
    }
};

// the first wheel
typedef struct tvroot {
    list_head vec[TVR_SIZE];
} tvroot_t;

// other wheel
typedef struct tvnum {
    list_head vec[TVN_SIZE];
} tvnum_t;



class CTimerWheel
{
private:
    tvroot_t tvroot;               
    tvnum_t tv[4];                 
    uint64_t lasttime;            
    uint32_t currtick;             
    uint16_t interval;             
    uint16_t remainder; // remain ms


public:
	CTimerWheel(uint16_t interval, uint64_t currtime);
	void AddNode(TimerNode* node, uint32_t ticks);
    int DelNode(TimerNode* node);
	void Update(uint64_t currtime);
private:
    void _Cascade(tvnum_t* tv, int idx);
    void _AddNode(TimerNode* node);
    void _Tick();
};
