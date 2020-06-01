#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "fs.h"

#ifdef __linux__

/*
 * '_cups_strlcat()' - Safely concatenate two strings.
 */

size_t                   /* O - Length of string */
strlcat(char *dst,       /* O - Destination string */
        const char *src, /* I - Source string */
        size_t size)     /* I - Size of destination string buffer */
{
    size_t srclen; /* Length of source string */
    size_t dstlen; /* Length of destination string */

    /*
  * Figure out how much room is left...
  */

    dstlen = strlen(dst);
    size -= dstlen + 1;

    if (!size)
        return (dstlen); /* No room, return immediately... */

    /*
  * Figure out how much room is needed...
  */

    srclen = strlen(src);

    /*
  * Copy the appropriate amount...
  */

    if (srclen > size)
        srclen = size;

    memcpy(dst + dstlen, src, srclen);
    dst[dstlen + srclen] = '\0';

    return (dstlen + srclen);
}

/*
 * '_cups_strlcpy()' - Safely copy two strings.
 */

size_t                   /* O - Length of string */
strlcpy(char *dst,       /* O - Destination string */
        const char *src, /* I - Source string */
        size_t size)     /* I - Size of destination string buffer */
{
    size_t srclen; /* Length of source string */

    /*
  * Figure out how much room is needed...
  */

    size--;

    srclen = strlen(src);

    /*
  * Copy the appropriate amount...
  */

    if (srclen > size)
        srclen = size;

    memcpy(dst, src, srclen);
    dst[srclen] = '\0';

    return (srclen);
}

#endif

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

int find_dir_fcb(dir_t *dir, fcb_t *fcb)
{
    int retval = -1;

    dir_t *parent_dir = (dir_t *)calloc(1, sizeof(blk_t));
    if (!parent_dir)
    {
        report_error("calloc error");
    }
    pread(fd, parent_dir, sizeof(blk_t), offset_of(dir->parent_bid));

    bool found = false;
    for (int i = 0; i < parent_dir->item_num; ++i)
    {
        if (parent_dir->fcb[i].bid == dir->bid)
        {
            found = true;
            memcpy(fcb, &parent_dir->fcb[i], sizeof(fcb_t));
            // for root fcb
            if (fcb->bid == root_bid)
            {
                strcpy(fcb->fname, "/");
            }
            break;
        }
    }

    free(parent_dir);

    if (!found)
    {
        report_error("Not found");
    }

    retval = 0;

out:
    return retval;
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
        if (!parent_dir)
        {
            memset(path, 0, PATH_LENGTH);
            return path;
        }
        pread(fd, parent_dir, sizeof(blk_t), offset_of(dir->parent_bid));

        // recursive get
        get_abspath(parent_dir);

        if (path[strlen(path) - 1] != '/')
            strlcat(path, "/", PATH_LENGTH);

        fcb_t fcb;
        if (find_dir_fcb(dir, &fcb) >= 0) /* ignore error */
        {
            strlcat(path, fcb.fname, PATH_LENGTH);
        }

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

    if (!check_path_length(path))
    {
        report_error("Pathname too long");
    }

    if (!strnlen(path, PATH_LENGTH))
    {
        memcpy(dir, cur_dir, sizeof(blk_t));
        return 0;
    }

    dir_t *tmp = calloc(1, sizeof(blk_t));
    if (!tmp)
    {
        report_error("calloc error");
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
            free(tmp);
            report_error("Filename invalid");
        }

        bool found = false;

        for (int i = 0; i < tmp->item_num; ++i)
        {
            if (fcb_exist(&tmp->fcb[i]))
            {
                if (!strncmp(tmp->fcb[i].fname, sub, FNAME_LENGTH))
                {
                    found = true;
                    if (fcb_isfile(&tmp->fcb[i]))
                    {
                        free(tmp);
                        report_error("Not a directory");
                    }

                    if (fcb_symlink(&tmp->fcb[i]))
                    {
                        pread(fd, tmp, sizeof(blk_t), offset_of(tmp->fcb[i].src_bid));
                    }
                    else
                    {
                        pread(fd, tmp, sizeof(blk_t), offset_of(tmp->fcb[i].bid));
                    }

                    if (!dir_check_magic(tmp))
                    {
                        free(tmp);
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

        if (!found)
        {
            free(tmp);
            report_error("No such file or directory");
        }
    }

    memcpy(dir, tmp, sizeof(blk_t));
    free(tmp);

    retval = 0;

out:
    return retval;
}

void split_path(const char *path, char **p, char **f)
{
    static char head[PATH_LENGTH];
    static char tail[FNAME_LENGTH + 1];

    memset(head, 0, PATH_LENGTH);
    memset(tail, 0, FNAME_LENGTH + 1);

    int i = strlen(path);
    if (!i)
        goto out;

    while (i >= 0 && path[i] != '/')
        i--;

    strlcpy(head, path, min(i + 2, PATH_LENGTH));
    strlcpy(tail, path + i + 1, FNAME_LENGTH + 1);

out:
    *p = head;
    *f = tail;
}

char *read_symlink(fcb_t *fcb)
{
    static char path[PATH_LENGTH] = {0};

    if (!fcb_symlink(fcb))
        return NULL;

    memset(path, 0, PATH_LENGTH);
    pread(fd, path, fcb->size, offset_of(fcb->bid));

    return path;
}
