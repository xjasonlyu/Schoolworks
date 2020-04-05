#include "sqlist.h"

int main(void)
{

	LIST my_List1,my_List2;//定义线标表LIST类型的变量
	DataType Array[]={27,39,2,77,89,66};

	//初始化线性表
	InitList(my_List1);
	InitList(my_List2);

	//向线性表的指定位置插入数据
	ListInsert(my_List1,1,89);
	ListInsert(my_List1,1,77);
	ListInsert(my_List1,3,66);
	ListInsert(my_List1,1,2);
	ListInsert(my_List1,1,39);
	ListInsert(my_List1,1,27);

	for(int i=1; i<=6; i++)
		ListInsert(my_List2,i,Array[i-1]);

	//输出线性表元素
	cout<<"my_list1：";
	TraverseList(my_List1);
	cout<<"my_list2：";
	TraverseList(my_List2);

	return 0;
}