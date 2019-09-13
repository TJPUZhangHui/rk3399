#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/printk.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h> 
#include <linux/interrupt.h>
#include <linux/poll.h>


static int gpio;
static int major;
static struct class *key_cls;
static struct device *key_dev;
static int irq_num;

static int rn_to_user;

/* 定义等待队列头部 */
static volatile int ev_press = 0;
static DECLARE_WAIT_QUEUE_HEAD(key_wait_queue);

static int rk_key_open (struct inode *node, struct file *pfile)
{
	
	return 0;
}

static ssize_t rk_key_read (struct file *pfile, char __user * buf, size_t size, loff_t *off)
{	
	int res;
		
	res = copy_to_user(buf, &rn_to_user, sizeof(rn_to_user));
	return sizeof(rn_to_user);
}


static int rk_key_release (struct inode *node, struct file *pfile)
{
	
	return 0;
}

unsigned int rk_key_poll (struct file *pfile, struct poll_table_struct *wait)
{
	unsigned int mask = 0;

	/* 把本进程挂到这个等待队列上 */
	poll_wait(pfile, &key_wait_queue, wait);

	if (ev_press)
		mask |= POLLIN | POLLRDNORM;
	
	ev_press = 0;
	return mask;
}


static irqreturn_t rk_key_irq(int irq, void *dev_id)
{
	rn_to_user++;

	/* 唤醒等待队列上的进程 */
	ev_press = 1;
	wake_up_interruptible(&key_wait_queue);
	
	return IRQ_HANDLED;
}


static struct file_operations my_key_fops = {
	.owner    = THIS_MODULE,
	.open     = rk_key_open,
	.read     = rk_key_read,
	.release  = rk_key_release,
	.poll     = rk_key_poll,
};


static int rk_key_probe(struct platform_device *dev)
{
	int ret;
	enum of_gpio_flags flag;
	struct device_node *rk_key_node = dev->dev.of_node;
	
	gpio = of_get_named_gpio_flags(rk_key_node, "gpios", 0, &flag);
	
	/* >0 有效   ； <0 无效 */
	if (!gpio_is_valid(gpio))
	{	
		printk(KERN_INFO "gpio 无效\n");
		return -ENODEV;
	}
	printk(KERN_INFO "gpio num is %d\n", gpio);	
	/* 申请GPIO */
	gpio_request(gpio, "gpios");

	irq_num = gpio_to_irq(gpio);
	ret     = request_irq(irq_num, rk_key_irq, flag, "key_irq", (void *)1);
	if (ret != 0) 
		free_irq(irq_num, (void *)1);
	
	/* 创建字符设备驱动 */
	major = register_chrdev(0, "mykey", &my_key_fops);
	key_cls = class_create(THIS_MODULE, "mykey");
	key_dev = device_create(key_cls,   NULL, MKDEV(major, 0), NULL, "key"); 	/* /dev/key */
	
	printk(KERN_INFO "%s\n", __func__);
	return 0;
	
}

static int rk_key_remove(struct platform_device * dev)
{
	unregister_chrdev(major, "mykey");
	device_unregister(key_dev);
	class_destroy(key_cls);

	free_irq(irq_num, (void *)1);
	gpio_free(gpio); 
	printk(KERN_INFO "%s\n", __func__);
	return 0;
}



static struct of_device_id of_match_rk_key[] = {

	{.compatible = "firefly,mykey", .data = NULL},
	{ /* NULL */ }
};

static struct platform_driver rk_key_drv = {

	.probe 		= rk_key_probe,
	.remove 	= rk_key_remove,
	.driver		= {
		.name = "my_key",
		.of_match_table = of_match_rk_key,
	} 
};

static int rk_key_drv_init(void)
{
	printk(KERN_INFO "%s\n", __func__);
	platform_driver_register(&rk_key_drv);
	return 0;

}


static void rk_key_drv_exit(void)
{
	printk(KERN_INFO "%s\n", __func__);
	platform_driver_unregister(&rk_key_drv);
}



module_init(rk_key_drv_init);
module_exit(rk_key_drv_exit);
MODULE_LICENSE("GPL");



