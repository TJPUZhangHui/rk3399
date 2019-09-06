
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include <unistd.h>

int main(int argc, char **argv)
{
	int fd;
	unsigned char val = 1;
	fd = open("/dev/led", O_RDWR);
	if (fd < 0)
	{
		printf("can't open!\n");
	}
	if (argc != 2)
	{
		printf("Usage :\n");
		printf("%s <on|off|auto>\n", argv[0]);
		return 0;
	}

	if (strcmp(argv[1], "on") == 0)
	{
		val = 1; 
		write(fd, &val, 1);
	}
	else if (strcmp(argv[1], "off") == 0)
	{
		val = 0;
		write(fd, &val, 1);
	}
	else 
	{	
		while(1)
		{	
			val = 1;
			write(fd, &val, 1);
			sleep(1);
			
			val = 0;
			write(fd, &val, 1);
			sleep(1);
		}
	}
	
	return 0;
}

