

#include <iostream>
#include "CTimerWheel.h"
#include <windows.h>

using namespace std;

void callback(void* data)
{
    std::cout << "callback" << (int)data << std::endl;
}

int main()
{

    CTimerWheel *timerMgr = new CTimerWheel(20, GetTickCount64());

    TimerNode *node = new TimerNode(callback,(void*)123);

    timerMgr->AddNode(node, -1);
  

    while (true)
    {
        //cout << "Update" << endl;
        timerMgr->Update(GetTickCount64());
        Sleep(30);
    }
}
