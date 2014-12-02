#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <string.h>

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

        if(     read(i2cBus1,buf,17) != 17)
        {
                printf("Failed to read from the i2c bus. \n");
        }

        else {
                printf("recv %s \n",buf);
        }


        // message = "fw 0000~";
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