obj-m += kmatr.o 
kmatr-objs := main.o libs/load_helpers.o libs/symbol_lookup.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean