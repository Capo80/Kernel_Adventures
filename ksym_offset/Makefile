KERNEL_MODULE_NAME := ksym_offset
KERNEL_MODULE_OBJECT_FILE_LIST := ksym_offset.o  mod.o libs/probe_helpers.o libs/smart_api_finder.o libs/smart_api.o

obj-m := $(KERNEL_MODULE_NAME).o
$(KERNEL_MODULE_NAME)-y += $(KERNEL_MODULE_OBJECT_FILE_LIST)


all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$$PWD 

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$$PWD clean