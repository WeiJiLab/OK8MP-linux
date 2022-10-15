#include     <stdio.h>      /*��׼�����������*/
#include     <stdlib.h>     /*��׼�����ⶨ��*/
#include     <unistd.h>     /*Unix��׼��������*/
#include     <sys/types.h>  /**/
#include     <sys/stat.h>   /**/
#include     <fcntl.h>      /*�ļ����ƶ���*/
#include     <termios.h>    /*PPSIX�ն˿��ƶ���*/
#include     <errno.h>      /*����Ŷ���*/
#include	 <sys/time.h>
#include     <string.h>
#include     <getopt.h>

#define TRUE 1
#define FALSE -1

int main(int argc, char **argv)
{
    int fd;
    int nread;
    char buffer[512];
    int n=0,i=0;
    char* dev  = NULL;
    struct termios oldtio,newtio;
	speed_t speed = B115200;
    int next_option,havearg = 0,flow = 0;
    const char *const short_opt = "fd:";
    const struct option long_opt[] = {
        {"devices",1,NULL,'d'},
        {"hard_flow",0,NULL,'f'},
        {NULL,0,NULL,0},
    };
    do{
        next_option = getopt_long(argc,argv,short_opt,long_opt,NULL);
        switch (next_option) {
            case 'd':
                dev = optarg;
                havearg = 1;
                break;
            case '?':
				printf("Usage: %s -d <device>\n", argv[0]);
                break;
            case -1:
                if(havearg)
                    break;
            default:
				printf("Usage: %s -d <device>\n", argv[0]);
                exit(1);

        }
    }while(next_option != -1);

    if(dev == NULL)
    {
			printf("Usage: %s -d <device>\n", argv[0]);
            exit(1);
    }	
    /*  �򿪴��� */
    fd = open(dev, O_RDWR | O_NONBLOCK| O_NOCTTY | O_NDELAY); 
    if (fd < 0)	{
        printf("Can't Open Serial Port!\n");
        exit(0);	
    }
	
	printf("Welcome to uart test\n");
	
    //save to oldtio
    tcgetattr(fd,&oldtio);
    bzero(&newtio,sizeof(newtio));
    newtio.c_cflag = speed|CS8|CLOCAL|CREAD;
    newtio.c_cflag &= ~CSTOPB;
    newtio.c_cflag &= ~PARENB;
    newtio.c_iflag = IGNPAR;  
    newtio.c_oflag = 0;
    tcflush(fd,TCIFLUSH);  
    tcsetattr(fd,TCSANOW,&newtio);  
    tcgetattr(fd,&oldtio);
	
    memset(buffer,0,sizeof(buffer));

    char test[100]="forlinx_uart_test.1234567890...";

    printf("Send test data:\n%s\n",test);
	write(fd, test, strlen(test) + 1);
	fd_set rd;
    while(1)
    {
		int ret;
//		FD_ZERO(&rd);
//		FD_SET(fd,&rd);
//        ret = select(fd+1,&rd,NULL,NULL,NULL);
		nread = read(fd, &buffer[n], 1);
        if (strlen(test) == strlen(buffer))
        {
            printf("Read Test Data finished,Read:\n%s\n",buffer);
            memset(buffer,0,sizeof(buffer));
            tcflush(fd, TCIOFLUSH);
            break;
        }
        n += nread;
    }
    close(fd);
}


