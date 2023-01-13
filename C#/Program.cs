// See https://aka.ms/new-console-template for more information
using System.Diagnostics;
using Timer;

Console.WriteLine("hello");
var mgr = new TimerManager();

void fun(object data)
{
    Console.WriteLine("callback {0}",data);
}


mgr.CreateTimer(3000, fun, 1, 3);

mgr.CreateTimer(3000, fun, "hello", 3);

mgr.CreateTimer(4000, fun, 2.222f, 3);

while (true)
{
    mgr.Update();
    GC.Collect();
}
