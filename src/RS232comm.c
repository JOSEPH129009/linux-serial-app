#include <stdio.h>		
#include <string.h>
#include <unistd.h>		// Unix standard functions
#include <fcntl.h>		// File control funcitons
#include <errno.h>		// Error number definitions
#include <termios.h>	// POSIX terminal control definitions
#include "RS232comm.h"
#include "stdlib.h"

const char PORT[] = "/dev/ttyUSB0"; // determine the USB port names from terminal using $ dmesg | grep tty 
// Open a port 
void serial_port_init(int* fd)
{
    struct termios options; /*serial ports setting*/
    *fd = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY);    /* ttyUSB0 is the FT232 based USB2SERIAL Converter   */
                            /* O_RDWR   - Read/Write access to serial port       */
                            /* O_NOCTTY - No terminal will control the process   */
                            /* Open in blocking mode,read will wait              */

    if(*fd < 0)					/* Error Checking */
    {
        perror("Error opening serial port");
        exit(1);
    }


    tcgetattr(*fd, &options);	/* Get the current attributes of the Serial port */

    /* Setting the Baud rate */
    cfsetispeed(&options,B115200); /* Set Read  Speed as 115200                       */
    cfsetospeed(&options,B115200); /* Set Write Speed as 115200                       */

    /* 8N1 Mode */
    options.c_cflag &= ~PARENB;   /* Disables the Parity Enable bit(PARENB),So No Parity   */
    options.c_cflag &= ~CSTOPB;   /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
    options.c_cflag &= ~CSIZE;	 /* Clears the mask for setting the data size             */
    options.c_cflag |=  CS8;      /* Set the data bits = 8                                 */
    
    options.c_cflag &= ~CRTSCTS;       /* No Hardware flow Control                         */
    options.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */ 
    
    
    options.c_iflag &= ~(IXON | IXOFF | IXANY);          /* Disable XON/XOFF flow control both i/p and o/p */
    options.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Non Cannonical mode                            */

    options.c_oflag &= ~OPOST;/*No Output Processing*/

    /*Apply the settings*/
    tcflush(*fd, TCIFLUSH); // cleae input buffer

    if((tcsetattr(*fd,TCSANOW,&options)) != 0) /* Set the attributes to the termios structure*/
        printf("\n  ERROR ! in Setting attributes");
    else
        printf("\n  BaudRate = 115200 \n  StopBits = 1 \n  Parity   = none\r\n");

}


