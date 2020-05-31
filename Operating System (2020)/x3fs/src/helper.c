#include <stdio.h>
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

char *format_size(uint32_t size)
{
    static char ssize[0xf] = {0};

    memset(ssize, 0, 0xf);

    if (size > 1024 * 1024 * 1024)
    {
        sprintf(ssize, "%.1fG", (float)size / (1024 * 1024 * 1024));
    }
    else if (size > 1024 * 1024)
    {
        sprintf(ssize, "%.1fM", (float)size / (1024 * 1024));
    }
    else if (size > 1024)
    {
        sprintf(ssize, "%.1fK", (float)size / 1024);
    }
    else
    {
        sprintf(ssize, "%dB", size);
    }

    return ssize;
}

char *get_dirname(dir_t *dir)
{
    static char name[FNAME_LENGTH + 1] = {0};

    // memset(name, 0, FNAME_LENGTH);

    if (dir->bid == root_bid)
    {
        // root dir
        strlcpy(name, "/", FNAME_LENGTH + 1);
    }
    else
    {
        dir_t *parent_dir = (dir_t *)malloc(sizeof(blk_t));
        pread(fd, parent_dir, sizeof(blk_t), offset_of(dir->parent_bid));
        for (int i = 0; i < parent_dir->item_num; ++i)
        {
            if (parent_dir->fcb[i].bid == dir->bid)
            {
                strlcpy(name, parent_dir->fcb[i].fname, FNAME_LENGTH + 1);
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
    static char path[PATH_LENGTH] = {0};

    if (dir->bid == root_bid)
    {
        // root dir
        // memset(path, 0, PATH_LENGTH);
        strlcpy(path, "/", FNAME_LENGTH + 1);
    }
    else
    {
        dir_t *parent_dir = (dir_t *)malloc(sizeof(blk_t));
        pread(fd, parent_dir, sizeof(blk_t), offset_of(dir->parent_bid));

        // recursive get
        get_abspath(parent_dir);

        if (path[strlen(path) - 1] != '/')
            strlcat(path, "/", PATH_LENGTH);
        strlcat(path, get_dirname(dir), PATH_LENGTH);

        if (parent_dir)
            free(parent_dir);
    }

    return path;
}

bool check_filename(const char *filename)
{
    if (!check_filename_length(filename))
    {
        return false;
    }

    for (int i = 0; i < strlen(filename); ++i)
    {
        switch (filename[i])
        {
        case '/':
        case '\\':
        case '?':
        case '*':
        case ':':
        case '>':
        case '<':
        case '"':
        case '|':
            return false;
        default:
            continue;
        }
    }

    return true;
}

int parse_path(const char *path, dir_t *dir)
{
    int retval = -1;

    if (!strnlen(path, PATH_LENGTH))
    {
        memcpy(dir, cur_dir, sizeof(blk_t));
        return 0;
    }

    dir_t *tmp = malloc(sizeof(blk_t));
    if (!tmp)
    {
        report_error("malloc error");
    }

    if (path[0] != '/') /* not abspath*/
    {
        pread(fd, tmp, sizeof(blk_t), offset_of(cur_dir->bid));
    }
    else
    {
        pread(fd, tmp, sizeof(blk_t), offset_of(root_bid));
    }

    char buffer[PATH_LENGTH] = {0};
    strlcpy(buffer, path, PATH_LENGTH);

    char *sub, *rest = buffer;

    while ((sub = strtok_r(rest, "/", &rest)))
    {
        if (!strlen(sub))
            continue;

        if (!check_filename(sub))
        {
            report_error("Filename invalid");
        }

        bool found = false;

        if (!strncmp(sub, ".", FNAME_LENGTH)) /* self dir */
        {
            found = true;
            // pread(fd, tmp, sizeof(blk_t), offset_of(tmp->bid));
        }
        else if (!strncmp(sub, "..", FNAME_LENGTH)) /* parent dir */
        {
            found = true;
            pread(fd, tmp, sizeof(blk_t), offset_of(tmp->parent_bid));
        }
        // else if (!strncmp(sub, "/", FNAME_LENGTH)) /* root dir */
        // {
        //     found = true;
        //     pread(fd, tmp, sizeof(blk_t), offset_of(root_bid));
        // }
        else /* sub dir */
        {
            for (int i = 0; i < tmp->item_num; ++i)
            {
                if (fcb_exist(&tmp->fcb[i]))
                {
                    if (!strncmp(tmp->fcb[i].fname, sub, FNAME_LENGTH))
                    {
                        found = true;
                        if (fcb_isfile(&tmp->fcb[i]))
                        {
                            report_error("Not a directory");
                        }
                        pread(fd, tmp, sizeof(blk_t), offset_of(tmp->fcb[i].bid));
                        if (!dir_check_magic(tmp))
                        {
                            report_error("Magic number of directory didn't match");
                        }
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
            report_error("No such file or directory");
        }
    }

    memcpy(dir, tmp, sizeof(blk_t));

    retval = 0;

out:
    if (tmp)
        free(tmp);
    return retval;
}

int split_path(const char *path, char *head, char *tail)
{
    int i = strlen(path) - 1;
    while (path[i] != '/')
    {
        if (--i < 0)
            break;
    }

    strlcpy(head, path, i + 1);
    strlcpy(tail, path + i + 1, strlen(path) - i);

    return 0;
}
