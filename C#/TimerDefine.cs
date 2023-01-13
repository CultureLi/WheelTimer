using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Timer
{
    internal class TimerDefine
    {
        // 第1个轮占的位数
        public static readonly int TVR_BITS = 8;
        // 第1个轮的长度
        public static readonly int TVR_SIZE = 1 << TVR_BITS;
        // 第n个轮占的位数
        public static readonly int TVN_BITS = 6;
        // 第n个轮的长度
        public static readonly int TVN_SIZE = (1 << TVN_BITS);
        // 掩码：取模或整除用
        public static readonly uint TVR_MASK = (uint)TVR_SIZE - 1;
        public static readonly uint TVN_MASK = (uint)TVN_SIZE - 1;

        // 第1个圈的当前指针位置
        public static uint FirstWheelIdx(uint v)
        {
            return v & TVR_MASK;
        }

        // 后面第n个圈的当前指针位置
        public static uint OtherWheelIdx(uint v, int n)
        {
            return ((v) >> (TVR_BITS + (n) * TVN_BITS)) & TVN_MASK;
        }

    }
}
