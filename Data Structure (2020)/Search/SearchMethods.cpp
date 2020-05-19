//静态查找表，有序表，折半查找
#include <iostream>
using namespace std;

#define MAXSIZE 10
#define NotFound 0
typedef int ElementType;

typedef int Position;
typedef struct LNode *List;

struct LNode
{
    ElementType Data[MAXSIZE + 1];
    int Length;
};

void ReadInput(List &L);

Position BinarySearch(List L, ElementType X);

int main()
{
    List L;
    ElementType X;
    Position P;

    ReadInput(L);

    cin >> X;
    P = BinarySearch(L, X);

    if (P == 0)
        cout << "NOT FOUND" << endl;
    else
        cout << P << endl;
    return 0;
}

void ReadInput(List &L)
{
    int i;
    L = new LNode;
    cin >> L->Length;
    for (i = 1; i <= L->Length; i++)
        cin >> L->Data[i];
}

/* 实现折半（二分）查找算法 */
Position BinarySearch(List L, ElementType X)
{
    int low = 1;
    int mid = 0;
    int high = L->Length;

    while (low <= high)
    {
        mid = (low + high) / 2;
        // printf(">>> %d: %d\n", mid, L->Data[mid]);
        if (L->Data[mid] == X)
            return mid;
        else if (L->Data[mid] > X)
            high = mid - 1;
        else
            low = mid + 1;
    }

    return 0;
}
