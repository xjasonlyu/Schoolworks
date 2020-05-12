#include "graph.h"

#define NUM 6

int main()
{
    MGraph gg;         //定义图结构变量gg
    AdjMetrix g = &gg; //定义指针变量g指向图gg

    char d[] = {'A', 'B', 'C', 'D', 'E', 'F'}; //定义一维数组d作为顶点表

    //定义一维数组visited并初始化，作为访问标记。
    int visited[NUM] = {0};

    //定义二维数组m为邻接矩阵
    int m[][MAXVEX] = {{INFINITY, 6, 7, INFINITY, 1, 4},
                       {6, INFINITY, INFINITY, 2, 5, INFINITY},
                       {7, INFINITY, INFINITY, 8, INFINITY, 2},
                       {INFINITY, 2, 8, INFINITY, 3, 9},
                       {1, 5, INFINITY, 3, INFINITY, INFINITY},
                       {4, INFINITY, 2, 9, INFINITY, INFINITY}};

    //已知图的顶点表和邻接矩阵数据，创建图gg的邻接矩阵
    CreateGraph(g, m, d, NUM);

    //显示邻接矩阵
    DispGraph(g);
    cout << "Prim Min Spanning Tree: " << endl;
    Prim(g, 0);
    cout << endl;

    return 0;
}
