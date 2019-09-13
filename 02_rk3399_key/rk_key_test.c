
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include <unistd.h>

int main(int argc, char **argv)
{
	int fd;
	int val;
	
	fd = open("/dev/key", O_RDWR);
	if (fd < 0)
	{
		printf("can't open!\n");
	}
	while(1)
	{
		read(fd, &val, sizeof(val));
		printf("val = %d\n", val);
	}
	return 0;
}

