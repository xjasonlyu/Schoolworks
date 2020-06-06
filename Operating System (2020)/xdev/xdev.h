#ifndef _XDEV_H
#define _XDEV_H

#include <linux/ioctl.h>
#include <linux/types.h>

#define MAJOR_NUM 100

#define DEVICE_FILE_NAME "/dev/xdev"

#define IOCTL_FULL _IO(MAJOR_NUM, 0)
#define IOCTL_NULL _IO(MAJOR_NUM, 1)
#define IOCTL_ZERO _IO(MAJOR_NUM, 2)
#define IOCTL_INSPECT _IO(MAJOR_NUM, 3)

#endif
