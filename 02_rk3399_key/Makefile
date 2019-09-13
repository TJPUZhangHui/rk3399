KERN_DIR = /home/book/work/linux-4.4

all:
	make -C $(KERN_DIR) M=`pwd` modules 
	
clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

obj-m	+= rk_key_drv.o