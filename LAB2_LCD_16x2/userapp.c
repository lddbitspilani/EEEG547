/**
 * @file   testebbchar.c
 * @author Derek Molloy
 * @date   7 April 2015
 * @version 0.1
 * @brief  A Linux user space program that communicates with the ebbchar.c LKM. It passes a
 * string to the LKM and reads the response from the LKM. For this example to work the device
 * must be called /dev/ebbchar.
 * @see http://www.derekmolloy.ie/ for a full description and follow-up descriptions.
*/
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include <sys/ioctl.h>

#define BUFFER_LENGTH 2               ///< The buffer length (crude but fine)
// static char receive[BUFFER_LENGTH];     ///< The receive buffer from the LKM



void delay(uint32_t time)
{
	int c;
	for (c = 0; c<time; c++);
}

int main(){
	int ret, fd;
	int amt;
	int choice;	
	char *p, s[100];
	int result;
	int len, i, flag;
	printf("Starting device test code example...\n");
	
             // Read in a string (with spaces)


  	flag = 1;
	char* receive = "123456789012345678901234567890----";
	
	
	
	fd = open("/dev/DEV_SET_GPIO", O_RDWR);             // Open the device with read/write access
	if (fd < 0){
		perror("Failed to open the device...");
		return errno;
	}
	// ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM
	ret =  ioctl(fd, 1,receive);        // Read the response from the LKM
	if (ret < 0){
		perror("Failed to read the data from the device.");
		return errno;
	}

	delay(100000);


	ret = close(fd);             // Close device
	
	
	
	
	do
	{
		printf("CHOOSE:\n1.(LCD CLEAR)  \n2.(DISPLAY TEXT) \n3.(RESET LCD) \n4.(L-SHIFT CURSOR) \n5.(R-SHIFT CURSOR) \n6.(CUSTOM COMMAND)\n\n0.(Exit program)\n");
	   while (fgets(s, sizeof(s), stdin)) {
       	choice = strtol(s, &p, 10);
      if (p == s || *p != '\n') {
         	printf("Please enter an integer: ");
      	} else break;
    	} 

			switch(choice)

	{
	case 1:
		fd = open("/dev/DEV_SET_GPIO", O_RDWR);             // Open the device with read/write access
		if (fd < 0){
			perror("Failed to open the device...");
			return errno;
		}
		// ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM
		ret =  ioctl(fd, 0,receive);        // Read the response from the LKM
		if (ret < 0){
			perror("Failed to read the data from the device.");
			return errno;
		}

		delay(100000);


		ret = close(fd);             // Close device
		if (ret < 0){
			perror("Failed to close the device...");
			return errno;
		}  break;

	case 2:
		printf("\nEnter Text\n");
		char str[33];
		fgets(str, 33, stdin);
		str[strcspn(str, "\n")] = 0;		
		fd = open("/dev/DEV_SET_GPIO", O_RDWR);             // Open the device with read/write access
		if (fd < 0){
			perror("Failed to open the device...");
			return errno;
		}
		ret = write(fd, str, sizeof(str));        // Read the response from the LKM
		if (ret < 0){
			perror("Failed to write the data to the device.");
			return errno;
		}
		delay(100000);



		ret = close(fd);             // close the device with read/write access
		if (ret < 0){
			perror("Failed to close the device...");
			return errno;
		} break;
		
	case 3:
		fd = open("/dev/DEV_SET_GPIO", O_RDWR);             // Open the device with read/write access
		if (fd < 0){
			perror("Failed to open the device...");
			return errno;
		}
		// ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM
		ret =  ioctl(fd, 1,receive);        // Read the response from the LKM
		if (ret < 0){
			perror("Failed to read the data from the device.");
			return errno;
		}

		delay(100000);


		ret = close(fd);             // Close device
		if (ret < 0){
			perror("Failed to close the device...");
			return errno;
		}  break;
		
	case 4:
		printf("\nENTER L-SHIFT AMT: ");
		
		
		while (fgets(str, sizeof(str), stdin)) 
		{
			amt = strtol(str, &p, 10);
			if (p == s || *p != '\n') 
			{
				printf("Please enter an integer: ");
			} 
			else 
				break;
    	}
		fd = open("/dev/DEV_SET_GPIO", O_RDWR);             // Open the device with read/write access
		if (fd < 0){
			perror("Failed to open the device...");
			return errno;
		}
		// ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM
		ret =  ioctl(fd,5,amt);        // Read the response from the LKM
		if (ret < 0){
			perror("Failed to read the data from the device.");
			return errno;
		}

		delay(100000);


		ret = close(fd);             // Close device
		if (ret < 0){
			perror("Failed to close the device...");
			return errno;
		}  break;
		
	case 5:
		printf("\nENTER R-SHIFT AMT: ");
		
		
		while (fgets(str, sizeof(str), stdin)) 
		{
			amt = strtol(str, &p, 10);
			if (p == s || *p != '\n') 
			{
				printf("Please enter an integer: ");
			} 
			else 
				break;
    	}
		fd = open("/dev/DEV_SET_GPIO", O_RDWR);             // Open the device with read/write access
		if (fd < 0){
			perror("Failed to open the device...");
			return errno;
		}
		// ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM
		ret =  ioctl(fd,3,amt);        // Read the response from the LKM
		if (ret < 0){
			perror("Failed to read the data from the device.");
			return errno;
		}

		delay(100000);


		ret = close(fd);             // Close device
		if (ret < 0){
			perror("Failed to close the device...");
			return errno;
		}  break;
		
	case 6:
		printf("\nENTER HEX CODE FOR CUSTOM COMMAND (no 0x appended): ");
		
		uint8_t code;
		scanf("%x", &code);
		
		
		fd = open("/dev/DEV_SET_GPIO", O_RDWR);             // Open the device with read/write access
		if (fd < 0){
			perror("Failed to open the device...");
			return errno;
		}
		// ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM
		ret =  ioctl(fd,7,code);        // Read the response from the LKM
		if (ret < 0){
			perror("Failed to read the data from the device.");
			return errno;
		}

		delay(100000);


		ret = close(fd);             // Close device
		if (ret < 0){
			perror("Failed to close the device...");
			return errno;
		}  break;



	case 0:
		flag=-1;
		printf("End of the program\n");
		return 0;
		break;

	default:
	printf("Incorrect Entry\n");
	break;
	}

	} while (flag>0); 

	printf("End of the program\n");
	return 0;
}


