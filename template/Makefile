KERNEL_MODULE_NAME := hook
KERNEL_MODULE_OBJECT_FILE_LIST := mod.o libs/kprobes_helper.o

# DOS_PROTECTION
# from command line use: make EXTRA_CFLAGS=-DKERNEL_SYNC_CHECK=42
# EXTRA_CFLAGS:= -D KERNEL_SYNC_CHECK=1

obj-m := $(KERNEL_MODULE_NAME).o
$(KERNEL_MODULE_NAME)-y += $(KERNEL_MODULE_OBJECT_FILE_LIST)

all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$$PWD 

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$$PWD clean