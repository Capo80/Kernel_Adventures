KERNEL_MODULE_NAME := trap_installer
KERNEL_MODULE_OBJECT_FILE_LIST := trap_installer.o control.o  mod.o

obj-m := $(KERNEL_MODULE_NAME).o
$(KERNEL_MODULE_NAME)-y += $(KERNEL_MODULE_OBJECT_FILE_LIST)


all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$$PWD

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$$PWD clean