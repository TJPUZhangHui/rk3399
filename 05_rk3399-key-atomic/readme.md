




```
atomic_t v = ATOMIC_INIT(0);     //定义原子变量v并初始化为0
atomic_read(atomic_t *v);        //返回原子变量的值
void atomic_inc(atomic_t *v);    //原子变量增加1
void atomic_dec(atomic_t *v);    //原子变量减少1
int atomic_dec_and_test(atomic_t *v); //自减操作后测试其是否为0，为0则返回true，否则返回false。

```
