#include "BiTree.h"

int main()
{
    BTNode *root;
    DataType A[] = {36, 20, 50, 80, 30, 10, 26, 56};
    TreeInit(root);

    // 建立二叉排序树
    for (int i = 0; i < 8; i++)
        BSTInsert(root, A[i]);

    BSTSearch(root, 26); // 查找

    BSTSearch(root, 70); // 查找

    DataType elem = 32;
    cout << "插入元素：" << elem << endl;
    BSTInsert(root, elem);

    cout << "第一次中序遍历序列：";
    InOrder(root); //中序遍历
    cout << endl;

    DataType item = 36;
    cout << "删除元素：" << item << endl;
    BSTDelete(root, item); //删除

    cout << "删除后中序遍历序列：";
    InOrder(root);

    cout << endl;

    ClearBTree(root); //清空二叉树
    return 0;
}