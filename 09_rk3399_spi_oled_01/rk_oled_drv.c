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
#include <linux/timer.h>
#include <linux/spi/spi.h>

struct spi_oled {
	unsigned int gpio_dc;
	unsigned int gpio_res;
	int major;
	struct class *oled_cls;
	struct device *oled_device;
	
	struct spi_device *spi_oled_device;
	unsigned char oled_gram[128][8];
};
static struct spi_oled g_oled ;

static void delay(void)
{
	int i, j;
	for (i = 100000; i > 0; i --)
		for (j = 100000; j > 0; j --)
			{
			}
}

static void oled_set_DC(char val)
{
	if (val)
		gpio_direction_output(g_oled.gpio_dc, 1);
	else 
		gpio_direction_output(g_oled.gpio_dc, 0);
}

static void spi_send_byte(unsigned char val)
{
	if (g_oled.spi_oled_device != NULL)
		spi_write(g_oled.spi_oled_device, &val, 1);
}

static void oled_write_cmd(unsigned char cmd)
{
	oled_set_DC(1);
	spi_send_byte(cmd);
	oled_set_DC(0);
}

static void oled_write_data(unsigned char dat)
{
	oled_set_DC(0);
	spi_send_byte(dat);
	oled_set_DC(0);
}

static void oled_set_page_mode(void)
{
	oled_write_cmd(0x20);
	oled_write_cmd(0x02);
}

static void oled_set_pos(int page, int col)
{
	oled_write_cmd(0xB0 + page);

	oled_write_cmd(col & 0xf);
	oled_write_cmd(0x10 + (col >> 4));

}

static void oled_clear(void)
{
    int page, i;
    for (page = 0; page < 8; page ++)
    {
        oled_set_pos(page, 0);
        for (i = 0; i < 128; i++)
            oled_write_data(0);
    }
}

void oled_clear_page(int page)
{
    int i;
    oled_set_pos(page, 0);
    for (i = 0; i < 128; i++)
        oled_write_data(0);    
}

//更新显存到OLED	
void __oled_refresh(void)
{
	u8 i,n;
	for(i=0; i<8; i++)
	{
	   oled_write_cmd(0xb0+i); //设置行起始地址
	   oled_write_cmd(0x00);   //设置低列起始地址
	   oled_write_cmd(0x10);   //设置高列起始地址
	   for(n = 0; n < 128; n++)
		 oled_write_data(g_oled.oled_gram[n][i]);
  }
}
//清屏函数
void __oled_clear(void)
{
	u8 i,n;
	for(i = 0; i < 8; i++)
	{
		for(n = 0; n < 32; n++)
		{
			g_oled.oled_gram[n][i] = 0; //清除所有数据
		}
		for(n = 32; n < 64; n++)
		{
			g_oled.oled_gram[n][i] = 0xff; //清除所有数据
		}
		for(n = 64; n < 96; n++)
		{
			g_oled.oled_gram[n][i] = 0; //清除所有数据
		}
		for(n = 96; n < 128; n++)
		{
			g_oled.oled_gram[n][i] = 0xff; //清除所有数据
		}
	}
	__oled_refresh();//更新显示
}

void __oled_display_on(void)
{
	oled_write_cmd(0x8D);//电荷泵使能
	oled_write_cmd(0x14);//开启电荷泵
	oled_write_cmd(0xAF);//点亮屏幕
}

static void oled_init(void)
{	
    /* 向OLED发命令以初始化 */
    oled_write_cmd(0xAE); /*display off*/ 
    oled_write_cmd(0x00); /*set lower column address*/ 
    oled_write_cmd(0x10); /*set higher column address*/ 
    oled_write_cmd(0x40); /*set display start line*/ 
    oled_write_cmd(0x81); /*set page address*/ 
    oled_write_cmd(0x81); /*contract control*/ 
    oled_write_cmd(0xCF); /*128*/ 
    oled_write_cmd(0xA1); /*set segment remap*/ 
    oled_write_cmd(0xC8); /*normal / reverse*/ 
    oled_write_cmd(0xA6); /*multiplex ratio*/ 
    oled_write_cmd(0xA8); /*duty = 1/64*/ 
    oled_write_cmd(0x3f); /*Com scan direction*/ 
    oled_write_cmd(0xD3); /*set display offset*/ 
    oled_write_cmd(0x00); 
    oled_write_cmd(0xD5); /*set osc division*/ 
    oled_write_cmd(0x80); 
    oled_write_cmd(0xD9); /*set pre-charge period*/ 
    oled_write_cmd(0xF1); 
    oled_write_cmd(0xDA); /*set COM pins*/ 
    oled_write_cmd(0x12); 
    oled_write_cmd(0xdb); /*set vcomh*/ 
    oled_write_cmd(0x40); 
    oled_write_cmd(0x20); /* set page mode */ 
    oled_write_cmd(0x02); 
	oled_write_cmd(0x8D); 
    oled_write_cmd(0x14); /*set vcomh*/ 
    oled_write_cmd(0xA4); 
    oled_write_cmd(0xA7); /*set charge pump enable*/ 
    oled_write_cmd(0xAF); 

	 __oled_clear();
	
}

static int oled_open (struct inode *node, struct file *pfile)
{	
	printk(KERN_INFO "%s\n", __func__);
	return 0;
}

static ssize_t oled_write (struct file *pfile, const char __user * buf, size_t size, loff_t *offset)
{	

	oled_init();
	printk(KERN_INFO "oled_init\n");
	return 1;
}

static int oled_release (struct inode *node, struct file *pfile)
{
	return 0;
}

struct file_operations oled_fops = {
	.owner   = THIS_MODULE,
	.open    = oled_open,
	.write   = oled_write,
	.release = oled_release,
};

static int spi_oled_probe(struct spi_device *spi)
{
	int ret;
	enum of_gpio_flags flag;
	struct device_node *spi_oled_node = spi->dev.of_node;

	g_oled.spi_oled_device = spi;

	/* 1 从设备树获取gpio号 */
	g_oled.gpio_dc = of_get_named_gpio_flags(spi_oled_node, "dc", 0, &flag);
	if (!gpio_is_valid(g_oled.gpio_dc))
	{	
		printk(KERN_INFO "gpio 无效\n");
		return -ENODEV;
	}
	printk(KERN_INFO "gpio num is %d\n", g_oled.gpio_dc);	
	/* 2 申请GPIO */
    gpio_request(g_oled.gpio_dc, "dc");
	
	/* 1 从设备树获取gpio号 */
	g_oled.gpio_res = of_get_named_gpio_flags(spi_oled_node, "res", 0, &flag);
	if (!gpio_is_valid(g_oled.gpio_res))
	{	
		printk(KERN_INFO "gpio 无效\n");
		return -ENODEV;
	}
	printk(KERN_INFO "gpio num is %d\n", g_oled.gpio_res);	
	/* 2 申请GPIO */
	gpio_request(g_oled.gpio_res, "dc");

	g_oled.major = register_chrdev(0, "oled", &oled_fops);
	g_oled.oled_cls = class_create(THIS_MODULE, "oled_cls");
	g_oled.oled_device = device_create(g_oled.oled_cls, NULL, MKDEV(g_oled.major, 0), NULL, "oled"); /* /dev/oled */	

	spi->max_speed_hz = 1000000;
	spi->bits_per_word = 8;
    printk(KERN_INFO "max_speed_hz : %d mode : %d cs gpio : %d \n", spi->max_speed_hz, spi->mode, spi->cs_gpio);
	
	ret = spi_setup(spi);
	if (ret < 0)
	{
		printk(KERN_INFO "spi setup error\n");
		return -EINVAL;
	}
 	
	printk(KERN_INFO "%s\n", __func__);
	return 0;
}

static int spi_oled_remove(struct spi_device *spi)
{
	gpio_free(g_oled.gpio_dc); 
	gpio_free(g_oled.gpio_res); 
	
	unregister_chrdev(g_oled.major, "oled");
	device_destroy(g_oled.oled_cls, MKDEV(g_oled.major, 0));
	class_destroy(g_oled.oled_cls);
	
	printk(KERN_INFO "%s\n", __func__);
	return 0;
}

static struct of_device_id spi_oled_match_table[] = 
{
	{ .compatible = "myspi,oled", .data = NULL},
	{ /*  */ },

};

static struct spi_driver spi_oled_driver = {
	.driver = {
		.name = "spi_oled",
		.owner = THIS_MODULE,
		.of_match_table = spi_oled_match_table,
		},
	.probe = spi_oled_probe,
	.remove = spi_oled_remove,
};


static int rk_spi_oled_init(void)
{
	printk(KERN_INFO "%s\n", __func__);
	spi_register_driver(&spi_oled_driver);
	return 0;
}

static void rk_spi_oled_exit(void)
{
	printk(KERN_INFO "%s\n", __func__);
	spi_unregister_driver(&spi_oled_driver);
}

module_init(rk_spi_oled_init);
module_exit(rk_spi_oled_exit);
MODULE_LICENSE("GPL");


