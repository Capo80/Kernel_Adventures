KERNEL_MODULE_NAME := user_exe
KERNEL_MODULE_OBJECT_FILE_LIST := main.o libs/syscall_patcher.o libs/memory_helpers.o  libs/page_table_libs/page_table_utils.o libs/symbol_lookup.o libs/kernel_patcher.o libs/allocator.o

obj-m := $(KERNEL_MODULE_NAME).o
$(KERNEL_MODULE_NAME)-y += $(KERNEL_MODULE_OBJECT_FILE_LIST)


all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$$PWD

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$$PWD clean