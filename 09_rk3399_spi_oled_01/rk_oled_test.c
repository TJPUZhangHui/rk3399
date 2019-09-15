
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	int fd;
	unsigned char  val;
	int res;

	fd = open("/dev/oled", O_RDWR);
	if (fd < 0)
	{
		printf("can't open!\n");
		return -1;
	}
	
	write(fd, &val, 1);
	return 0;
}




