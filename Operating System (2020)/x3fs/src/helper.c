#include <stdlib.h>
#include <unistd.h>

#include "fs.h"

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

char *get_dirname(dir_t *dir)
{
    static char name[FNAME_LENGTH] = {0};

    if (dir->parent_bid == dir->bid)
    {
        // root dir
        strncpy(name, "/", FNAME_LENGTH);
    }
    else
    {
        dir_t *parent_dir = (dir_t *)malloc(sizeof(blk_t));
        pread(fd, parent_dir, sizeof(blk_t), offset_of(cur_dir->parent_bid));
        for (int i = 0; i < parent_dir->item_num; ++i)
        {
            if (parent_dir->fcb[i].bid == dir->bid)
            {
                strncpy(name, parent_dir->fcb[i].fname, FNAME_LENGTH);
                break;
            }
        }

        if (parent_dir)
            free(parent_dir);
    }

    return name;
}
