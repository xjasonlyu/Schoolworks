#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "fs.h"

#define MAXSIZE (256 * 1024 * 1024)

int main(int argc, char *argv[])
{
    int retval = -1;

    if (argc < 3)
    {
        puts("Usage: ./mkx3fs filename size");
        goto out;
    }

    int total_size = -1;
    char *ssize = argv[2];
    if (strlen(ssize) < 2)
    {
        puts("invalid size");
        goto out;
    }
    switch (ssize[strlen(ssize) - 1])
    {
    case 'm':
    case 'M':
        ssize[strlen(ssize) - 1] = '\0';
        total_size = atoi(ssize) * 1024 * 1024;
        break;
    case 'k':
    case 'K':
        ssize[strlen(ssize) - 1] = '\0';
        total_size = atoi(ssize) * 1024;
        break;
    case 'B':
    default:
        total_size = atoi(ssize);
        break;
    }

    if (total_size < 0 || total_size > MAXSIZE)
    {
        puts("disk file size too small or too large");
        goto out;
    }
    if (total_size % BLOCK_SIZE)
    {
        puts("disk file size must be aligned with 4KB");
        goto out;
    }

    blk_t *blk = (blk_t *)malloc(sizeof(blk_t));
    int fd = open(argv[1], O_WRONLY | O_CREAT,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);

    // super block
    memset(blk, 0, sizeof(blk_t));
    sb_t *sb = (sb_t *)blk;
    sb->magic = MAGIC_SUPERBLOCK;
    sb->total_size = total_size;
    sb->total_block_num = total_size / BLOCK_SIZE;
    sb->fat_block_num = ((sb->total_block_num - 1) / (BLOCK_SIZE / 2)) + 1;
    sb->fcb_num_per_block = (BLOCK_SIZE - sizeof(dir_t)) / sizeof(fcb_t);
    sb->data_start_bid = 2 * sb->fat_block_num + 2;
    // 1 superblock + 2 FAT blocks + 1 root dir
    sb->free_block_num = sb->total_block_num - 2 * sb->fat_block_num - 2;
    sb->fat_crc = 0;
    write(fd, sb, sizeof(blk_t));
    printf("size=%d total_blocks=%d fat_blocks=%d fcbs=%d sbid=%d\n",
           sb->total_size, sb->total_block_num, sb->fat_block_num,
           sb->fcb_num_per_block, sb->data_start_bid);
    puts("superblock ok");

    sb = malloc(sizeof(sb_t));
    memcpy(sb, blk, sizeof(sb_t));

    // fat1
    memset(blk, 0, sizeof(blk_t));
    bid_t *fat = (bid_t *)blk;
    for (int i = 0; i < sb->data_start_bid; ++i)
    {
        fat[i] = BLK_END;
    }
    write(fd, fat, sizeof(blk_t));
    memset(blk, 0, sizeof(blk_t));
    for (int i = 1; i < sb->fat_block_num; ++i)
    {
        write(fd, fat, sizeof(blk_t));
    }
    puts("fat1 ok");

    // fat2
    for (int i = 0; i < sb->data_start_bid; ++i)
    {
        fat[i] = BLK_END;
    }
    write(fd, fat, sizeof(blk_t));
    memset(blk, 0, sizeof(blk_t));
    for (int i = 1; i < sb->fat_block_num; ++i)
    {
        write(fd, fat, sizeof(blk_t));
    }
    puts("fat2 ok");

    // root directory
    memset(blk, 0, sizeof(blk_t));
    dir_t *root_dir = (dir_t *)blk;
    root_dir->magic = MAGIC_DIR;
    root_dir->item_num = 0;
    root_dir->bid = sb->data_start_bid - 1; // usually is 3
    root_dir->parent_bid = sb->data_start_bid - 1;
    // .
    strcpy(root_dir->fcb[0].fname, ".");
    root_dir->fcb[0].size = 0;
    root_dir->fcb[0].bid = root_dir->bid;
    root_dir->fcb[0].src_bid = root_dir->bid;
    root_dir->fcb[0].attrs = EXIST_MASK | DIR_MASK;
    root_dir->fcb[0].created_time = time(NULL);
    root_dir->fcb[0].modified_time = root_dir->fcb[0].created_time;
    root_dir->item_num++;
    // ..
    strcpy(root_dir->fcb[1].fname, "..");
    root_dir->fcb[1].size = 0;
    root_dir->fcb[1].bid = root_dir->parent_bid;
    root_dir->fcb[1].src_bid = root_dir->parent_bid;
    root_dir->fcb[1].attrs = EXIST_MASK | DIR_MASK;
    root_dir->fcb[1].created_time = time(NULL);
    root_dir->fcb[1].modified_time = root_dir->fcb[0].created_time;
    root_dir->item_num++;

    write(fd, root_dir, sizeof(blk_t));
    puts("root directory ok");

    // remains
    memset(blk, 0, sizeof(blk_t));
    for (int i = 2 + sb->fat_block_num; i < sb->total_block_num; ++i)
    {
        write(fd, blk, sizeof(blk_t));
    }
    puts("remains ok");

    close(fd);

    free(sb);
    free(blk);

    retval = 0;

out:
    return retval;
}
