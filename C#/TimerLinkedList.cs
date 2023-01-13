using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Timer
{
    internal class TimerLinkedList
    {

        public class LinkNode
        {
            public LinkNode? next;
            public LinkNode? prev;
            ~LinkNode() 
            {
                Console.WriteLine("clinknode release");
            }
        }

        // 初始化链表头：前后都指向自己
        public static void InitLinkList(LinkNode head)
        {
            head.prev = head;
            head.next = head;
        }

        // 插入结点到链表的前面，因为是循环链表，其实是在head的后面
        public static void AddFront(LinkNode head, LinkNode node)
        {
            node.prev = head;
            node.next = head.next;
            head.next.prev = node;
            head.next = node;
        }

        // 插入结点到链表的后面，因为是循环链表，所以其实是在head的前面
        public static void AddBack(LinkNode head, LinkNode node)
        {
            node.prev = head.prev;
            node.next = head;
            node.prev.next = node;
            head.prev = node;
        }

        // 判断链表是否为空：循环链表为空是头的下一个和上一个都指向自己
        public static bool IsLinkListEmpty(LinkNode head)
        {
            return head == head.next;
        }

        // 从链表中移除自己，同时会重设结点
        public static void RemoveNode(LinkNode? node)
        {
            if (node == null)
                return;
            node.next.prev = node.prev;
            node.prev.next = node.next;
            InitLinkList(node);
        }

        // 将链表1的结点取出来，放到链表2
        public static void LinkListSplice(LinkNode head1, LinkNode head2)
        {
            if (!IsLinkListEmpty(head1))
            {
                LinkNode first = head1.next;       // 第1个结点
                LinkNode last = head1.prev;        // 最后1个结点
                LinkNode at = head2.next;          // 插在第2个链表的这个结点前面
                first.prev = head2;
                head2.next = first;
                last.next = at;
                at.prev = last;
                InitLinkList(head1);
            }
        }
    }
}
