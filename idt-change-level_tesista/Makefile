obj-m := my_module3.o
trap_discovery_spurious-objs :=  my_module3.o  mod.o

KDIR ?= /lib/modules/$(shell uname -r)/build


all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
