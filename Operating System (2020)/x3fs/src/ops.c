#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "fs.h"

int fs_mkdir(const char *path)
{
    int retval = -1;

    if (!check_filename(path))
    {
        report_error("Filename invalid");
    }

    if (!strcmp(path, ".."))
    {
        report_error("Filename cannot be `..`");
    }

    for (int i = 0; i < sb->fcb_num_per_block; ++i)
    {
        if (fcb_exist(&cur_dir->fcb[i]))
        {
            if (!strncmp(cur_dir->fcb[i].fname, path, FNAME_LENGTH))
            {
                report_error("File exists");
            }
        }
    }

    if (cur_dir->item_num >= sb->fcb_num_per_block)
    {
        report_error("Current FCB is full, no more item is allowed");
    }

    bid_t bid;
    if ((bid = find_free_block()) == 0)
    {
        report_error("No free space");
    }

    fat[bid] = BLK_END;
    sb->free_block_num--;

    fcb_t *fcb = &(cur_dir->fcb)[cur_dir->item_num];
    strncpy(fcb->fname, path, FNAME_LENGTH);
    fcb->size = 0;
    fcb->attrs = EXIST_MASK | DIR_MASK;
    fcb->bid = bid;
    fcb->created_time = time(NULL);
    fcb->modified_time = fcb->created_time;
    cur_dir->item_num++;
    // update current dir block to image file
    pwrite(fd, cur_dir, sizeof(blk_t), offset_of(cur_dir->bid));

    dir_t *new_dir = (dir_t *)calloc(1, sizeof(blk_t));
    new_dir->magic = MAGIC_DIR;
    new_dir->item_num = 0;
    new_dir->bid = bid;
    new_dir->parent_bid = cur_dir->bid;
    // write new dir block to image file
    pwrite(fd, new_dir, sizeof(blk_t), offset_of(bid));
    free(new_dir);

    retval = 0;

out:
    return retval;
}

/*
    rmdir dirname
*/
int fs_rmdir(const char *path)
{
    int retval = -1;

    if (!check_filename(path))
    {
        report_error("Filename invalid");
    }

    bool found = false;
    for (int i = 0; i < sb->fcb_num_per_block; ++i)
    {
        if (fcb_exist(&cur_dir->fcb[i]))
        {
            if (!strncmp(cur_dir->fcb[i].fname, path, FNAME_LENGTH))
            {
                found = true;

                if (fcb_isfile(&cur_dir->fcb[i]))
                {
                    /* is file */
                    // fs_rm(cur_dir->fcb[i].fname);
                    report_error("Not a directory");
                }

                // check if sub dir empty
                int item_num = 0;
                dir_t *sub_dir = malloc(sizeof(blk_t));
                pread(fd, sub_dir, sizeof(blk_t), offset_of(cur_dir->fcb[i].bid));
                item_num = sub_dir->item_num;
                free(sub_dir);

                if (item_num > 0)
                {
                    report_error("Directory not empty");
                }

                bid_t bid, next_bid = cur_dir->fcb[i].bid;
                while (next_bid > 1)
                {
                    bid = next_bid;
                    next_bid = fat[bid];
                    fat[bid] = BLK_FREE;
                    sb->free_block_num++;
                }
                memset(&cur_dir->fcb[i], 0, sizeof(fcb_t));

                for (int j = i + 1; j < sb->fcb_num_per_block; ++j)
                {
                    if (fcb_exist(&cur_dir->fcb[j]))
                    {
                        memcpy(&cur_dir->fcb[j - 1], &cur_dir->fcb[j], sizeof(fcb_t));
                        memset(&cur_dir->fcb[j], 0, sizeof(fcb_t));
                    }
                }

                cur_dir->item_num--;
            }
        }
        else
        {
            break;
        }
    }

    if (!found)
    {
        report_error("No such file or directory");
    }
    else
    {
        pwrite(fd, cur_dir, sizeof(blk_t), offset_of(cur_dir->bid));
    }

    retval = 0;
out:
    return retval;
}

/*
    ls
*/
int fs_ls()
{
    char buffer[0x10];

    printf("total %d\n", cur_dir->item_num);
    for (int i = 0; i < cur_dir->item_num; ++i)
    {
        if (fcb_exist(&cur_dir->fcb[i]))
        {
            strftime(buffer, 0x0F, "%b %d %H:%M", localtime(&cur_dir->fcb[i].modified_time));
            printf("%s %6d %7s %13s %-9s\n",
                   (fcb_isdir(&cur_dir->fcb[i]) ? "d" : "-"),
                   cur_dir->fcb[i].bid,
                   format_size(cur_dir->fcb[i].size),
                   buffer,
                   cur_dir->fcb[i].fname);
        }
        else
        {
            break;
        }
    }

    return 0;
}

/* lsof */
int fs_lsof()
{
    printf("FD\tFilename\tOffset\tBID\tMode\tModified\n");
    for (int i = 0; i < MAX_FD; ++i)
    {
        if (ofs[i].not_empty)
        {
            printf("%d\t%-9s\t%lld\t%u\t%-4u\t%-8s\n", i, ofs[i].fcb.fname, ofs[i].off,
                   ofs[i].at_bid, ofs[i].oflag, (ofs[i].is_fcb_modified ? "true" : "false"));
        }
    }

    return 0;
}

/*
    stat
*/
int fs_stat()
{
    printf("Size\tBlocks\tUsed\tAvail\tCap\tFAT\tFCB\tBID\n");
    printf("%s\t%d\t%d\t%d\t%.1f%%\t%d\t%d\t%d\n", format_size(sb->total_size),
           sb->total_block_num, sb->total_block_num - sb->free_block_num, sb->free_block_num,
           100.0 * (float)(sb->total_block_num - sb->free_block_num) / sb->total_block_num,
           sb->fat_block_num * 2, sb->fcb_num_per_block, sb->data_start_bid);

    return 0;
}

/*
    cd dirname
*/
int fs_cd(const char *path)
{
    return parse_path(path, cur_dir);
}

/*
    create filename
*/
int fs_create(const char *path)
{
    int retval = -1;

    if (!check_filename(path))
    {
        report_error("Filename invalid");
    }

    if (!strcmp(path, ".."))
    {
        report_error("Filename cannot be `..`");
    }

    if (cur_dir->item_num >= sb->fcb_num_per_block)
    {
        report_error("Current FCB is full, no more item is allowed");
    }

    // check if file already existed
    bool found = false;
    for (int i = 0; i < sb->fcb_num_per_block; ++i)
    {
        if (fcb_exist(&cur_dir->fcb[i]))
        {
            if (!strncmp(cur_dir->fcb[i].fname, path, FNAME_LENGTH))
            {
                found = true;
                break;
            }
        }
    }
    if (found)
    {
        report_error("File exists");
    }

    fcb_t *fcb = &cur_dir->fcb[cur_dir->item_num];
    strncpy(fcb->fname, path, FNAME_LENGTH);
    fcb->size = 0;
    fcb->bid = 0;
    fcb->attrs = EXIST_MASK;
    fcb->created_time = time(NULL);
    fcb->modified_time = fcb->created_time;

    cur_dir->item_num++;

    // save current dir block to image file
    pwrite(fd, cur_dir, sizeof(blk_t), offset_of(cur_dir->bid));

    retval = 0;

out:
    return retval;
}

/*
    open filename oflag
*/
int fs_open(const char *filename, int oflag)
{
    int retval = -1; /* opened fd */

    if (!check_filename(filename))
    {
        report_error("Filename invalid");
    }

    if (!check_oflag(oflag))
    {
        report_error("Open flag invalid");
    }

    bool found = false;
    int available_fd = -1;
find:
    for (int i = 0; i < cur_dir->item_num; ++i)
    {
        if (fcb_exist(&cur_dir->fcb[i]))
        {
            if (!strncmp(cur_dir->fcb[i].fname, filename, FNAME_LENGTH))
            {
                found = true;
                if (fcb_isdir(&cur_dir->fcb[i]))
                {
                    report_error("Cannot open a directory");
                }

                for (int index = 0; index < MAX_FD; ++index)
                {
                    if (ofs[index].not_empty)
                    {
                        if (!strncmp(ofs[index].fcb.fname, filename, FNAME_LENGTH))
                        {
                            // retval = index;
                            // goto out;
                            report_error("File already opened");
                        }
                    }
                }

                if ((available_fd = find_available_fd()) < 0)
                {
                    report_error("Too many opened files");
                }
                memcpy(&ofs[available_fd].fcb, &cur_dir->fcb[i], sizeof(fcb_t));
                ofs[available_fd].not_empty = true;
                ofs[available_fd].at_bid = cur_dir->bid;
                ofs[available_fd].fcb_id = i;
                ofs[available_fd].off = 0;
                ofs[available_fd].oflag = oflag;
                ofs[available_fd].is_fcb_modified = false;
                retval = available_fd;
                break;
            }
        }
        else
        {
            break;
        }
    }

    if (!found)
    {
        if (check_create(oflag))
        {
            if (!fs_create(filename))
                goto find;
        }
        else
        {
            report_error("File not found");
        }
    }

out:
    return retval;
}

/*
    seek fd offset whence
*/
off_t fs_seek(int target_fd, off_t offset, int whence)
{
    off_t retval = -1;

    // fd validation check
    if (!check_opened_fd(target_fd))
    {
        report_error("Illegal fd");
    }

    // fd empty
    if (!ofs[target_fd].not_empty)
    {
        report_error("Illegal fd");
    }

    switch (whence)
    {
    case SEEK_SET:
        ofs[target_fd].off = offset;
        break;
    case SEEK_CUR:
        ofs[target_fd].off += offset;
        break;
    case SEEK_END:
        ofs[target_fd].off = ofs[target_fd].fcb.size + offset;
        break;
    default:
        goto out;
    }

    retval = ofs[target_fd].off;

out:
    return retval;
}

/*
    close fd
*/
int fs_close(int target_fd)
{
    int retval = -1;

    // fd validation check
    if (!check_opened_fd(target_fd))
    {
        report_error("Illegal fd");
    }

    // fd empty
    if (!ofs[target_fd].not_empty)
    {
        report_error("Illegal fd");
    }

    // save modified file
    if (ofs[target_fd].is_fcb_modified)
    {
        dir_t *dir = (dir_t *)malloc(sizeof(blk_t));
        pread(fd, dir, sizeof(blk_t), offset_of(ofs[target_fd].at_bid));
        memcpy(&dir->fcb[ofs[target_fd].fcb_id], &ofs[target_fd].fcb, sizeof(fcb_t));
        pwrite(fd, dir, sizeof(blk_t), offset_of(dir->bid));

        if (ofs[target_fd].at_bid == cur_dir->bid)
        {
            memcpy(&cur_dir->fcb[ofs[target_fd].fcb_id], &ofs[target_fd].fcb, sizeof(fcb_t));
        }

        free(dir);
    }

    // reset to zero
    memset(&ofs[target_fd], 0, sizeof(of_t));

    retval = 0;

out:
    return retval;
}

/*
    write fd buf size
*/
ssize_t fs_write(int target_fd, const char *buf, size_t size)
{
    ssize_t retval = -1;

    if (!check_opened_fd(target_fd))
    {
        report_error("Illegal fd");
    }

    if (!ofs[target_fd].not_empty)
    {
        report_error("Illegal fd");
    }

    if (!check_write(ofs[target_fd].oflag))
    {
        report_error("Not writable");
    }

    if (!size)
    {
        return 0;
    }

    // alloc new bid
    bid_t bid = ofs[target_fd].fcb.bid;
    if (bid <= 1)
    {
        bid_t new_bid = find_free_block();
        if (!new_bid)
        {
            report_error("No free space");
        }
        // ofs[target_fd].fcb.bid = new_bid;
        fat[new_bid] = BLK_END;
        sb->free_block_num--;
        bid = new_bid;
        ofs[target_fd].fcb.bid = new_bid;
        ofs[target_fd].is_fcb_modified = true;
        // set new modified timestamp
        ofs[target_fd].fcb.modified_time = time(NULL);
    }

    // find bid to continue
    off_t off = ofs[target_fd].off;
    while (off >= BLOCK_SIZE)
    {
        if (off == BLOCK_SIZE)
        {
            bid_t new_bid = find_free_block();
            if (!new_bid)
            {
                report_error("No free space");
            }
            if (bid)
            {
                fat[bid] = new_bid;
            }
            fat[new_bid] = BLK_END;
            sb->free_block_num--;
            bid = new_bid;
            off = 0;
            break;
        }
        bid = fat[bid];
        off -= BLOCK_SIZE;
    }

    //  ofs[target_fd].fcb.size = 0;

    size_t count = size;
    ssize_t written = 0;
    char *pbuf = (char *)buf;

    while (1)
    {
        if (count <= (BLOCK_SIZE - off)) /* direct append */
        {
            int actually_wrote = pwrite(fd, pbuf, count, offset_of(bid) + off);
            ofs[target_fd].off += actually_wrote;
            ofs[target_fd].fcb.size += actually_wrote;
            ofs[target_fd].is_fcb_modified = true;
            // set new modified timestamp
            ofs[target_fd].fcb.modified_time = time(NULL);
            written += actually_wrote;
            break;
        }
        else /* need new blocks to append */
        {
            int n = BLOCK_SIZE - off;
            int actually_wrote = pwrite(fd, pbuf, n, offset_of(bid) + off);
            ofs[target_fd].off += actually_wrote;
            ofs[target_fd].fcb.size += actually_wrote;
            ofs[target_fd].is_fcb_modified = true;
            // set new modified timestamp
            ofs[target_fd].fcb.modified_time = time(NULL);
            written += actually_wrote;
            bid_t new_bid = find_free_block();
            if (!new_bid)
            {
                report_error("No more free space");
            }
            if (bid)
            {
                fat[bid] = new_bid;
            }
            fat[new_bid] = BLK_END;
            sb->free_block_num--;
            bid = new_bid;
            pbuf += n;
            count -= n;
            off = 0;
        }
    }

    retval = written;

out:
    // if (buf)
    //     free(buf);
    return retval;
}

/*
    read fd buf size
*/
ssize_t fs_read(int target_fd, const char *buf, size_t size)
{
    ssize_t retval = -1;

    if (!check_opened_fd(target_fd))
    {
        report_error("Illegal fd");
    }

    if (!ofs[target_fd].not_empty)
    {
        report_error("Illegal fd");
    }

    if (!check_read(ofs[target_fd].oflag))
    {
        report_error("Not readable");
    }

    if (!size || !ofs[target_fd].fcb.size)
    {
        // file empty
        return 0;
    }

    // if (size > ofs[target_fd].fcb.size)
    //     size = ofs[target_fd].fcb.size;

    size_t count = min(size, ofs[target_fd].fcb.size - ofs[target_fd].off);

    bid_t bid = ofs[target_fd].fcb.bid;
    off_t off = ofs[target_fd].off;
    while (off >= BLOCK_SIZE)
    {
        bid = fat[bid];
        off -= BLOCK_SIZE;
    }

    size_t n = 0;
    ssize_t written = 0;
    char *pbuf = (char *)buf;

    while (ofs[target_fd].off < ofs[target_fd].fcb.size)
    {
        if (count <= (BLOCK_SIZE - off))
        {
            blk_t *blk = (blk_t *)malloc(sizeof(blk_t));
            pread(fd, blk, count, offset_of(bid) + off);
            n = min(count, ofs[target_fd].fcb.size - ofs[target_fd].off);
            memcpy(pbuf, blk, n);
            pbuf += n;

            ofs[target_fd].off += n;
            count -= n;
            written += n;

            free(blk);
            break;
        }
        else
        {
            blk_t *blk = (blk_t *)malloc(sizeof(blk_t));
            n = BLOCK_SIZE - off;
            pread(fd, blk, n, offset_of(bid) + off);

            memcpy(pbuf, blk, n);
            pbuf += n;

            ofs[target_fd].off += n;
            count -= n;
            written += n;
            // TODO
            off = 0;
            free(blk);
        }
    }

    retval = written;

out:
    return retval;
}

/*
    rm filename
*/
int fs_rm(const char *path)
{
    int retval = -1;

    if (!check_filename(path))
    {
        report_error("Filename invalid");
    }

    bool found = false;
    for (int i = 0; i < sb->fcb_num_per_block; ++i)
    {
        if (fcb_exist(&cur_dir->fcb[i]))
        {
            if (!strncmp(cur_dir->fcb[i].fname, path, FNAME_LENGTH))
            {
                found = true;

                if (fcb_isdir(&cur_dir->fcb[i]))
                {
                    report_error("Is a directory");
                }

                fcb_t *fcb = &cur_dir->fcb[i];
                bid_t bid, next_bid = fcb->bid;
                while (next_bid > 1)
                {
                    bid = next_bid;
                    next_bid = fat[bid];
                    fat[bid] = BLK_FREE; /* set block to free */
                    sb->free_block_num++;
                }
                memset(fcb, 0, sizeof(fcb_t));

                // align fcb array
                for (int j = i + 1; j < sb->fcb_num_per_block; ++j)
                {
                    if (fcb_exist(&cur_dir->fcb[j]))
                    {
                        memcpy(&cur_dir->fcb[j - 1], &cur_dir->fcb[j], sizeof(fcb_t));
                        memset(&cur_dir->fcb[j], 0, sizeof(fcb_t));
                    }
                }

                cur_dir->item_num--;
            }
        }
        else
        {
            break;
        }
    }

    if (!found)
    {
        report_error("No such file or directory");
    }

    pwrite(fd, cur_dir, sizeof(blk_t), offset_of(cur_dir->bid));

    retval = 0;

out:
    return retval;
}

/*
    rename old new
*/
int fs_rename(const char *old, const char *new)
{
    int retval = -1;

    if (!check_filename(old))
    {
        report_error("Filename invalid");
    }

    if (!check_filename(new))
    {
        report_error("Filename invalid");
    }

    int index = -1;
    bool found = false;
    for (int i = 0; i < sb->fcb_num_per_block; ++i)
    {
        if (fcb_exist(&cur_dir->fcb[i]))
        {
            if (!strncmp(cur_dir->fcb[i].fname, new, FNAME_LENGTH))
            {
                report_error("File exists");
            }

            if (!strncmp(cur_dir->fcb[i].fname, old, FNAME_LENGTH))
            {
                found = true;
                index = i;
            }
        }
        else
        {
            break;
        }
    }

    if (!found)
    {
        report_error("No such file or directory");
    }

    // rename
    strncpy(cur_dir->fcb[index].fname, new, FNAME_LENGTH);

    pwrite(fd, cur_dir, sizeof(blk_t), offset_of(cur_dir->bid));

    retval = 0;

out:
    return retval;
}
