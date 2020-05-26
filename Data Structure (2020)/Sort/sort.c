#include <stdio.h>
#include "sort.h"

void show_array(int *arr, int start, int end)
{
    printf("start=%d end=%d\t", start, end);
    for (int i = start; i <= end; i++)
        printf("%d\t", *(arr + i));
    printf("\n");
}

void merge(int *seq, int start, int mid, int end)
{
    //stores the starting position of both parts in temporary variables.
    int p = start, q = mid + 1;

    int tmp[end - start + 1], k = 0;

    for (int i = start; i <= end; i++)
    {
        if (p > mid) //checks if first part comes to an end or not .
            tmp[k++] = seq[q++];

        else if (q > end) //checks if second part comes to an end or not
            tmp[k++] = seq[p++];

        else if (seq[p] < seq[q]) //checks which part has smaller element.
            tmp[k++] = seq[p++];

        else
            tmp[k++] = seq[q++];
    }

    for (int p = 0; p < k; p++)
    {
        /* Now the real array has elements in sorted manner including both parts.*/
        seq[start++] = tmp[p];
    }
}

void merge_sort(int *arr, int start, int end)
{
    if (start < end)
    {
        int mid = (start + end) / 2;   // defines the current array in 2 parts .
        merge_sort(arr, start, mid);   // sort the 1st part of array .
        merge_sort(arr, mid + 1, end); // sort the 2nd part of array.

        // merge the both parts by comparing elements of both the parts.
        merge(arr, start, mid, end);

        show_array(arr, start, end);
    }
}

void quick_sort(int *arr, int start, int end)
{
    if (start >= end)
        return;
    if (start < 0 || end < 0)
        return;

    int piv = arr[start];
    int left = start;
    int right = end;
    int tmp;

    while (left < right)
    {
        while (arr[right] >= piv && left < right)
            right--;
        while (arr[left] <= piv && left < right)
            left++;
        tmp = arr[left];
        arr[left] = arr[right];
        arr[right] = tmp;
    }

    arr[start] = arr[left];
    arr[left] = piv;

    show_array(arr, start, end);

    quick_sort(arr, start, left - 1);
    quick_sort(arr, right + 1, end);
}
