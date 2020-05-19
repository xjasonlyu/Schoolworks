#include <iostream>
using namespace std;

typedef int DataType;

typedef struct Node{		
	DataType	data;    //数据域	
	Node     	*left,*right; //结点的左右子树指针	
}BTNode;//二叉树结点类型

//初始化空二叉树
void TreeInit(BTNode* &root);

//从二叉排序树中查找元素item，返回item所在结点的地址
BTNode *BSTSearch(BTNode *root,DataType item);

//向二叉排序树中插入item
int BSTInsert(BTNode *&root,DataType item);//补充完整此接口函数

//删除二叉排序树中与指定的数据相同的结点
int BSTDelete(BTNode *&root,DataType item);///补充完整此接口函数

//中序遍历二叉树
void InOrder(BTNode *root);

//释放二叉树中所有结点
void ClearBTree(BTNode* &root);
