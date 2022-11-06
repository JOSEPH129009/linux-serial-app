
#include "RS232comm.h"


/**
 * @brief   This function sort serial data for MSB_FIRST & LSB_FIRST on little endian machine.
 * @param   *dest: data pointer for sorted value
 * @param   *src: data pointer to be sorted
 * @param   len: length.
 * @param   endianness: LSB_FIRST or MSB_FIRST
 */
void serial_sort_function(uint8_t *dest,const uint8_t* src, uint8_t len, uint32_t endianness)
{
    int i;
    if(endianness == MSB_FIRST)
    {
        for(i=0; i<len; i++)
            dest[len-i-1] = src[i];
    }
    else
    {
        for(i=0; i<len; i++)
            dest[i] = src[i];
    }
}

const char PORT[] = "/dev/ttyUSB0"; // determine the USB port names from terminal using $ dmesg | grep tty 
// Open a port 
void serial_port_init(int* file_descriptor)
{
    struct termios options; /*serial ports setting*/
    *file_descriptor = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY);    /* ttyUSB0 is the FT232 based USB2SERIAL Converter   */
                            /* O_RDWR   - Read/Write access to serial port       */
                            /* O_NOCTTY - No terminal will control the process   */
                            /* O_NONBLOCK - Open in nonblocking mode,read will not wait              */

    if(*file_descriptor < 0)	/* Error Checking */
    {
        perror("Error opening serial port");
        exit(1);
    }

    tcgetattr(*file_descriptor, &options);	/* Get the current attributes of the Serial port */

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
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Non Cannonical mode                            */

    options.c_oflag &= ~OPOST;/*No Output Processing*/
    options.c_cc[VMIN]  = 1;
    options.c_cc[VTIME] = 1;
    /*Apply the settings*/
    tcflush(*file_descriptor, TCIFLUSH); // cleae input buffer

    if((tcsetattr(*file_descriptor,TCSANOW,&options)) != 0) /* Set the attributes to the termios structure*/
        printf("\n  ERROR ! in Setting attributes");
    else
        printf("\n  BaudRate = 115200 \n  StopBits = 1 \n  Parity   = none\r\n");

}