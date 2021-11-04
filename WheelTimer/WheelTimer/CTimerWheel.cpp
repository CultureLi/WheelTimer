#include "CTimerWheel.h"

CTimerWheel::CTimerWheel(uint16_t interval, uint64_t currtime) {
	memset(this, 0, sizeof(*this));
	this->interval = interval;
	this->lasttime = currtime;
	int i, j;
	for (i = 0; i < TVR_SIZE; ++i) {
		INIT_LIST_HEAD(this->tvroot.vec + i);
	}
	for (i = 0; i < 4; ++i) {
		for (j = 0; j < TVN_SIZE; ++j)
			INIT_LIST_HEAD(this->tv[i].vec + j);
	}
}

void CTimerWheel::_AddNode(TimerNode* node) {
	uint32_t expire = node->expire;
	uint32_t idx = expire - this->currtick;

	list_head* head = NULL;
	if (idx < TVR_SIZE) {
		head = this->tvroot.vec + FIRST_INDEX(expire);
	}
	else {
		int i;
		uint64_t sz;
		for (i = 0; i < 4; ++i) {
			sz = (1ULL << (TVR_BITS + (i + 1) * TVN_BITS));
			if (idx < sz) {
				idx = NTH_INDEX(expire, i);
				head = this->tv[i].vec + idx;
				break;
			}
		}
	}
	list_add_tail(head, &node->lst);
}

void CTimerWheel::AddNode(TimerNode* node, uint32_t ticks) {
	node->expire = this->currtick + ((ticks > 0) ? ticks : 1);
	_AddNode(node);
}

int CTimerWheel::DelNode(TimerNode* node) {
	if (!list_empty(&node->lst)) {
		list_del_init(&node->lst);
		return 1;
	}
	return 0;
}

void CTimerWheel::_Cascade(tvnum_t* tv, int idx) {
	LIST_HEAD(head);
	list_splice(tv->vec + idx, &head);
	while (!list_empty(&head)) {
		TimerNode* node = (TimerNode*)head.next;
		list_del_init(head.next);
		_AddNode(node);
	}
}

void CTimerWheel::_Tick() {
	++this->currtick;

	uint32_t currtick = this->currtick;
	int index = (currtick & TVR_MASK);
	if (index == 0) {
		int i = 0;
		int idx;
		do {
			idx = NTH_INDEX(this->currtick, i);
			_Cascade(&(this->tv[i]), idx);
		} while (idx == 0 && ++i < 4);
	}

	list_head* head = this->tvroot.vec + index;
	while (!list_empty(head)) {
		list_head* node = head->next;
		list_del_init(node);
		TimerNode* pNode = list_entry(node,TimerNode,lst);
		if (pNode->callback) {
			pNode->callback(pNode->userdata);
		}
	}
}

void CTimerWheel::Update(uint64_t currtime)
{
	if (currtime > this->lasttime) {
		int diff = currtime - this->lasttime + this->remainder;
		int intv = this->interval;
		this->lasttime = currtime;
		while (diff >= intv) {
			diff -= intv;
			_Tick();
		}
		this->remainder = diff;
	}
}