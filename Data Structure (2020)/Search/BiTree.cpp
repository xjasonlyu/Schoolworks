#include "BiTree.h"

//初始化空二叉树
void TreeInit(BTNode *&root)
{
    root = NULL;
}

//从二叉排序树中查找元素item，返回item所在结点的地址
BTNode *BSTSearch(BTNode *root, DataType item)
{
    BTNode *p = root;
    if (root == NULL)
        return NULL; //空树查找失败
    cout << "开始查找" << item << endl;
    while (p)
    {
        cout << p->data << "->";
        if (p->data == item)
        {
            cout << "查找成功" << endl;
            return p;
        }
        else if (p->data > item)
            p = p->left;
        else
            p = p->right;
    }
    cout << "查找失败" << endl;
    return NULL;
}

//向二叉排序树中插入item
int BSTInsert(BTNode *&root, DataType item)
{
    if (!root)
    {
        root = new BTNode;
        root->data = item;
        // set children to NULL
        root->left = NULL;
        root->right = NULL;
        // insert success
        return 1;
    }

    if (root->data > item)
        return BSTInsert(root->left, item);
    else
        return BSTInsert(root->right, item);
}

//删除二叉排序树中与指定的数据相同的结点
int BSTDelete(BTNode *&root, DataType item)
{
    if (!root)
    {
        return 0;
    }

    if (root->data == item)
    {
        BTNode *q, *s;
        if (!root->right)
        {
            root = root->left;
        }
        else if (!root->left)
        {
            root = root->right;
        }
        else
        {
            q = root;
            s = root->left;
            while (s->right)
            {
                q = s;
                s = s->right;
            }
            root->data = s->data;
            if (q != root)
                q->right = s->left;
            else
                q->left = s->left;
        }
        return 1;
    }
    else if (root->data > item)
        return BSTDelete(root->left, item);
    else
        return BSTDelete(root->right, item);
}

//中序遍历二叉树
void InOrder(BTNode *root)
{
    if (root != NULL)
    {
        InOrder(root->left);       //前序遍历左子树
        cout << root->data << " "; //访问根
        InOrder(root->right);      //前序遍历右子树
    }
}

//释放二叉树中所有结点
void ClearBTree(BTNode *&root)
{
    if (root != NULL)
    {
        ClearBTree(root->left);
        ClearBTree(root->right);
        delete root;
        root = NULL;
    }
}
