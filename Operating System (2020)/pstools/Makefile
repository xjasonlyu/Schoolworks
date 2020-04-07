ifneq ($(KERNELRELEASE),)

	obj-m := psinfo.o
	obj-m += pstree.o

else

	KERN_DIR := /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

default:
	make -C $(KERN_DIR) M=$(PWD) modules

test_psinfo:
	dmesg -C
	insmod psinfo.ko
	rmmod psinfo.ko
	dmesg

test_pstree:
	dmesg -C
ifneq ($(args),)
	insmod pstree.ko $(args)
else
	insmod pstree.ko
endif
	rmmod pstree.ko
	dmesg

clean:
	make -C $(KERN_DIR) M=$(PWD) clean

endif
