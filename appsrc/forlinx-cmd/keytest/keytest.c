#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#define NOKEY 0

int main(int argc, const char *argv[])
{
	int keys_fd;	
	struct input_event t;

	setvbuf(stdout, (char *)NULL, _IONBF, 0);//disable stdio out buffer;

	keys_fd = open("/dev/input/by-path/platform-keys-event", O_RDONLY);
	if(keys_fd<=0)
	{
		printf("open %s device error!\n", "/dev/input/keypad");
		return 0;
	}

	while(1)
	{	
		if(read(keys_fd,&t,sizeof(t))==sizeof(t)) {
			if(t.type==EV_KEY)
				if(t.value==0 || t.value==1)
				{
					printf("key%d %s\n",t.code, (t.value)?"Presse":"Released");

				}
		}
	}	
	close(keys_fd);

	return 0;
}

