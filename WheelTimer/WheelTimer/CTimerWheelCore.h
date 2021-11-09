#pragma once
#include "CIntrusiveList.h"

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
typedef void (Timer_cb_t)(void*);


struct CTimerNodeCore
{
    list_head lst;
    void* userdata;              
    Timer_cb_t *callback;
    uint32_t expire;  // expire time
    uint64_t endtick;

    CTimerNodeCore():callback(nullptr),userdata(nullptr),expire(0), endtick(0)
    {
        INIT_LIST_HEAD(&lst);
    }
    ~CTimerNodeCore()
    {
        list_del_init(&lst);
        expire = 0;
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



class CTimerWheelCore
{
private:
    tvroot_t tvroot;               
    tvnum_t tv[4];                          
    uint32_t currtick;    
    uint64_t alltick;



public:
    CTimerWheelCore():currtick(0),alltick(0)
    {
		for (int i = 0; i < TVR_SIZE; ++i) {
			INIT_LIST_HEAD(this->tvroot.vec + i);
		}
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < TVN_SIZE; ++j)
				INIT_LIST_HEAD(this->tv[i].vec + j);
		}
    }
    void AddNode(CTimerNodeCore* node, uint32_t delta)
    {
        delta = delta > 0 ? delta : 1;
        node->expire = currtick + delta;
        node->endtick = alltick + delta;
        _AddNode(node, true);
    }
	void Update(uint32_t delta)
	{
		while (delta > 0)
		{
			_Tick();
			--delta;
		}
	}
private:
    void _Cascade(list_head* head)
    {
		while (!list_empty(head)) {
			list_head* node = head->next;
			list_del_init(head->next);
            CTimerNodeCore* pNode = list_entry(node, CTimerNodeCore, lst);
			_AddNode(pNode,false);
		}
    }
    void _AddNode(CTimerNodeCore* node,bool bAddTail)
    {
		uint32_t expire = node->expire;
		uint32_t idx = expire - this->currtick;

		list_head* head = NULL;
		if (idx < TVR_SIZE) {
			head = this->tvroot.vec + FIRST_INDEX(expire);
		}
		else {
			for (int i = 0; i < 4; ++i) {
                uint64_t sz = (1ULL << (TVR_BITS + (i + 1) * TVN_BITS));
				if (idx < sz) {
					idx = NTH_INDEX(expire, i);
					head = this->tv[i].vec + idx;
					break;
				}
			}
		}
        if (bAddTail)
            list_move_tail(&node->lst, head);
        else
            list_move(&node->lst, head);
    }
    void _Tick()
    {
		++currtick;
		++alltick;

		int index = (currtick & TVR_MASK);
		if (index == 0) {
			int i = 0, idx = 0;
			do {
				idx = NTH_INDEX(currtick, i);
				_Cascade(tv[i].vec+idx);
			} while (idx == 0 && ++i < 4);
		}
		list_head* head = this->tvroot.vec + index;
		while (!list_empty(head)) {
			list_head* node = head->next;
			list_del_init(node);
			CTimerNodeCore* pNode = list_entry(node, CTimerNodeCore, lst);
			if (pNode->callback) {
				pNode->callback(pNode->userdata);
			}
		}
    }
};
