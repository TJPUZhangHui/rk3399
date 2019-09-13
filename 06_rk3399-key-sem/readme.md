

```
定义信号量
struct semaphore sem;
初始化信号量
void sema_init (struct semaphore *sem, int val);
void init_MUTEX(struct semaphore *sem);//初始化为0

获得信号量
void down(struct semaphore * sem);
int down_interruptible(struct semaphore * sem); 
int down_trylock(struct semaphore * sem);
释放信号量
void up(struct semaphore * sem);
```

