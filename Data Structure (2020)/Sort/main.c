#include <stdio.h>
#include "sort.h"

#define MAXLEN 0xFF

#define print_array(S, arr, len)          \
    do                                    \
    {                                     \
        printf(S "\t");                   \
        for (int i = 0; i < (len); i++)   \
            printf("%d\t", *((arr) + i)); \
        putchar('\n');                    \
    } while (0)

int main()
{
    // an unsorted array to test
    // int seq[] = {25, 46, 47, 6, 37, 12, 44, 9, 1, 39};
    // int len = sizeof(seq) / sizeof(int);

    int seq[MAXLEN] = {0};
    int len = 0;

    printf("input array length: ");
    scanf("%d", &len);

    for (int i=0;i<len;i++){
        scanf("%d", &seq[i]);
    }

    print_array("init array", seq, len);

    merge_sort(seq, 0, len - 1);
    // quick_sort(seq, 0, len - 1);

    print_array("sorted array", seq, len);

    return 0;
}
