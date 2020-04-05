#include <iostream>
#include "sqlist.h"

using namespace std;

int main(void)
{
	// 定义线标表LIST类型的变量
	LIST books;
	// 定义5本书的信息
	DataType Array[]={
		{ .name = "War and Peace", .number = "001", .price = 10.0 },
		{ .name = "Lolita", .number = "002", .price = 13.0 },
		{ .name = "Hamlet", .number = "003", .price = 11.5 },
		{ .name = "The Odyssey", .number = "004", .price = 16.0 },
		{ .name = "Crime and Punishment", .number = "005", .price = 9.1 },
	};

	// 初始化线性表
	InitList(books);

	// 插入5本书的信息
	for(int i=0; i<5; i++)
		ListInsert(books, i+1, Array[i]);

	// 输出线性表元素
	cout << "Total Books:" << endl;
	TraverseList(books);

	// 输出第3本书的信息
	cout << "The 3rd Book:" << endl;
	DataType d;
	if (GetElem(books, 3, &d))
		cout << d << endl;

	if (Find(books, d))
		cout << "Book \"" << d.name << "\" Found!" << endl;
	else
		cout << "Book \"" << d.name << "\" Not Found!" << endl;

	return 0;
}