
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>
#include <sys/types.h>

static int fd;

void my_handle(int num)
{
	int val;
	read(fd, &val, sizeof(val)); 
	printf("%d : val = %d\n", getpid(), val);
}

int main(int argc, char **argv)
{
	int res;
	int flag;
	
	fd = open("/dev/key", O_RDWR);
	if (fd < 0)
	{
		printf("can't open!\n");
		return -1;
	}

	/* 设置设备文件的拥有者为本进程 */
	fcntl(fd, F_SETOWN, getpid()); 
	flag = fcntl(fd, F_GETFL); 
	fcntl(fd, F_SETFL, flag | FASYNC);
	
	signal(SIGIO, my_handle);
	
	while (1)
	{
		sleep(1000);
	}

	return 0;
}



