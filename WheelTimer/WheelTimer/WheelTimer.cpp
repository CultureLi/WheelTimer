
#include <iostream>
#include "CTimerWheelCore.h"
#include <windows.h>

using namespace std;


static int64_t startTime;


void callback(void* data)
{
    uint64_t end = GetTickCount64();
    cout << "消耗 " << end - startTime << endl;
    std::cout << "callback" << (int)data << std::endl;
}


int main()
{
    CTimerWheelCore *timerMgr = new CTimerWheelCore();

    CTimerNodeCore *node = new CTimerNodeCore();
    node->callback = callback;
    node->userdata = (void*)124;
    timerMgr->AddNode(node, 1251);

    startTime = GetTickCount64();
    uint64_t lastUpdateTime = startTime;
    while (true)
    {
        uint64_t now = GetTickCount64();
        uint32_t delta = now - lastUpdateTime;
        if (delta > 3)
        {
			timerMgr->Update(delta);
			lastUpdateTime = now;
        }
    }
}
