
#include <iostream>
#include "CTimerWheel.h"
#include <windows.h>

using namespace std;


static double startTime;

void callback(void* data)
{
    double end = GetTickCount64();
    if (end - startTime > 3000)
    {
        //test 回调时删除
        CTimerMgr::Instance().DeleteCurExecuteNode();
    }

    std::cout << "callback" << (int)data << std::endl;

}


int main()
{
    CTimerWheel* pWheel = new CTimerWheel();
    pWheel->CreateTimerNode(1000, 7, callback, (void*)12);
    pWheel->CreateTimerNode(1345, 3, callback, (void*)34);
    startTime = GetTickCount64();
    double lastUpdateTime = startTime;
    while (true)
    {
        double now = GetTickCount64();
        double delta = now - lastUpdateTime;
        if (delta > 3)
        {
            if (pWheel != nullptr)
            {
                pWheel->Update(delta);
            }
			lastUpdateTime = now;
        }


        // test 直接删除timerwheel
        if(now - startTime > 2000)
        {
            if (pWheel != nullptr)
            {
                delete pWheel;
                pWheel = nullptr;
            }
        }
    }
}
