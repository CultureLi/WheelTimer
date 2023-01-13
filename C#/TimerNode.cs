using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;
using static Timer.TimerLinkedList;

namespace Timer
{
    internal class TimerNode: LinkNode
    {
        public uint uid;
        // 用户数据
        public object? userdata;
        // 回调函数
        public Action<object> callback;
        // 到期时间
        public uint expire;
        // 重复次数
        public uint repeadCount;
        // 触发间隔
        public uint interval;
        public TimerNode(Action<object> cb, object userdata)
        {
            callback = cb;
            Init(cb, userdata);
        }
        ~TimerNode()
        {
            Console.WriteLine("timerNode release ");
        }
        public void Init(Action<object> cb, object userdata)
        {
            uid = 0;
            next = null;
            prev = null;
            this.userdata = userdata;
            callback = cb;
            expire = 0;
            repeadCount = 0;
        }
    }
}
