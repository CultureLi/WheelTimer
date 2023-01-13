
#include <iostream>
#include "CTimerWheel.h"
#include <windows.h>
#include <stdlib.h>
#include <chrono>
using namespace std;


static double startTime;

void callback(void* data)
{
//     double end = GetTickCount64();
//     if (int(data) == 1)
//     {
//         //test 回调时删除
//         CTimerMgr::Instance().DeleteCurExecuteNode();
//     }

    //std::cout << "callback" << (int)data << std::endl;

}


int main()
{
	using namespace std::chrono;
    CTimerWheel* pWheel = new CTimerWheel();
//     pWheel->CreateTimerNode(1000, 7, callback, (void*)1);
//     pWheel->CreateTimerNode(1345, 3, callback, (void*)2);
//     uint64_t node3 = pWheel->CreateTimerNode(6000, 2, callback, (void*)3);
//     pWheel->CreateTimerNode(12000, 2, callback, (void*)4);
//     pWheel->CreateTimerNode(14000, 2, callback, (void*)5);
    startTime = GetTickCount64();

    uint64_t testCount = 10 * 10000;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    for(int i=0;i< testCount;i++)
        pWheel->CreateTimerNode(rand()% 1000, 3, callback, (void*)i);
    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    duration<double, std::milli> time_span = t2 - t1;

    cout << "创建耗时："<<time_span.count() << endl;

    double lastUpdateTime = startTime;
    while (true)
    {
        double now = GetTickCount64();
        double delta = now - lastUpdateTime;
        if (delta > 3)
        {
            if (pWheel != nullptr)
            {
                high_resolution_clock::time_point t1 = high_resolution_clock::now();
                pWheel->Update(delta);
                high_resolution_clock::time_point t2 = high_resolution_clock::now();
                duration<double, std::milli> time_span = t2 - t1;
                cout << "更新消耗：" << time_span.count() <<" count: "<< pWheel->GetNodeCount()<< endl;
            }
			lastUpdateTime = now;
        }

//         double timecost = now - startTime;
//         if (timecost > 4000)
//         {
//             CTimerMgr::Instance().DeleteNode(node3);
//         }
//         // test 直接删除timerwheel
//         if(timecost > 10000)
//         {
//             if (pWheel != nullptr)
//             {
//                 delete pWheel;
//                 pWheel = nullptr;
//             }
//         }
    }
}
