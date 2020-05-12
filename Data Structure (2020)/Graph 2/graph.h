#include <iostream>

using namespace std;

#define MAXVEX 50 //最大顶点个数
#define INFINITY 0xFFFF

typedef int weight;    //邻接矩阵元素类型
typedef char DataType; //顶点数据类型

//邻接矩阵类型
typedef struct
{
    weight arcs[MAXVEX][MAXVEX]; //邻接矩阵
    DataType data[MAXVEX];       //一维数组顶点表
    int vexs;                    //顶点个数
} MGraph, *AdjMetrix;

//创建邻接矩阵, g是指向图的指针变量，m[][MAXVEX]是邻接矩阵，d[]是顶点表,n顶点个数
void CreateGraph(AdjMetrix g, int m[][MAXVEX], DataType d[], int n);

//显示邻接矩阵
void DispGraph(AdjMetrix g);

//取第一个邻接点
int GetFirst(AdjMetrix g, int k);

//取下一个邻接点
int GetNext(AdjMetrix g, int k, int t);

//给顶点k赋值x
void PutVex(AdjMetrix g, int k, DataType x);

//取顶点k的值
DataType GetVex(AdjMetrix g, int k);

//连通图深度优先遍历
void DFS(AdjMetrix g, int k, int visited[]);

// Prim Algorithm
void Prim(AdjMetrix g, int k);
