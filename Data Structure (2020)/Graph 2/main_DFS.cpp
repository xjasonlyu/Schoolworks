#include "graph.h"

#define NUM 6

int main() {
    MGraph gg;         //定义图结构变量gg
    AdjMetrix g = &gg; //定义指针变量g指向图gg

    char d[] = {'A', 'B', 'C', 'D', 'E', 'F'}; //定义一维数组d作为顶点表

    //定义一维数组visited并初始化，作为访问标记。
    int visited[NUM] = {0};

    //定义二维数组m为邻接矩阵
    int m[][MAXVEX] = {{0, 1, 1, 0, 1, 1},
                       {1, 0, 0, 1, 1, 0},
                       {1, 0, 0, 1, 0, 1},
                       {0, 1, 1, 0, 1, 1},
                       {1, 1, 0, 1, 0, 0},
                       {1, 0, 1, 1, 0, 0}};

    //已知图的顶点表和邻接矩阵数据，创建图gg的邻接矩阵
    CreateGraph(g, m, d, NUM);

    //显示邻接矩阵
    DispGraph(g);
    cout << "深度优先遍历：";
    DFS(g, 0, visited);
    cout << endl;

    return 0;
}
