
### spi - oled驱动 ###

**1 连接硬件**

- spi接MOSI引脚,不接MISO引脚，因为没有用
- CLK正常接
- CS直接接地
- VCC 接 3.3V
- DC引脚为数据/指令切换引脚，3.3V为1，rk输出为1.8v，因此要加一个MOS驱动
- RES引脚为低电平复位，也需要加MOS驱动

使用RK3399的spi引脚为spi2,dts配置文件在文件夹dts文件的最后

