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

    char *p, *f;
    split_path(path, &p, &f);

    if (!check_filename(f))
    {
        report_error("%s: Invalid filename", f);
    }

    if (parse_path(p, tmp_dir) < 0)
    {
        report_error("%s: Parse path error", p);
    }

    for (int i = 0; i < sb->fcb_num_per_block; ++i)
    {
        if (fcb_exist(&tmp_dir->fcb[i]))
        {
            if (!strncmp(tmp_dir->fcb[i].fname, f, FNAME_LENGTH))
            {
                report_error("File exists");
            }
        }
    }

    if (tmp_dir->item_num >= sb->fcb_num_per_block)
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

    fcb_t *fcb = &(tmp_dir->fcb)[tmp_dir->item_num];
    strlcpy(fcb->fname, f, FNAME_LENGTH + 1);
    fcb->size = 0;
    fcb->attrs = EXIST_MASK | DIR_MASK;
    fcb->bid = bid;
    fcb->src_bid = bid;
    fcb->created_time = time(NULL);
    fcb->modified_time = fcb->created_time;
    tmp_dir->item_num++;
    // update current dir block to image file
    pwrite(fd, tmp_dir, sizeof(blk_t), offset_of(tmp_dir->bid));

    dir_t *new_dir = (dir_t *)calloc(1, sizeof(blk_t));
    new_dir->magic = MAGIC_DIR;
    new_dir->item_num = 0;
    new_dir->bid = bid;
    new_dir->parent_bid = tmp_dir->bid;
    // .
    strcpy(new_dir->fcb[0].fname, ".");
    new_dir->fcb[0].size = fcb->size;
    new_dir->fcb[0].bid = fcb->bid;
    new_dir->fcb[0].src_bid = fcb->src_bid;
    new_dir->fcb[0].attrs = fcb->attrs;
    new_dir->fcb[0].created_time = fcb->created_time;
    new_dir->fcb[0].modified_time = fcb->modified_time;
    new_dir->item_num++;
    // ..
    fcb_t tmp_dir_fcb;
    if (find_dir_fcb(tmp_dir, &tmp_dir_fcb) < 0)
    {
        free(new_dir);
        goto out;
    }
    strcpy(new_dir->fcb[1].fname, "..");
    new_dir->fcb[1].size = tmp_dir_fcb.size;
    new_dir->fcb[1].bid = tmp_dir_fcb.bid;
    new_dir->fcb[1].src_bid = tmp_dir_fcb.src_bid;
    new_dir->fcb[1].attrs = tmp_dir_fcb.attrs;
    new_dir->fcb[1].created_time = tmp_dir_fcb.created_time;
    new_dir->fcb[1].modified_time = tmp_dir_fcb.modified_time;
    new_dir->item_num++;
    // write new dir block to image file
    pwrite(fd, new_dir, sizeof(blk_t), offset_of(bid));
    free(new_dir);

    update_cur_dir(tmp_dir);

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

    char *p, *f;
    split_path(path, &p, &f);

    if (!check_filename(f))
    {
        report_error("%s: Invalid filename", f);
    }

    if (!strcmp(f, ".") || !strcmp(f, ".."))
    {
        report_error("Invalid argument");
    }

    if (parse_path(p, tmp_dir) < 0)
    {
        report_error("%s: Parse path error", p);
    }

    bool found = false;
    for (int i = 0; i < sb->fcb_num_per_block; ++i)
    {
        if (fcb_exist(&tmp_dir->fcb[i]))
        {
            if (!strncmp(tmp_dir->fcb[i].fname, f, FNAME_LENGTH))
            {
                found = true;

                if (fcb_isfile(&tmp_dir->fcb[i]))
                {
                    /* is file */
                    report_error("Not a directory");
                }

                // check if sub dir empty
                int item_num = 0;
                dir_t *sub_dir = calloc(1, sizeof(blk_t));
                pread(fd, sub_dir, sizeof(blk_t), offset_of(tmp_dir->fcb[i].bid));
                item_num = sub_dir->item_num;
                free(sub_dir);

                if (item_num > 2) /* ignore . & .. */
                {
                    report_error("Directory not empty");
                }

                bid_t bid, next_bid = tmp_dir->fcb[i].bid;
                while (next_bid > 1)
                {
                    bid = next_bid;
                    next_bid = fat[bid];
                    fat[bid] = BLK_FREE;
                    sb->free_block_num++;
                }
                memset(&tmp_dir->fcb[i], 0, sizeof(fcb_t));

                for (int j = i + 1; j < sb->fcb_num_per_block; ++j)
                {
                    if (fcb_exist(&tmp_dir->fcb[j]))
                    {
                        memcpy(&tmp_dir->fcb[j - 1], &tmp_dir->fcb[j], sizeof(fcb_t));
                        memset(&tmp_dir->fcb[j], 0, sizeof(fcb_t));
                    }
                }

                tmp_dir->item_num--;
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

    pwrite(fd, tmp_dir, sizeof(blk_t), offset_of(tmp_dir->bid));
    update_cur_dir(tmp_dir);

    retval = 0;

out:
    return retval;
}

/*
    ls
*/
int fs_ls(const char *path)
{
    int retval = -1;

    if (parse_path(path ? path : "", tmp_dir) < 0)
        goto out;

    char *lnk;
    char buffer[0x10];

    printf("total %d\n", tmp_dir->item_num);
    for (int i = 0; i < tmp_dir->item_num; ++i)
    {
        if (fcb_exist(&tmp_dir->fcb[i]))
        {
            strftime(buffer, 0x0F, "%b %d %H:%M", localtime(&tmp_dir->fcb[i].modified_time));
            if ((lnk = read_symlink(&tmp_dir->fcb[i])))
            {
                printf("%sl %6d %6d %7s %13s %s -> %s\n",
                       (fcb_isdir(&tmp_dir->fcb[i]) ? "d" : "f"),
                       tmp_dir->fcb[i].bid,
                       tmp_dir->fcb[i].src_bid,
                       format_size(tmp_dir->fcb[i].size),
                       buffer, tmp_dir->fcb[i].fname, lnk);
            }
            else
            {
                printf("%s- %6d %6d %7s %13s %-9s\n",
                       (fcb_isdir(&tmp_dir->fcb[i]) ? "d" : "f"),
                       tmp_dir->fcb[i].bid,
                       tmp_dir->fcb[i].src_bid,
                       format_size(tmp_dir->fcb[i].size),
                       buffer, tmp_dir->fcb[i].fname);
            }
        }
        else
        {
            break;
        }
    }

    retval = 0;

out:
    return retval;
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
    return parse_path(path ? path : "", cur_dir);
}

/*
    create filename
*/
int fs_create(const char *path)
{
    int retval = -1;

    char *p, *f;
    split_path(path, &p, &f);

    if (!check_filename(f))
    {
        report_error("%s: Invalid filename", f);
    }

    if (parse_path(p, tmp_dir) < 0)
    {
        report_error("%s: Parse path error", p);
    }

    if (tmp_dir->item_num >= sb->fcb_num_per_block)
    {
        report_error("Current FCB is full, no more item is allowed");
    }

    // check if file already existed
    for (int i = 0; i < sb->fcb_num_per_block; ++i)
    {
        if (fcb_exist(&tmp_dir->fcb[i]))
        {
            if (!strncmp(tmp_dir->fcb[i].fname, f, FNAME_LENGTH))
            {
                report_error("File exists");
            }
        }
    }

    fcb_t *fcb = &tmp_dir->fcb[tmp_dir->item_num];
    strlcpy(fcb->fname, f, FNAME_LENGTH + 1);
    fcb->size = 0;
    fcb->bid = 0;
    fcb->src_bid = 0;
    fcb->attrs = EXIST_MASK;
    fcb->created_time = time(NULL);
    fcb->modified_time = fcb->created_time;

    tmp_dir->item_num++;

    // save current dir block to image file
    pwrite(fd, tmp_dir, sizeof(blk_t), offset_of(tmp_dir->bid));
    update_cur_dir(tmp_dir);

    retval = 0;

out:
    return retval;
}

/*
    open filename oflag
*/
int fs_open(const char *path, int oflag)
{
    int retval = -1; /* opened fd */

    if (!check_oflag(oflag))
    {
        report_error("Open flag invalid");
    }

    char *p, *f;
    split_path(path, &p, &f);

    if (!check_filename(f))
    {
        report_error("%s: Invalid filename", f);
    }

    if (parse_path(p, tmp_dir) < 0)
    {
        report_error("%s: Parse path error", p);
    }

    bool found = false;
    bool is_symlink = false;
    int available_fd = -1;
find:
    for (int i = 0; i < tmp_dir->item_num; ++i)
    {
        if (fcb_exist(&tmp_dir->fcb[i]))
        {
            if (!strncmp(tmp_dir->fcb[i].fname, f, FNAME_LENGTH))
            {
                found = true;
                if (fcb_isdir(&tmp_dir->fcb[i]))
                {
                    report_error("Cannot open a directory");
                }

                if (fcb_symlink(&tmp_dir->fcb[i]))
                {
                    // refind
                    found = false;
                    is_symlink = true;
                    // reload source name
                    split_path(read_symlink(&tmp_dir->fcb[i]), &p, &f);
                    // re-parse
                    if (parse_path(p, tmp_dir) < 0)
                    {
                        report_error("%s: Parse path error", p);
                    }
                    else
                    {
                        goto find;
                    }
                }

                for (int index = 0; index < MAX_FD; ++index)
                {
                    if (ofs[index].not_empty)
                    {
                        if (!strncmp(ofs[index].fcb.fname, f, FNAME_LENGTH))
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
                memcpy(&ofs[available_fd].fcb, &tmp_dir->fcb[i], sizeof(fcb_t));
                ofs[available_fd].not_empty = true;
                ofs[available_fd].at_bid = tmp_dir->bid;
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
            if (!fs_create(f))
                goto find;
        }
        else if (is_symlink)
        {
            report_error("%s: Symlink file not found", f);
        }
        else
        {
            report_error("File not found");
        }
    }

    update_cur_dir(tmp_dir);

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

    char *p, *f;
    split_path(path, &p, &f);

    if (!check_filename(f))
    {
        report_error("%s: Invalid filename", f);
    }

    if (parse_path(p, tmp_dir) < 0)
    {
        report_error("%s: Parse path error", p);
    }

    bool found = false;
    for (int i = 0; i < sb->fcb_num_per_block; ++i)
    {
        if (fcb_exist(&tmp_dir->fcb[i]))
        {
            if (!strncmp(tmp_dir->fcb[i].fname, f, FNAME_LENGTH))
            {
                found = true;

                if (fcb_isdir(&tmp_dir->fcb[i]))
                {
                    report_error("Is a directory");
                }

                fcb_t *fcb = &tmp_dir->fcb[i];
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
                    if (fcb_exist(&tmp_dir->fcb[j]))
                    {
                        memcpy(&tmp_dir->fcb[j - 1], &tmp_dir->fcb[j], sizeof(fcb_t));
                        memset(&tmp_dir->fcb[j], 0, sizeof(fcb_t));
                    }
                }

                tmp_dir->item_num--;
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

    pwrite(fd, tmp_dir, sizeof(blk_t), offset_of(tmp_dir->bid));
    update_cur_dir(tmp_dir);

    retval = 0;

out:
    return retval;
}

/*
    rename path newname
*/
int fs_rename(const char *path, const char *newname)
{
    int retval = -1;

    char *p, *f;
    split_path(path, &p, &f);

    if (!check_filename(f))
    {
        report_error("%s: Invalid filename", f);
    }

    if (!check_filename(newname))
    {
        report_error("%s: Invalid filename", newname);
    }

    if (parse_path(p, tmp_dir) < 0)
    {
        report_error("%s: Parse path error", p);
    }

    int index = -1;
    bool found = false;
    for (int i = 0; i < sb->fcb_num_per_block; ++i)
    {
        if (fcb_exist(&tmp_dir->fcb[i]))
        {
            if (!strncmp(tmp_dir->fcb[i].fname, newname, FNAME_LENGTH))
            {
                report_error("File exists");
            }

            if (!strncmp(tmp_dir->fcb[i].fname, f, FNAME_LENGTH))
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
    strlcpy(tmp_dir->fcb[index].fname, newname, FNAME_LENGTH + 1);
    // update time
    tmp_dir->fcb[index].modified_time = time(NULL);

    pwrite(fd, tmp_dir, sizeof(blk_t), offset_of(tmp_dir->bid));
    update_cur_dir(tmp_dir);

    retval = 0;

out:
    return retval;
}

/*
    symlink src target
*/
int fs_symlink(const char *src, const char *target)
{
    int retval = -1;
    int lnk_fd = -1;

    char *p, *f;
    split_path(src, &p, &f);

    if (!check_filename(f))
    {
        report_error("%s: Invalid filename", f);
    }

    if (parse_path(p, tmp_dir) < 0)
    {
        report_error("%s: Parse path error", p);
    }

    // check source file validation
    fcb_t src_fcb;
    bool found = false;
    for (int i = 0; i < sb->fcb_num_per_block; ++i)
    {
        if (fcb_exist(&tmp_dir->fcb[i]))
        {
            if (!strncmp(tmp_dir->fcb[i].fname, f, FNAME_LENGTH))
            {
                found = true;
                memcpy(&src_fcb, &tmp_dir->fcb[i], sizeof(fcb_t));
                break;
            }
        }
    }

    if (!found)
    {
        report_error("Src file not found");
    }

    split_path(target, &p, &f);

    if (!check_filename(f))
    {
        report_error("%s: Invalid filename", f);
    }

    if (parse_path(p, tmp_dir) < 0)
    {
        report_error("%s: Parse path error", p);
    }

    if (tmp_dir->item_num >= sb->fcb_num_per_block)
    {
        report_error("Current FCB is full, no more item is allowed");
    }

    // check if file already existed
    for (int i = 0; i < sb->fcb_num_per_block; ++i)
    {
        if (fcb_exist(&tmp_dir->fcb[i]))
        {
            if (!strncmp(tmp_dir->fcb[i].fname, f, FNAME_LENGTH))
            {
                report_error("File exists");
            }
        }
    }

    bid_t bid;
    if ((bid = find_free_block()) == 0)
    {
        report_error("No free space");
    }

    fat[bid] = BLK_END;
    sb->free_block_num--;

    fcb_t *fcb = &(tmp_dir->fcb)[tmp_dir->item_num];
    strlcpy(fcb->fname, f, FNAME_LENGTH + 1);
    fcb->size = 0;
    fcb->attrs = src_fcb.attrs | SYMLINK_MASK;
    fcb->bid = bid;
    fcb->src_bid = src_fcb.bid; // set to source bid
    fcb->created_time = time(NULL);
    fcb->modified_time = fcb->created_time;
    tmp_dir->item_num++;

    // write link source to block
    fcb->size += pwrite(fd, src, strlen(src) + 1, offset_of(bid));

    // update current dir block to image file
    pwrite(fd, tmp_dir, sizeof(blk_t), offset_of(tmp_dir->bid));

    update_cur_dir(tmp_dir);

    retval = 0;

out:
    return retval;
}
