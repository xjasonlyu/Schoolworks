obj-m += xdev.o

major := 100
name := xdev

PWD := $(shell pwd)
KERNEL := /lib/modules/$(shell uname -r)/build

all: xdev xdevctl

xdev:
	make -C $(KERNEL) M=$(PWD) modules

xdevctl:
	gcc -o xdevctl xdevctl.c

install:
	dmesg -C && insmod xdev.ko && dmesg
	mknod /dev/$(name) c $(major) 0
	cat /proc/devices | grep $(name)

remove:
	rm /dev/$(name)
	dmesg -C && rmmod xdev.ko && dmesg

test:
	sh test.sh

clean:
	make -C $(KERNEL) M=$(PWD) clean
	rm -rf ./xdevctl
