KERNEL_MODULE_NAME := trap_syscall
KERNEL_MODULE_OBJECT_FILE_LIST := trap_syscall.o  mod.o libs/trap_controller.o libs/syscall_dispatcher.o

obj-m := $(KERNEL_MODULE_NAME).o
$(KERNEL_MODULE_NAME)-y += $(KERNEL_MODULE_OBJECT_FILE_LIST)


all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$$PWD

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$$PWD clean