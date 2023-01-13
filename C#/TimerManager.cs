using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Timer
{
    internal class TimerManager
    {

        TimerWheel pWheel;
        uint uid;
        
        public TimerManager()
        {
            uid = 0;
            var now = (ulong)(DateTime.Now.ToUniversalTime().Ticks - 621355968000000000) / 10000;
            pWheel = new TimerWheel(1,now);
        }

        public uint CreateTimer(uint interval,Action<object> cb,object userdata,uint repeadCount)
        {
            uid += 1;
            // get from pool
            TimerNode node = new TimerNode(cb,userdata);
            node.uid = uid;
            node.repeadCount = repeadCount;
            node.interval = interval;

            pWheel.AddNode(node);

            return uid;
        }

        public void Update()
        {
            pWheel.WheelUpdate((ulong)(DateTime.Now.ToUniversalTime().Ticks - 621355968000000000) / 10000);
        }
    }
}
