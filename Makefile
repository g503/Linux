obj-m+=hello.o

KVERSION = $(shell uname -r)

all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
	$(CC) testHelloChar.c -o test

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
	rm test


