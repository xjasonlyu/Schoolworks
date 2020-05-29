#include <stdlib.h>

#include "fs.h"

#define len_of(p) __len_of__((void **)(p))

size_t __len_of__(void **p)
{
    size_t n = 0;
    while (*p++)
        n++;
    return n;
}

bid_t find_free_block()
{
    for (int i = sb->data_start_bid; i < sb->total_block_num; ++i)
    {
        if (fat[i] == BLK_FREE)
        {
            return i;
        }
    }
    return 0;
}

int find_available_fd()
{
    for (int i = 0; i < MAX_FD; ++i)
    {
        if (!ofs[i].not_empty)
        {
            return i;
        }
    }
    return -1;
}
