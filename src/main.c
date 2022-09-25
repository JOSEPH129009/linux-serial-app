
/*====================================================================================================*/
    /* Serial Port Programming in C                                                                       */
/*----------------------------------------------------------------------------------------------------*/
    /* Program opens the serial port and closes it                                                        */
/*----------------------------------------------------------------------------------------------------*/
/* Compiler/IDE  : gcc 4.6.3                                                                          */
/* Library       :                                                                                    */
/* Commands      : gcc -o serial serial.c                                                             */
/* OS            : Linux(x86) (Linux Mint 13 Maya/Ubuntu)(Linux Kernel 3.x.x)                         */                              
/* Programmer    : Rahul.S                                                                            */
/* Date	         : 21-December-2014                                                                   */
/*====================================================================================================*/

/*====================================================================================================*/
/* www.xanthium.in										      */
/* Copyright (C) 2014 Rahul.S                                                                         */
/*====================================================================================================*/

/*====================================================================================================*/
/* Running the executable                                                                             */
/* ---------------------------------------------------------------------------------------------------*/ 
/* 1) Compile the  serial.c  file using gcc on the terminal (without quotes)                          */
    /*                                                                                                    */
/*	" gcc -o serial serial.c "                                                                    */
/*                                                                                                    */
    /* 2) Linux will not allow you to access the serial port from user space,you have to be root.So use   */
    /*    "sudo" command to execute the compiled binary as super user.                                    */
    /*                                                                                                    */
    /*       "sudo ./serial"                                                                              */
/*                                                                                                    */
/*====================================================================================================*/

/*====================================================================================================*/
/* Sellecting the Serial port Number on Linux                                                         */
/* ---------------------------------------------------------------------------------------------------*/ 
/* /dev/ttyUSBx - when using USB to Serial Converter, where x can be 0,1,2...etc                      */
/* /dev/ttySx   - for PC hardware based Serial ports, where x can be 0,1,2...etc                      */
    /*====================================================================================================*/

#include <stdio.h>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions 	   */ 
#include <errno.h>   /* ERROR Number Definitions           */
#include "string.h"
#include "stdlib.h"
#include "time.h"
#include "stdbool.h"
#include "frame.h"

const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1; 
}


bool Time_Interval(clock_t time, clock_t gap)
{
    // _Static_assert(sizeof(int) == 4, "wrong" );
    clock_t current_time = clock();
    if(time <= clock())
    {
        if((time - gap) >= current_time)
            return true;
        else
            return false;
    }
    else
    {  
        if(((clock_t)0xFFFFFFFFFFFFFFFF - time + current_time + 1) >= gap)
            return true;
        else
            return false;
    }
}

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


int main(int argc, char** argv)
{
    //-----------file
    FILE *ptr;
    int i,k;
    const char* filename;
    //----------serialport
    int fd; /*File Descriptor*/
    int len;
    char text[255];
    int rlen;
    char rtext[255];
    //----------read usr input
    char* buffer;
    size_t bufsize = 32;
    size_t characters;
    //----------

    if(argc != 2)
    {
        perror("wrong argument");
        exit(1);
    }

    filename = get_filename_ext(argv[argc - 1]);
    if(strcmp(filename, "bin"))
    {
        perror("argument is not a bin file");
        exit(1);
    }
    
    ptr = fopen(argv[argc - 1],"rb");  // r for read, b for binary
    if(ptr==NULL)
    {
        perror("Error Reporting");
        exit(1);
    }
    serial_port_init(&fd);

  

    /*Write to serial port*/
    // bootloader_init();
    strcpy(text, "Hello from_PC\r\n");
    len = strlen(text);
    len = write(fd, text ,len);
    printf("wrote %d bytes over UART\n", len);
    // len = write(fd, &transmit_frame ,sizeof(frame_format_t));
    // printf("wrote %ld bytes over UART\n", sizeof(frame_format_t));
    printf("you have 5s to send me some input data\n");
    sleep(5);

    // for(int i = 0; i < len; i++)
    //     printf("0x%02X ", text[i]);

    // buffer = (char*) malloc (bufsize * sizeof(char));

    // if(buffer == NULL)
    // {
    //     perror("unable to allocate buffer");
    //     exit(1);
    // }

    // characters = getline(&buffer, &bufsize, stdin);
    // len = write(fd, &buffer ,characters);
    // printf("wrote %d bytes over UART\n", len);

    /*Read from serial port*/
    memset(rtext, 0, 255);
    rlen = read(fd, rtext, 255);
    printf("Received %d bytes\n", rlen);
    printf("Received string: %s\n", rtext);

    /** verify the bin file   
    i=0;
    while(!feof(ptr))
    {
        if(!(i%16))
            printf("[%X] =",i);
        if(fread(&k,sizeof(char),1,ptr)) //return number of k read 
            printf(" 0x%02X",k);
        i++;
        if(!(i%16))
            printf("\r\n");
    }*/

    close(fd); /* Close the serial port */
    fclose(ptr); /* Close file*/
    return 0;
}


/*
    sleep(5);
    int readin, readinTot = 0;						// Bytes read from Rx port
	char buffer[140] = {0};								// read buffer
	char* bufptr;									// buffer ponter
	// Read data from Rx port
	bufptr = buffer; 
    while((readin = read(fd, bufptr, buffer + sizeof(buffer) - bufptr - 1)) > 0) {
		bufptr += readin;
		readinTot += readin;
		if (bufptr[-1] == '\n' || bufptr[-1] == '\r') break;
	} 
	*bufptr = '\0';		// Null terminate the string so we can printf() it

    printf("Received %d bytes\n", readinTot);
    printf("Received string: %s\n", buffer);
*/