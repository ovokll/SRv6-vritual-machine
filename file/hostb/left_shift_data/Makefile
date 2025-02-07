obj-m += left_shift_data.o
left_shift_data-objs :=leftshiftdata.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shelll uname -r)/build M=$(PWD) clean
