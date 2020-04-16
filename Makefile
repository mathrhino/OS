obj-m += mousehole.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
LD_LIBRARY_PATH=/home/youngchae/OS gcc -0 mousehole mousehole.c 
