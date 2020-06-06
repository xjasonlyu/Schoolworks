
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include "xdev.h"

#define SUCCESS 0
#define DEVICE_NAME "xdev"

static int dev_open = 0;
// default mode
static int cur_mode = IOCTL_FULL;

int init_module(void);
void cleanup_module(void);

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static long device_ioctl(struct file *, unsigned int, unsigned long);

static int device_open(struct inode *inode, struct file *file)
{
#ifdef DEBUG
    printk(KERN_INFO "device_open(%p)\n", file);
#endif

    if (dev_open)
        return -EBUSY;

    dev_open++;

    try_module_get(THIS_MODULE);
    return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
#ifdef DEBUG
    printk(KERN_INFO "device_release(%p,%p)\n", inode, file);
#endif

    dev_open--;

    module_put(THIS_MODULE);
    return SUCCESS;
}

static ssize_t device_read(struct file *file,
                           char __user *buffer,
                           size_t length,
                           loff_t *offset)
{
    int bytes_read = 0;

#ifdef DEBUG
    printk(KERN_INFO "device_read(%p,%p,%d)\n", file, buffer, length);
#endif

    switch (cur_mode)
    {
    case IOCTL_FULL:
        /* simply break */
        break;
    case IOCTL_NULL:
        /* simply break */
        break;
    case IOCTL_ZERO:
        while (length--)
        {
            /* write zeros to user buffer */
            put_user(0, buffer++);
            bytes_read++;
        }
        break;
    default:
        return -EINVAL;
    }

    /* 
	 * Read functions are supposed to return the number
	 * of bytes actually inserted into the buffer 
	 */
    return bytes_read;
}

static ssize_t
device_write(struct file *file,
             const char __user *buffer, size_t length, loff_t *offset)
{
    int written = 0;

#ifdef DEBUG
    printk(KERN_INFO "device_write(%p,%s,%d)", file, buffer, length);
#endif

    switch (cur_mode)
    {
    case IOCTL_FULL:
        /* no space */
        return -ENOSPC;
    case IOCTL_NULL:
        written += length;
        break;
    case IOCTL_ZERO: /* fallthrough */
    default:
        return -EINVAL;
    }

    return written;
}

long device_ioctl(struct file *file,
                  unsigned int ioctl_num,
                  unsigned long ioctl_param)
{
    /* 
	 * Switch according to the ioctl called 
	 */
    switch (ioctl_num)
    {
    case IOCTL_FULL:
    case IOCTL_NULL:
    case IOCTL_ZERO:
        cur_mode = ioctl_num;
        break;
    case IOCTL_INSPECT:
        /* return current mode */
        return cur_mode;
    default:
        return -EINVAL;
    }

    return SUCCESS;
}

struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = device_read,
    .write = device_write,
    .unlocked_ioctl = device_ioctl,
    .open = device_open,
    .release = device_release, /* a.k.a. close */
};

/* 
 * Initialize the module - Register the character device 
 */
int init_module()
{
    int ret_val;
    /* 
	 * Register the character device (atleast try) 
	 */
    ret_val = register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops);

    /* 
	 * Negative values signify an error 
	 */
    if (ret_val < 0)
    {
        printk(KERN_ALERT "register device failed with %d\n", ret_val);
        return ret_val;
    }

    printk(KERN_INFO "The major device number is %d.\n", MAJOR_NUM);

    return SUCCESS;
}

/* 
 * Cleanup - unregister the appropriate file from /proc 
 */
void cleanup_module()
{
    /* 
	 * Unregister the device 
	 */
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jason Lyu"); 
