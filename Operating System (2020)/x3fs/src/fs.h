#ifndef _FS_H
#define _FS_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>

extern int fd;

#define report_error(msg, val...)                              \
    do                                                         \
    {                                                          \
        fprintf(stderr, "%s: " msg "\n", __FUNCTION__, ##val); \
        retval = -1;                                           \
        goto out;                                              \
    } while (0)

#define min(a, b) ((a) < (b) ? (a) : (b))

#define BLOCK_SIZE 4096
#define BLK_FREE 0
#define BLK_END 1

typedef uint16_t bid_t; // block id
typedef uint8_t blk_t[BLOCK_SIZE];

#define offset_of(x) (sizeof(blk_t) * x)

typedef struct superblock
{
    uint16_t magic;
#define MAGIC_SUPERBLOCK 0x1510u
    int total_size;
    int total_block_num;
    int free_block_num;
    int fat_block_num;
    int fcb_num_per_block;
    int data_start_bid;
    int fat_crc;
} sb_t;
#define sb_check_magic(x) (((sb_t *)x)->magic == MAGIC_SUPERBLOCK)

#define PATH_LENGTH 0xFF
#define FNAME_LENGTH 0x10
typedef struct fcb
{
    char fname[FNAME_LENGTH + 1]; // 17
    uint32_t size;                // 4
    bid_t bid;                    // 2
    bid_t src_bid;                // 2
    uint8_t attrs;                // 1
    time_t created_time;          // 4
    time_t modified_time;         // 4
} fcb_t;
#define check_path_length(x) (strlen(x) <= PATH_LENGTH)
#define check_filename_length(x) (0 < strlen(x) && strlen(x) <= FNAME_LENGTH)
#define SYMLINK_MASK 0b100u
#define DIR_MASK 0b10u
#define EXIST_MASK 0b1u
#define fcb_symlink(fcb) ((fcb)->attrs & SYMLINK_MASK)
#define fcb_isdir(fcb) ((fcb)->attrs & DIR_MASK)
#define fcb_isfile(fcb) (!((fcb)->attrs & DIR_MASK))
#define fcb_exist(fcb) ((fcb)->attrs & EXIST_MASK)

typedef struct opened_file
{
    bool not_empty;
    fcb_t fcb;
    bid_t at_bid;
    int fcb_id;
    off_t off;     // offset
    uint8_t oflag; // support: read write create
    bool is_fcb_modified;
} of_t;
#define MAX_FD 0x0F
#define check_opened_fd(x) ((x) >= 0 && (x) <= MAX_FD)
#define RD_MASK 0b1u
#define WR_MASK 0b10u
#define RW_MASK (RD_MASK | WR_MASK)
#define CR_MASK 0b100u
#define check_read(x) ((x)&RD_MASK)
#define check_write(x) ((x)&WR_MASK)
#define check_create(x) ((x)&CR_MASK)
#define check_oflag(x) (RD_MASK <= (x) && (x) <= (RW_MASK | CR_MASK))
extern of_t ofs[MAX_FD];

typedef struct directory
{
    uint16_t magic;
#define MAGIC_DIR 0xD151u
    int item_num;
    bid_t bid;
    bid_t parent_bid;
    fcb_t fcb[0];
} dir_t;
#define dir_check_magic(x) (((dir_t *)x)->magic == MAGIC_DIR)

extern sb_t *sb;
#define root_bid (1 + 2 * sb->fat_block_num)
extern bid_t *fat;
extern dir_t *cur_dir;
extern dir_t *tmp_dir;
/* update if it's current dir */
#define update_cur_dir(x)                        \
    do                                           \
    {                                            \
        if ((x)->bid == cur_dir->bid)            \
            memcpy(cur_dir, (x), sizeof(blk_t)); \
    } while (0)

// init fs image
int fs_loadfrom(const char *filename);
int fs_writeto(const char *filename);

// operations
int fs_mkdir(const char *path);
int fs_rmdir(const char *path);
int fs_ls(const char *path);
int fs_lsof();
int fs_stat();
int fs_cd(const char *path);
int fs_create(const char *path);
int fs_open(const char *path, int);
int fs_close(int);
#ifndef SEEK_SET
#define SEEK_SET 0 /* set file offset to offset */
#endif
#ifndef SEEK_CUR
#define SEEK_CUR 1 /* set file offset to current plus offset */
#endif
#ifndef SEEK_END
#define SEEK_END 2 /* set file offset to EOF plus offset */
#endif
off_t fs_seek(int, off_t, int);
ssize_t fs_write(int, const char *, size_t);
ssize_t fs_read(int, const char *, size_t);
int fs_rm(const char *path);
int fs_rename(const char *, const char *);
int fs_symlink(const char *, const char *);
int fs_exit();

// helper
bid_t find_free_block();
int find_available_fd();
int find_dir_fcb(dir_t *, fcb_t *);
char *format_size(uint32_t);
char *get_abspath(dir_t *);
char *read_symlink(fcb_t *);
bool check_filename(const char *);
int parse_path(const char *, dir_t *);
void split_path(const char *, char **, char **);

#endif
