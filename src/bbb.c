#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <string.h>
#include <stdbool.h>


#define SENSOR_F0 20
#define SENSOR_F1 20
#define SENSOR_F2 20
float const percentage = 0.38;
#define TEN 10
int max_F0 = 0;
int max_F1 = 0;
int max_F2 = 0;
int min_F0 = 0;
int min_F1 = 0;
int min_F2 = 0;
int avr_F0 = 0;
int avr_F1 = 0;
int avr_F2 = 0;

void updateValues(int,int,int);
int call_once = 1;
int counter = 0;
int main()
{
        // Open the bus in read/write mode.
        // If the returned value is < 0, there was an error.
        int i2cBus1 = open("/dev/i2c-1", O_RDWR);
        int slaveAddress = 0x7;

        // Initiate the handshake with the slave.
        // If the returned value is < 0, there was an error.
        ioctl(i2cBus1, I2C_SLAVE, slaveAddress);
        char* message;
        char buf[17];

        while(1)
        {
				counter = counter + 1;
				
                if(read(i2cBus1,buf,17) != 17)
                {
                        printf("Failed to read from the i2c bus. \n");
                }

                else {
                        //printf("recv %s \n",buf);
                        char* buf0;
                        char* buf1;
                        char* buf2;

                        buf0 = strtok(buf,  " ");
                        buf1 = strtok(NULL, " ");
                        buf2 = strtok(NULL, " ");
                       // printf("Converted data is: F0 %s\tF1 %s\t F2 %s\t", buf0, buf1, buf2);

                        int f0buf = atoi(buf0)/ TEN;
                        int f1buf = atoi(buf1)/ TEN;
                        int f2buf = atoi(buf2)/ TEN;
						
						if (call_once){
							max_F0 = min_F0 = f0buf;
							max_F1 = min_F1 = f1buf;
							max_F2 = min_F2 = f2buf;
							min_F0  -= (min_F0 * percentage);
							max_F1  += (max_F1 * percentage);
							min_F2  -= (min_F2 * percentage);
							call_once = 0;
						}
						
						
						updateValues(f0buf,f1buf,f2buf);
						
						//printf("f0buf: %d  max_F0: %d min_F0: %d  avr_F0: %d \n", f0buf, max_F0, min_F0);
                       printf("Converted data is: F0 %d\tF1 %d\t F2 %d\t", f0buf, f1buf, f2buf);
					   printf(" avr_F0: %d\tavr_F1: %d\t avr_F2: %d\t", avr_F0, avr_F1, avr_F2);
					    bool f0 = false;
                        bool f1 = false;
                        bool f2 = false;

                        if (f0buf < avr_F0)
                                f0 = true;
                        if (f1buf < avr_F1)
                                f1 = true;
                        if (f2buf < avr_F2)
						    	f2 = true;

						 printf("Binary values are %d%d%d\n", f0, f1, f2);


		                 if      (!f0 && f1 && !f2)
		                         message = "fw 0000~";
		                 else if (f0 && !f1 && !f2)
		                         message = "tr 0200,0800~";
		                 else if (f0 && f1 && !f2)
		                         message = "tr 0200 0600~";
		                 else if (!f0 && !f1 && f2)
		                         message = "tr 0800,0200~";
		                 else if (!f0 && f1 && f2)
		                         message = "tr 0600,0200~";
		                 else
		                         message = "bw 0600~";

		                 write(i2cBus1, message, strlen(message));
		                 usleep(500000);
				}
						   //message = "fw 0000~";
						   //               printf("Sending %s\n", message);
						   //               // Send the message. Returns the number of bytes sent.
						   //               write(i2cBus1, message, strlen(message));
						   //               usleep(500000);
						   //       message = "tr 0000,0500~";
						   //               printf("Sending %s\n", message);
						   //               // Send the message. Returns the number of bytes sent.
						   //               write(i2cBus1, message, strlen(message));
						   //               usleep(500000);
						   //       message = "bw 0000~";
						   //               printf("Sending %s\n", message);
						   //               // Send the message. Returns the number of bytes sent.
						   //               write(i2cBus1, message, strlen(message));
						   //               usleep(500000);
						   //       message = "hl 0000~";
						   //               printf("Sending %s\n", message);
						   //               // Send the message. Returns the number of bytes sent.
						   //               write(i2cBus1, message, strlen(message));
						   //               usleep(500000);
		}
						                close(i2cBus1);
}


void updateValues(int f0buf,int f1buf,int f2buf)
{
	
	if(f0buf > max_F0)
	{
		max_F0 = f0buf;
	}
	else if(f0buf < min_F0)	
	{
		min_F0 = f0buf;
	}
	
	avr_F0 = ((max_F0 + min_F0) / 2);
	//#############
	if(f1buf > max_F1)
	{
		max_F1 = f1buf;
	}
	else if(f1buf < min_F1)	
	{
		min_F1 = f1buf;
	}
	
	avr_F1 = ((max_F1 + min_F1) / 2);
	
	//#############
	if(f2buf > max_F2)
	{
		max_F2 = f2buf;
	}
	else if(f2buf < min_F2)	
	{
		min_F2 = f2buf;
	}
	
	avr_F2 = ((max_F2 + min_F2) / 2);
	

}
