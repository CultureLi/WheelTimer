using static Timer.TimerLinkedList;

namespace Timer
{
    internal class TimerWheel
    {
        public class RootWheel
        {
            public LinkNode[] vec;
            public RootWheel()
            {
                vec = new LinkNode[TimerDefine.TVR_SIZE];
            }
        }
        public class OtherWheel
        {
            public LinkNode[] vec;
            public OtherWheel()
            {
                vec = new LinkNode[TimerDefine.TVR_SIZE];
            }
        }


        // 第1个轮
        public RootWheel rootWheel;
        // 后面4个轮
        public OtherWheel[] otherWheel;
        public ulong lastTime;             // 上一次的时间毫秒
        public uint curTick;             // 当前的tick
        public ushort interval;             // 每个时间点的毫秒间隔
        public ushort remainder;            // 剩余的毫秒
        public TimerWheel(ushort interval, ulong currtime)
        {
            rootWheel = new RootWheel();
            otherWheel = new OtherWheel[4];
            lastTime = 0;
            curTick = 0;
            remainder = 0;
            InitWheel(interval, currtime);
        }
      

        void InitLinkList(LinkNode head)
        {
            head.prev = head;
            head.next = head;
        }


        void InitWheel(ushort interval, ulong currtime)
        {
            this.interval = interval;
            lastTime = currtime;
            int i, j;
            for (i = 0; i < TimerDefine.TVR_SIZE; ++i)
            {
                rootWheel.vec[i] = new LinkNode();
                InitLinkList(rootWheel.vec[i]);
            }
            for (i = 0; i < 4; ++i)
            {
                otherWheel[i] = new OtherWheel();
                for (j = 0; j < TimerDefine.TVN_SIZE; ++j)
                {
                    otherWheel[i].vec[j] = new LinkNode();
                    InitLinkList(otherWheel[i].vec[j]);
                }
            }
        }


        void InsertToWheel(TimerNode node)
        {
            uint expire = node.expire;
            uint idx = expire - curTick;
            if (idx < TimerDefine.TVR_SIZE)
            {
                var head = rootWheel.vec[TimerDefine.FirstWheelIdx(expire)];
                AddBack(head, node);
            }
            else
            {
                int i;
                ulong sz;
                for (i = 0; i < 4; ++i)
                {
                    sz = (ulong)1 << (TimerDefine.TVR_BITS + (i + 1) * TimerDefine.TVN_BITS);
                    if (idx < sz)
                    {
                        idx = TimerDefine.OtherWheelIdx(expire, i);
                        var head = otherWheel[i].vec[idx];
                        AddBack(head, node);
                        break;
                    }
                }
            }
        }

        public void AddNode( TimerNode node)
        {
            node.expire = curTick + ((node.interval > 0) ? node.interval : 1);
            InsertToWheel(node);
        }

        int RemoveNode(TimerNode node)
        {
            if (!IsLinkListEmpty(node))
            {
                RemoveNode(node);
                return 1;
            }
            return 0;
        }

        void WheelCascade( OtherWheel otherWheel, uint idx)
        {
            InitLinkList(tempHead);
            LinkListSplice(otherWheel.vec[idx], tempHead);
            while (!IsLinkListEmpty(tempHead))
            {
                TimerNode node = (TimerNode)tempHead.next;
                TimerLinkedList.RemoveNode(tempHead.next);
                InsertToWheel(node);
            }
        }

        static LinkNode tempHead = new LinkNode();
        void WheelTick()
        {
            ++curTick;

            uint index = curTick & TimerDefine.TVR_MASK;
            if (index == 0)
            {
                int i = 0;
                uint idx;
                do
                {
                    idx = TimerDefine.OtherWheelIdx(curTick, i);
                    WheelCascade(otherWheel[i], idx);
                } while (idx == 0 && ++i < 4);
            }

            
            InitLinkList(tempHead);
            LinkListSplice(rootWheel.vec[index], tempHead);
            while (!IsLinkListEmpty(tempHead))
            {
                TimerNode? node = tempHead.next as TimerNode;
                TimerLinkedList.RemoveNode(tempHead.next);
                if (node.callback != null)
                {
                    node.callback(node.userdata);
                }
                if (node.repeadCount > 1)
                {
                    node.repeadCount--;
                    AddNode(node);
                }

            }
        }

        public void WheelUpdate(ulong currtime)
        {
            if (currtime > lastTime)
            {
                var diff = currtime - lastTime + remainder;
                lastTime = currtime;

                while (diff >= interval)
                {
                    diff -= interval;
                    WheelTick();
                }
                remainder = (ushort)diff;
            }
        }

    }
}
