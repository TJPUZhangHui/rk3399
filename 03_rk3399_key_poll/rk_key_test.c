
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include <unistd.h>
#include <poll.h>


int main(int argc, char **argv)
{
	int fd;
	int val;
	int res;
	struct pollfd fds[1];
	
	fd = open("/dev/key", O_RDWR);
	if (fd < 0)
	{
		printf("can't open!\n");
	}
	
	fds[0].fd = fd;
	fds[0].events = POLLIN;

	while (1)
	{
		res = poll(fds, 1, 5000);
		if (res == 0)
			printf("time out\n");
		else 
		{
			read(fd, &val, 1);
			printf("key val is %d\n",val);
		}
	}

	return 0;
}



