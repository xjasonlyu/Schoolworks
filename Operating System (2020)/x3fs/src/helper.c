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

    memset(name, 0, FNAME_LENGTH);

    if (dir->bid == root_bid)
    {
        // root dir
        strncpy(name, "/", FNAME_LENGTH);
    }
    else
    {
        dir_t *parent_dir = (dir_t *)malloc(sizeof(blk_t));
        pread(fd, parent_dir, sizeof(blk_t), offset_of(dir->parent_bid));
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

char *get_abspath(dir_t *dir)
{
    static char path[0xFF] = {0};

    if (dir->bid == root_bid)
    {
        // root dir
        memset(path, 0, 0xFF);
        strncpy(path, "/", FNAME_LENGTH);
    }
    else
    {
        dir_t *parent_dir = (dir_t *)malloc(sizeof(blk_t));
        pread(fd, parent_dir, sizeof(blk_t), offset_of(dir->parent_bid));

        // recursive get
        get_abspath(parent_dir);

        if (path[strlen(path) - 1] != '/')
            strcat(path, "/");
        strncat(path, get_dirname(dir), FNAME_LENGTH);

        if (parent_dir)
            free(parent_dir);
    }

    return path;
}
