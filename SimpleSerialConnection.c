/*
 ============================================================================
 Name        : SimpleSerialConnection.c
 Author      : DA
 Version     :
 Copyright   : Your copyright notice
 Description : Simple script that connects to a serial port, sends a command and 
               waits for the response. 
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>    /* Error number definitions */
#include <termios.h>  /* POSIX terminal control definitions */
#include <stdint.h>   /* Standard types */
#include <string.h>   /* String function definitions */
#include <unistd.h>   /* UNIX standard function definitions */
#include <fcntl.h>    /* File control definitions */

int main(void) {
	int fd;
	const char *device="/dev/ttyUSB0";
	struct termios config;
	fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
	if (fd == -1){
		printf("failed to open port\n");
		exit(-3);
	}
	if(!isatty(fd)){
		printf("Descriptor fd is not connected to a terminal device\n");
		exit(-4);
	}
	if(tcgetattr(fd, &config)<0){
		printf("error getting serial port attribute!\n");
		exit(-5);
	}
	/* these setting are equivalent to use cfmakeraw() */
	config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
	//	config.c_oflag &= ~(OCRNL | ONLCR | ONLRET | ONOCR | ONOEOT | OFILL | OLCUC | OPOST);
	config.c_oflag=0; //equivalent to set each flag as the instruction commented above
	config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
	config.c_cflag &= ~(CSIZE | PARENB);
	config.c_cflag |= CS8;
	config.c_cc[VMIN]=6;
	config.c_cc[VTIME]=0;
	if(cfsetispeed(&config, B115200)<0 || cfsetospeed(&config, B115200)<0){
		printf("error setting baud rate\n");
		exit(-6);
	}
	if(tcsetattr(fd, TCSANOW, &config)<0){
		printf("impossible set the new attribute for serial port\n");
		exit(-7);
	}


	unsigned char mystring[6];
	unsigned char ch[6];
	int j;
	for(j=0; j<6; j++)
		ch[j]=0;

	mystring[0]=0xFE;
	mystring[1]=0x01;
	mystring[2]=0X4D;
	mystring[3]=0X10;
	mystring[4]=0X00;
	mystring[5]=0X5C;
	int n_write, n_read;
	n_write=write(fd, (const void *)&mystring, (size_t)6);
	printf("No. of written bytes %d\n", n_write);
	usleep(500);
	int i=0;
	n_read=read(fd,(void *)&ch,1);
	if(n_read>-1){
		i=5;
		printf("No. of read data %d\n", n_read);
	}
	while(i<5){
		usleep(100);
		n_read=read(fd,(void *)ch,6);
		if(n_read>-1){
			printf("No. of read data %d\n", n_read);
			break;
		}
		else{
			i++;
		}
	}
	close(fd);
	puts("!!!Done!!!");
	return EXIT_SUCCESS;
}
