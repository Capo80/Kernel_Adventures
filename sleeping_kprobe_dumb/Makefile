KERNEL_MODULE_NAME := sleep
KERNEL_MODULE_OBJECT_FILE_LIST := sleep.o mod.o libs/kprobes_helper.o libs/sleeping_kprobes.o

obj-m := $(KERNEL_MODULE_NAME).o
$(KERNEL_MODULE_NAME)-y += $(KERNEL_MODULE_OBJECT_FILE_LIST)

all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$$PWD 

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$$PWD clean