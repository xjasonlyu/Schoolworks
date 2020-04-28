#include <iostream>
using namespace std;

//Huffman树的存储结构
#define n 9			  //叶子数目
#define m (2 * n - 1) //树中结点总数

typedef struct //结点类型
{
	float weight;				//权值，不妨设权值均大于零
	int lchild, rchild, parent; //左右孩子及双亲指针
} HTNode;
typedef HTNode HuffmanTree[m]; //HuffmanTree是向量类型

typedef struct
{
	char ch;		  //存储字符
	char bits[n + 1]; //存放编码位串
} CodeNode;
typedef CodeNode HuffmanCode[n];

void CreateHuffmanTree(HuffmanTree T);					   //构造Huffman树，T[m-1]为其根结点
void InitHuffmanTree(HuffmanTree T);					   //初始化Huffman树
void InputWeight(HuffmanTree T);						   //输入权值
void SelectMin(HuffmanTree T, int i, int *p1, int *p2);	   //在T中选择两个权最小的根结点
void CharSetHuffmanEncoding(HuffmanTree T, HuffmanCode H); //根据Huffman树T求Huffman编码表H

#define ShowCodeNode(H)                                               \
	do                                                                \
	{                                                                 \
		for (int i = 0; i < n; i++)                                   \
			printf("CodeNode[%d]:\t{ch=%d '%c' bits=%p '%s'}\n",      \
				   i, (int)(H[i].ch), H[i].ch, H[i].bits, H[i].bits); \
	} while (0)

#define ShowHuffmanTree(T)                                                    \
	do                                                                        \
	{                                                                         \
		for (int i = 0; i < m; i++)                                           \
			printf("HuffmanTree[%d]: \t{weight=%.f lchild=%d(.weight=%.f) "   \
				   "rchild=%d(.weight=%.f) parent=%d(.weight=%.f)}\n",        \
				   i,                                                         \
				   T[i].weight, T[i].lchild,                                  \
				   (T[i].lchild || T[i].rchild) ? T[T[i].lchild].weight : -1, \
				   T[i].rchild,                                               \
				   (T[i].lchild || T[i].rchild) ? T[T[i].rchild].weight : -1, \
				   T[i].parent,                                               \
				   (T[i].parent) ? T[T[i].parent].weight : -1);               \
	} while (0)

int main()
{
	HuffmanTree T;
	HuffmanCode H;
	CreateHuffmanTree(T);
	CharSetHuffmanEncoding(T, H);

	ShowCodeNode(H);
	ShowHuffmanTree(T);
	return 0;
}

void CreateHuffmanTree(HuffmanTree T)
{ //构造Huffman树，T[m-1]为其根结点
	int i, p1, p2;
	InitHuffmanTree(T);		//将T初始化
	InputWeight(T);			//输入叶子权值至T[0..n-1]的weight域
	for (i = n; i < m; i++) //共进行n-1次合并，新结点依次存于T[i]中
	{
		SelectMin(T, i - 1, &p1, &p2);
		//在T[0..i-1]中选择两个权最小的根结点，其序号分别为p1和p2
		T[p1].parent = T[p2].parent = i;
		T[i].lchild = p1; //最小权的根结点是新结点的左孩子
		T[i].rchild = p2; //次小权的根结点是新结点的右孩子
		T[i].weight = T[p1].weight + T[p2].weight;
		// printf("i=%d\tp1=%d\tp3=%d \tT[i].weight=%d\tT[p1].weight=%d\tT[p2].weight=%d\n",
		// 	i, p1, p2, T[i].weight, T[p1].weight, T[p2].weight);
	}
}

void InitHuffmanTree(HuffmanTree T)
{ //初始化Huffman树
	int i;
	for (i = 0; i < m; i++)
	{
		T[i].weight = 0;
		T[i].lchild = T[i].rchild = T[i].parent = 0;
	}
}

void InputWeight(HuffmanTree T)
{ //输入权值
	int i;
	int a[n] = {6, 1, 7, 5, 2, 4, 9, 8, 3};
	for (i = 0; i < n; i++)
	{
		// cout << "请输入第" << i + 1 << "个权值：";
		// cin >> T[i].weight;
		T[i].weight = a[i];
	}
}

void SelectMin(HuffmanTree T, int i, int *p1, int *p2)
{ //在T中选择两个权最小的根结点
	int j;
	float min1, min2;
	min1 = min2 = -1;
	for (j = 0; j <= i; j++)
		if (T[j].parent == 0)
		{
			if (T[j].weight < min1 || min1 == -1)
			{
				if (min1 != -1)
				{
					min2 = min1;
					*p2 = *p1;
				}
				min1 = T[j].weight;
				*p1 = j;
			}
			else if (T[j].weight < min2 || min2 == -1)
			{
				min2 = T[j].weight;
				*p2 = j;
			}
		}
}

void CharSetHuffmanEncoding(HuffmanTree T, HuffmanCode H)
{					 //根据Huffman树T求Huffman编码表H
	int c, p, i;	 //c和p分别指示T中孩子和双亲的位置
	char cd[n + 1];	 //临时存放编码
	char str[n + 1]; //存放输入字符串
	int start;		 //指示编码在cd中的起始位置
	cd[n] = '\0';	 //编码结束符
	// cout << "请输入字符串：";
	// cin >> str;
	strcpy(str, "XJASONLYU");

	for (i = 0; i < n; i++) //依次求叶子T[i]的编码
	{
		H[i].ch = str[i];			   //读入叶子T[i]对应的字符
		start = n;					   //编码起始位置的初值
		c = i;						   //从叶子T[i]开始上溯
		while ((p = T[c].parent) != 0) //直至上溯到T[c]是树根为止
		{							   //若T[c]是T[p]的左孩子，则生成代码0；否则生成代码1
			cd[--start] = (T[p].lchild == c) ? '0' : '1';
			c = p; //继续上溯
		}
		strcpy(H[i].bits, &cd[start]); //复制编码位串
	}
	// for (i = 0; i < n; i++)
	// 	cout << "第" << i + 1 << "个字符" << H[i].ch << "的编码为" << H[i].bits << endl;
}
