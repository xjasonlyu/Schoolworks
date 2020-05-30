#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#include "fs.h"

int fs_mkdir(const char *path)
{
    int retval = -1;

    if (!check_filename_length(path))
    {
        report_error("mkdir: dirname too long");
    }

    if (!strcmp(path, ".."))
    {
        report_error("mkdir: dirname cannot be `..`");
    }

    for (int i = 0; i < sb->fcb_num_per_block; ++i)
    {
        if (fcb_exist(&cur_dir->fcb[i]))
        {
            if (!strncmp(cur_dir->fcb[i].fname, path, FNAME_LENGTH))
            {
                report_error("mkdir: File exists");
            }
        }
    }

    if (cur_dir->item_num >= sb->fcb_num_per_block)
    {
        report_error("mkdir: current FCB is full, no more item is allowed");
    }

    bid_t bid;
    if ((bid = find_free_block()) == 0)
    {
        report_error("mkdir: no free space");
    }

    fat[bid] = BLK_END;

    fcb_t *fcb = &(cur_dir->fcb)[cur_dir->item_num];
    strncpy(fcb->fname, path, FNAME_LENGTH);
    fcb->size = 0;
    fcb->attrs = EXIST_MASK | DIR_MASK;
    fcb->bid = bid;
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

    if (!check_filename_length(path))
    {
        report_error("rmdir: filename too long");
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
                    report_error("rmdir: Not a directory");
                }

                // check if sub dir empty
                int item_num = 0;
                dir_t *sub_dir = malloc(sizeof(blk_t));
                pread(fd, sub_dir, sizeof(blk_t), offset_of(cur_dir->fcb[i].bid));
                item_num = sub_dir->item_num;
                free(sub_dir);

                if (item_num > 0)
                {
                    report_error("rmdir: Directory not empty");
                }
                /*
                else
                {
                    fs_cd(cur_dir->fcb[i].fname);
                    
                    for (int j = 0; j < sb->fcb_num_per_block; ++j)
                    {
                        if (fcb_exist(&cur_dir->fcb[j]))
                        {
                            if (!fcb_isdir(&cur_dir->fcb[j]))
                            {
                                fs_rm(cur_dir->fcb[j].fname);
                            }
                            else
                            {
                                fs_rmdir(cur_dir->fcb[j].fname);
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                    fs_cd("..");
                }*/

                bid_t bid, next_bid = cur_dir->fcb[i].bid;
                while (next_bid > 1)
                {
                    bid = next_bid;
                    next_bid = fat[bid];
                    fat[bid] = BLK_FREE;
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
        report_error("rmdir: No such file or directory");
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
    for (int i = 0; i < cur_dir->item_num; ++i)
    {
        if (fcb_exist(&cur_dir->fcb[i]))
        {
            printf("%9s [%s\tbid=%d\tsize=%d]\n",
                   cur_dir->fcb[i].fname,
                   (fcb_isdir(&cur_dir->fcb[i]) ? "dir" : "file"),
                   cur_dir->fcb[i].bid,
                   cur_dir->fcb[i].size);
        }
        else
        {
            break;
        }
    }

    return 0;
}

/*
    cd dirname
*/
int fs_cd(const char *path)
{
    int retval = -1;

    if (!check_filename_length(path))
    {
        report_error("cd: filename too long");
    }

    bool found = false;

    if (!strncmp(path, "..", FNAME_LENGTH)) /* parent dir */
    {
        found = true;
        if (cur_dir->bid != root_bid)
            pread(fd, cur_dir, sizeof(blk_t), offset_of(cur_dir->parent_bid));
    }
    else if (!strncmp(path, "/", FNAME_LENGTH)) /* root dir */
    {
        found = true;
        if (cur_dir->bid != root_bid)
            pread(fd, cur_dir, sizeof(blk_t), offset_of(sb->data_start_bid - 1));
    }
    else /* sub dir */
    {
        for (int i = 0; i < cur_dir->item_num; ++i)
        {
            if (fcb_exist(&cur_dir->fcb[i]))
            {
                if (!strncmp(cur_dir->fcb[i].fname, path, FNAME_LENGTH))
                {
                    found = true;
                    if (fcb_isfile(&cur_dir->fcb[i]))
                    {
                        report_error("cd: not a directory");
                    }
                    pread(fd, cur_dir, sizeof(blk_t), offset_of(cur_dir->fcb[i].bid));
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    if (!found)
    {
        report_error("cd: no such file or directory");
    }

    retval = 0;

out:
    return retval;
}

/*
    create filename
*/
int fs_create(const char *path)
{
    int retval = -1;

    if (!check_filename_length(path))
    {
        report_error("create: filename too long");
    }

    if (!strcmp(path, ".."))
    {
        report_error("create: filename cannot be `..`");
    }

    if (cur_dir->item_num >= sb->fcb_num_per_block)
    {
        report_error("create: current FCB is full, no more item is allowed");
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
        report_error("create: file already existed");
    }

    fcb_t *fcb = &cur_dir->fcb[cur_dir->item_num];
    strncpy(fcb->fname, path, FNAME_LENGTH);
    fcb->size = 0;
    fcb->bid = 0;
    fcb->attrs = EXIST_MASK;

    cur_dir->item_num++;

    // save current dir block to image file
    pwrite(fd, cur_dir, sizeof(blk_t), offset_of(cur_dir->bid));

    retval = 0;

out:
    return retval;
}

/*
    open filename
*/
int fs_open(const char *path)
{
    int retval = -1; /* opened fd */

    if (!check_filename_length(path))
    {
        report_error("open: filename too long");
    }

    bool found = false;
    for (int i = 0; i < cur_dir->item_num; ++i)
    {
        if (fcb_exist(&cur_dir->fcb[i]))
        {
            if (!strncmp(cur_dir->fcb[i].fname, path, FNAME_LENGTH))
            {
                found = true;
                if (fcb_isdir(&cur_dir->fcb[i]))
                {
                    report_error("open: cannot open a directory");
                }

                for (int index = 0; index < MAX_FD; ++index)
                {
                    if (ofs[index].not_empty)
                    {
                        if (!strncmp(ofs[index].fcb.fname, path, FNAME_LENGTH))
                        {
                            // retval = index;
                            // goto out;
                            report_error("open: file already opened");
                        }
                    }
                }

                int available_fd = find_available_fd();
                if (available_fd < 0)
                {
                    report_error("open: too many opened files");
                }
                memcpy(&ofs[available_fd].fcb, &cur_dir->fcb[i], sizeof(fcb_t));
                ofs[available_fd].not_empty = true;
                ofs[available_fd].at_bid = cur_dir->bid;
                ofs[available_fd].fcb_id = i;
                ofs[available_fd].off = 0;
                ofs[available_fd].is_fcb_modified = false;
                // printf("open: fd: %d\n", available_fd);
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
        report_error("open: file not found");
    }

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
        report_error("close: illegal fd");
    }

    // fd empty
    if (!ofs[target_fd].not_empty)
    {
        report_error("close: illegal fd");
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
int fs_write(int target_fd, const char *buf, size_t size)
{
    int retval = -1;

    if (!check_opened_fd(target_fd))
    {
        report_error("write: illegal fd");
    }

    if (!ofs[target_fd].not_empty)
    {
        report_error("write: illegal fd");
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
            report_error("write: no free space");
        }
        // ofs[target_fd].fcb.bid = new_bid;
        fat[new_bid] = BLK_END;
        bid = new_bid;
        ofs[target_fd].fcb.bid = new_bid;
        ofs[target_fd].is_fcb_modified = true;
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
                report_error("write: no more free space");
            }
            if (bid)
            {
                fat[bid] = new_bid;
            }
            fat[new_bid] = BLK_END;
            bid = new_bid;
            off = 0;
            break;
        }
        bid = fat[bid];
        off -= BLOCK_SIZE;
    }

    //  ofs[target_fd].fcb.size = 0;

    int written = 0;
    int count = size;
    char *pbuf = (char *)buf;

    while (1)
    {
        if (count <= (BLOCK_SIZE - off)) /* direct append */
        {
            int actually_wrote = pwrite(fd, pbuf, count, offset_of(bid) + off);
            ofs[target_fd].off += actually_wrote;
            ofs[target_fd].fcb.size += actually_wrote;
            ofs[target_fd].is_fcb_modified = true;
            written += actually_wrote;
            break;
        }
        else /* need new blocks to append */
        {
            int n = BLOCK_SIZE - off;
            int actually_wrote = pwrite(fd, pbuf, n, offset_of(bid) + off);
            ofs[target_fd].off += actually_wrote;
            ofs[target_fd].fcb.size += actually_wrote;
            written += actually_wrote;
            bid_t new_bid = find_free_block();
            if (!new_bid)
            {
                report_error("write: no free space");
            }
            if (bid)
            {
                fat[bid] = new_bid;
            }
            fat[new_bid] = BLK_END;
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
int fs_read(int target_fd, const char *buf, size_t size)
{
    int retval = -1;

    if (!check_opened_fd(target_fd))
    {
        report_error("read: illegal fd");
    }

    if (!ofs[target_fd].not_empty)
    {
        report_error("read: illegal fd");
    }

    if (!size || !ofs[target_fd].fcb.size)
    {
        // file empty
        return 0;
    }

    // if (size > ofs[target_fd].fcb.size)
    //     size = ofs[target_fd].fcb.size;

    int count = min(size, ofs[target_fd].fcb.size - ofs[target_fd].off);

    bid_t bid = ofs[target_fd].fcb.bid;
    off_t off = ofs[target_fd].off;
    while (off >= BLOCK_SIZE)
    {
        bid = fat[bid];
        off -= BLOCK_SIZE;
    }

    int n = 0;
    int written = 0;
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

    if (!check_filename_length(path))
    {
        report_error("rm: filename too long");
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
                    report_error("rm: is a directory");
                }

                fcb_t *fcb = &cur_dir->fcb[i];
                bid_t bid, next_bid = fcb->bid;
                while (next_bid > 1)
                {
                    bid = next_bid;
                    next_bid = fat[bid];
                    fat[bid] = BLK_FREE; /* set block to free */
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
        report_error("rm: No such file or directory");
    }

    pwrite(fd, cur_dir, sizeof(blk_t), offset_of(cur_dir->bid));

    retval = 0;

out:
    return retval;
}
