#include "graph.h"

//创建邻接矩阵, g是指向图的指针变量，m[][MAXVEX]是邻接矩阵，d[]是顶点表,n顶点个数
void CreateGraph(AdjMetrix g, int m[][MAXVEX], DataType d[], int n)
{
    int i, j;
    g->vexs = n; //为图g的顶点个数域赋值
    for (i = 0; i < n; i++)
    { //为图g的顶点表赋值
        g->data[i] = d[i];
        for (j = 0; j < n; j++) //为图g的邻接矩阵赋值
            g->arcs[i][j] = m[i][j];
    }
}

//显示邻接矩阵
void DispGraph(AdjMetrix g)
{
    int i, j;
    cout << "图的顶点：  ";
    for (i = 0; i < g->vexs; i++) //输出图g的顶点
        cout << " " << g->data[i];
    cout << "\n\n图的邻接矩阵:" << endl;
    //
    cout << " *";
    for (i = 0; i < g->vexs; i++) //输出图g的顶点
        cout << " " << g->data[i];
    cout << endl;
    //外层for循环控制对每一个顶点进行遍历
    for (i = 0; i < g->vexs; i++)
    {
        cout << " " << g->data[i];    //输出顶点i的数据
        for (j = 0; j < g->vexs; j++) //在输出顶点i的数据之后，输出顶点i对应在邻接矩阵的第i行的数据
            printf(" %c", ((g->arcs[i][j] == INFINITY) ? '#' : g->arcs[i][j]+'1'-1));
        cout << endl;
    }
    cout << endl;
}

//取顶点k第一个邻接点
int GetFirst(AdjMetrix g, int k)
{
    int i;
    if (k < 0 || k > g->vexs) //顶点表的下标范围在0到g->vexs-1之间，下标即是顶点的序号
    {
        cout << "参数k超出范围" << endl;
        return -1;
    }
    for (i = 0; i < g->vexs; i++)
        if (g->arcs[k][i] == 1) //扫描顶点k所在行的元素值，第一个元素值为1的对应的顶点即是顶点k第一个邻接点
            return i;           //返回顶点k第一个邻接点的序号
    return -1;
}

//取顶点k的邻接点t的下一个邻接点
int GetNext(AdjMetrix g, int k, int t)
{
    int i;
    if (k < 0 || k > g->vexs || t < 0 || t > g->vexs)
    {
        cout << "参数k或t超出范围" << endl;
        return -1;
    }
    for (i = t + 1; i < g->vexs; i++)
        if (g->arcs[k][i] == 1)
            return i;
    return -1;
}

//给顶点k赋值x
void PutVex(AdjMetrix g, int k, DataType x)
{
    if (k < 0 || k > g->vexs)
    {
        cout << "参数k超出范围" << endl;
        return;
    }
    g->data[k] = x;
}

//取顶点k的值
DataType GetVex(AdjMetrix g, int k)
{
    if (k < 0 || k > g->vexs)
    {
        cout << "参数k超出范围" << endl;
        return -1;
    }
    return g->data[k];
}

//连通图深度优先遍历
void DFS(AdjMetrix g, int k, int visited[])
{
    int u;
    cout << " " << g->data[k];
    visited[k] = 1;
    u = GetFirst(g, k);
    while (u != -1)
    {
        if (visited[u] == 0)
            DFS(g, u, visited);
        u = GetNext(g, k, u);
    }
}

typedef struct
{
    DataType adjvex; // 记录权值最小的边的起始点
    weight lowcost;  // 记录该边的权值
} closedge[MAXVEX];

closedge theclose;

static int minimun(AdjMetrix g, closedge close)
{
    int min = INFINITY;
    int min_i = -1;
    for (int i = 0; i < g->vexs; i++)
    {
        //权值为0，说明顶点已经归入最小生成树中；然后每次和min变量进行比较，最后找出最小的。
        if (close[i].lowcost > 0 && close[i].lowcost < min)
        {
            min = close[i].lowcost;
            min_i = i;
        }
    }
    //返回最小权值所在的数组下标
    return min_i;
}

void Prim(AdjMetrix g, int k)
{
    int w, total = 0;

    for (int i = 0; i < g->vexs; i++)
    {
        if (i != k)
        {
            theclose[i].adjvex = k;
            theclose[i].lowcost = g->arcs[k][i];
        }
    }
    theclose[k].lowcost = 0;
    cout << endl;
    // 选择下一个点，并更新辅助数组中的信息
    for (int i = 1; i < g->vexs; i++)
    {
        // 找出权值最小的边所在数组下标
        k = minimun(g, theclose);
        // 输出选择的路径
        w = g->arcs[k][theclose[k].adjvex];
        total += w;
        printf("\t%c <--> %c\tweight: %d\n", g->data[theclose[k].adjvex], g->data[k], w);
        // 归入最小生成树的顶点的辅助数组中的权值设为0
        theclose[k].lowcost = 0;
        // 信息辅助数组中存储的信息，由于此时树中新加入了一个顶点，需要判断，
        // 由此顶点出发，到达其它各顶点的权值是否比之前记录的权值还要小，如果还小，则更新
        for (int j = 0; j < g->vexs; j++)
        {
            if (g->arcs[k][j] < theclose[j].lowcost)
            {
                theclose[j].adjvex = k;
                theclose[j].lowcost = g->arcs[k][j];
            }
        }
    }
    printf("\nWeight of minimum spanning tree is %d\n", total);
}
