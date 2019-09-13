
定时器按键消抖

```
1 初始化定时器，设置定时器中断服务函数
void init_timer(struct timer_list * timer);
xxx_timer.function = &xxx_do_timer;

2 注册定时器进内核
void add_timer(struct timer_list * timer);

3 修改定时器的延时时间
int mod_timer(struct timer_list *timer, unsigned long expires);


```