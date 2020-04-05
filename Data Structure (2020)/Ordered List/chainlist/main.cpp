#include <iostream>
#include "chainlist.h"

int main()
{
    // 定义单链表头指针
    Node *H;
    // 初始化单链表
    InitList(H);
    // 插入a, c, e三个元素
    ListInsert_order(H, 'a');
    ListInsert_order(H, 'c');
    ListInsert_order(H, 'e');
    // 输出链表长度
    cout << "1. The length of chainlist is " <<
        ListLength(H) << "." << endl;
    // 遍历输出当前线性表的每一个元素
    TraverseList(H); // 直接调用TraverseList接口

    // 有序插入元素b, d, f, 输出当前线性表的长度
    ListInsert_order(H, 'b');
    ListInsert_order(H, 'd');
    ListInsert_order(H, 'f');
    // 输出链表长度
    cout << "2. The length of chainlist is " <<
        ListLength(H) << "." << endl;
    // 遍历输出当前线性表的每一个元素
    TraverseList(H);

    // 删除用户指定的任意元素
    ListDelete(H, 'b');
    ListDelete(H, 'c');
    // 输出链表长度
    cout << "3. The length of chainlist is " <<
        ListLength(H) << "." << endl;
    // 遍历输出当前线性表的每一个元素
    TraverseList(H);
    
    // 释放单链表
    DestroyList(H);
    
    return 0;
}