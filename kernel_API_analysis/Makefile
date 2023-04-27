KERNEL_MODULE_NAME := ksym_an
KERNEL_MODULE_OBJECT_FILE_LIST := ksym_an.o  mod.o

obj-m := $(KERNEL_MODULE_NAME).o
$(KERNEL_MODULE_NAME)-y += $(KERNEL_MODULE_OBJECT_FILE_LIST)

all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$$PWD 

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$$PWD clean