#include <iostream>
#include <cstring>

using namespace std;

typedef char DataType;

//二叉树结点类型
typedef struct Node
{
    //数据域
    DataType data;

    //结点的左右子树指针
    struct Node *lchild;
    struct Node *rchild;
} BTNode;

//实现细节忽略
void TreeInit(BTNode *&root)
{
    // root = new BTNode;
}

//按照前序遍历序列建立二叉树
void CreateBTree_Pre(BTNode *&root, char *exp)
{
    static int i = -1;

    i++;
    // cout << i << "::" << exp[i] << " ";
    if (exp[i] == '\0')
        return;

    if (exp[i] == '#')
    {
        root = NULL;
    }
    else
    {
        root = new BTNode;
        root->data = exp[i];
        CreateBTree_Pre(root->lchild, exp);
        CreateBTree_Pre(root->rchild, exp);
    }

    return;
}

//前序遍历二叉树
void PreOrder(BTNode *root)
{
    if (root)
    {
        cout << root->data;
        PreOrder(root->lchild);
        PreOrder(root->rchild);
    }
}

//中序遍历二叉树
void InOrder(BTNode *root)
{
    if (root)
    {
        InOrder(root->lchild);
        cout << root->data;
        InOrder(root->rchild);
    }
}

//后序遍历二叉树
void PostOrder(BTNode *root)
{
    if (root)
    {
        PostOrder(root->lchild);
        PostOrder(root->rchild);
        cout << root->data;
    }
}

//计算二叉树深度
int BTreeDepth(BTNode *root)
{
    int h, lh, rh;
    if (!root)
        h = 0;
    else
    {
        lh = BTreeDepth(root->lchild);
        rh = BTreeDepth(root->rchild);
        if (lh > rh)
            h = lh + 1;
        else
            h = rh + 1;
    }
    return h;
}

int main()
{
    BTNode *root;

    //初始化空二叉树
    TreeInit(root);

    char expr[100];
    cout << "Preorder Expression of BiTree: ";
    cin >> expr;
    char *ex = expr;

    //以前序遍历序列建立二叉树
    CreateBTree_Pre(root, ex);

    cout << "Preorder: ";
    PreOrder(root);
    cout << endl;
    cout << "Inorder: ";
    InOrder(root);
    cout << endl;
    cout << "Postorder: ";
    PostOrder(root);
    cout << endl;
    cout << "Depth: " << BTreeDepth(root) << endl;

    return 0;
}