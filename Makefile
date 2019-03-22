obj-m+=hello.o

KVERSION = $(shell uname -r)

all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
	$(CC) testHelloChar.c -o test

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
	sudo rmmod hello
	sudo rm hello

ins:
	sudo insmod ./hello.ko
	sudo mknod hello c 243 0
	sudo chmod 666 /dev/helloDevName

