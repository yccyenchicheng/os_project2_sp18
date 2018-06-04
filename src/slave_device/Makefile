#
# Makefile for kernel test
#
PWD         := $(shell pwd) 
KVERSION    := $(shell uname -r)
#KERNEL_DIR   = /usr/src/linux-headers-$(KVERSION)/
#KERNEL_DIR   = /home/charles/linux-$(KVERSION)/
KDIR        := /lib/modules/$(shell uname -r)/build
KMOD        := /lib/modules/$(shell uname -r)/kernel
MODULE_NAME  = slave_device
#obj-m       := $(MODULE_NAME).o   
obj-m       += $(MODULE_NAME).o   

all:
	cp ../ksocket/Module.symvers ./Module.symvers
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
	#make -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
	#make -C $(KERNEL_DIR) M=$(PWD) clean
