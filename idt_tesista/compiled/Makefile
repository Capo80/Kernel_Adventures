obj-m := idt_entry_discovery.o
trap_discovery_spurious-objs :=  idt_entry_discovery.o  mod.o

KDIR ?= /lib/modules/$(shell uname -r)/build


all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
