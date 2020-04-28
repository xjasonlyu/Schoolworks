#include <stdio.h>
#include <stdlib.h>
#define MVNum 10

int visited[MVNum];

typedef struct
{
    char vexs[MVNum];
    int arcs[MVNum][MVNum];
    int vexnum, arcnum;
} Graph;

void CreateUDN(Graph &G); //实现细节隐藏
void BFS(Graph G, int v);

void BFSTraverse(Graph G)
{
    int v;
    for (v = 0; v < G.vexnum; ++v)
        visited[v] = 0;
    for (v = 0; v < G.vexnum; ++v)
        if (!visited[v])
            BFS(G, v);
}

int main()
{
    Graph G;
    CreateUDN(G);
    BFSTraverse(G);
    return 0;
}

void BFS(Graph G, int v)
{
    int i;
    if (!visited[v])
    {
        visited[v] = 1;
        printf("%c ", G.vexs[v]);
    }
}

