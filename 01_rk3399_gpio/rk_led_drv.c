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

/*
	平台驱动 相关的结构体
1	struct platform_driver {
		int (*probe)(struct platform_device *);
		int (*remove)(struct platform_device *);
		void (*shutdown)(struct platform_device *);
		int (*suspend)(struct platform_device *, pm_message_t state);
		int (*resume)(struct platform_device *);
		struct device_driver driver;
		const struct platform_device_id *id_table;
		bool prevent_deferred_probe;
	};
2	struct device_driver {
		... ...
		const struct of_device_id	*of_match_table;
		struct driver_private *p;
		... ...
	};
	
3	struct of_device_id {
		char	name[32];
		char	type[32];
		char	compatible[128];
		const void *data;
	};
*/

/*
	GPIO子系统相关结构体
	enum of_gpio_flags {
		OF_GPIO_ACTIVE_LOW = 0x1,
		OF_GPIO_SINGLE_ENDED = 0x2,
	};
*/

/*
	平台设备 相关的结构体
	struct platform_device {
		const char	*name;
		int		id;
		bool		id_auto;
		struct device	dev;
		u32		num_resources;
		struct resource	*resource;

		const struct platform_device_id	*id_entry;
		char *driver_override; // Driver name to force a match 

		// MFD cell pointer 
		struct mfd_cell *mfd_cell;

		// arch specific additions 
		struct pdev_archdata	archdata;
	};
	
	struct resource {
		resource_size_t start;
		resource_size_t end;
		const char *name;
		unsigned long flags;
		struct resource *parent, *sibling, *child;
	};
	
	struct device_node {
		const char *name;
		const char *type;
		phandle phandle;
		const char *full_name;
		struct fwnode_handle fwnode;

		struct	property *properties;
		struct	property *deadprops;	// removed properties 
		struct	device_node *parent;
		struct	device_node *child;
		struct	device_node *sibling;
		struct	kobject kobj;
		unsigned long _flags;
		void	*data;
};
*/

static int gpio;
static int major;
static struct class *led_cls;


static int rk_led_open (struct inode *node, struct file *pfile)
{
	
	return 0;
}

static ssize_t rk_led_write (struct file *pfile, const char __user * buf, size_t size, loff_t *offset)
{	
	int res = 0;
	unsigned char val;
	res = copy_from_user(&val, buf, 1);
	
	if (val == 1)
		gpio_direction_output(gpio, 1);
	else if (val == 0)
		gpio_direction_output(gpio, 0);

	return 1;
}

static int rk_led_release (struct inode *node, struct file *pfile)
{
	return 0;
}


static struct file_operations my_led_fops = {
	.owner = THIS_MODULE,
	.open = rk_led_open,
	.write = rk_led_write,
	.release = rk_led_release,
};

static int rk_led_probe(struct platform_device *dev)
{
	enum of_gpio_flags flag;
	struct device_node *rk_led_node = dev->dev.of_node;
	
	gpio = of_get_named_gpio_flags(rk_led_node, "gpios", 0, &flag);
	
	/* >0 有效   ； <0 无效 */
	if (!gpio_is_valid(gpio))
	{	
		printk(KERN_INFO "gpio 无效\n");
		return -ENODEV;
	}
	printk(KERN_INFO "gpio num is %d\n", gpio);	
	/* 申请GPIO */
	gpio_request(gpio, "gpios");

	/* 创建字符设备驱动 */
	major = register_chrdev(0, "myled", &my_led_fops);
	led_cls = class_create(THIS_MODULE, "myled");
	device_create(led_cls,   NULL, MKDEV(major, 0), NULL, "led");
	
	printk(KERN_INFO "%s\n", __func__);
	return 0;
}

static int rk_led_remove(struct platform_device * dev)
{
	printk(KERN_INFO "%s\n", __func__);
	gpio_free(gpio); 
	return 0;
}



static struct of_device_id of_match_rk_led[] = {
	{.compatible = "firefly,myled", .data = NULL},
	{ /* NULL */ }
};

static struct platform_driver rk_led_drv = {
	.probe = rk_led_probe,
	.remove = rk_led_remove,
	.driver = {
		.name = "my_led",
		.of_match_table = of_match_rk_led,
	} 
};

static int rk_led_drv_init(void)
{
	printk(KERN_INFO "%s\n", __func__);
	platform_driver_register(&rk_led_drv);
	return 0;
}

static void rk_led_drv_exit(void)
{
	platform_driver_unregister(&rk_led_drv);
}

module_init(rk_led_drv_init);
module_exit(rk_led_drv_exit);
MODULE_LICENSE("GPL");


