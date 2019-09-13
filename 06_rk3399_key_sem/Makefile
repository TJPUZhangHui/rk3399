KERN_DIR = /home/book/work/linux-4.4
CC = aarch64-linux-gnu-gcc
all:
	make -C $(KERN_DIR) M=`pwd` modules 
	
clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order
	
test : rk_key_test

%.o : %.c
	$(CC) -o $@ $< 

obj-m	+= rk_key_drv.o