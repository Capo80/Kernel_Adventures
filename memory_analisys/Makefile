KERNEL_MODULE_NAME := hook
KERNEL_MODULE_OBJECT_FILE_LIST := hook.o mod.o page_table_libs/page_table_utils.o

obj-m := $(KERNEL_MODULE_NAME).o
$(KERNEL_MODULE_NAME)-y += $(KERNEL_MODULE_OBJECT_FILE_LIST)

all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$$PWD 

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$$PWD clean