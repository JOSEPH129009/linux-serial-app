
/*====================================================================================================*/
    /* Serial Port Programming in C                                                                       */
/*----------------------------------------------------------------------------------------------------*/
    /* Program opens the serial port and closes it                                                        */
/*----------------------------------------------------------------------------------------------------*/
/* Compiler/IDE  : gcc 11.2.0                                                                        */
/* Library       :                                                                                    */
/* Commands      : gcc -o serial serial.c                                                             */
/* OS            : Linux(x86) (Ubuntu)(Linux Kernel 5.15.0-43-generic)                                            */                              
/* Programmer    : Joseph.H                                                                           */
/* Date	         : 2022                                                                               */
/*====================================================================================================*/

/*====================================================================================================*/
/* Sellecting the Serial port Number on Linux                                                         */
/* ---------------------------------------------------------------------------------------------------*/ 
/* /dev/ttyUSBx - when using USB to Serial Converter, where x can be 0,1,2...etc                      */
/* /dev/ttySx   - for PC hardware based Serial ports, where x can be 0,1,2...etc                      */
    /*====================================================================================================*/

#include <stdio.h>
#include <unistd.h>  /* UNIX Standard Definitions 	   */ 
#include <errno.h>   /* ERROR Number Definitions           */
#include "string.h"
#include "stdlib.h"
#include "time.h"
#include "stdbool.h"
#include "frame.h"
#include "RS232comm.h"

const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1; 
}


bool Time_Interval(clock_t time, clock_t gap)
{
    // _Static_assert(sizeof(int) == 4, "wrong" );
    clock_t current_time = clock();
    if(time <= current_time)
    {
        if((current_time - time) >= gap)
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

enum{
    TRANSMIT,
    RECEIVE,
    SECOND_READ,
    FINISH
};
uint32_t state = 0;


int main(int argc, char** argv)
{
    clock_t time;
    //-----------file
    FILE *ptr;
    int i,k;
    const char* filename;
    //----------serialport
    int fd; /*File Descriptor*/
    long binary_size;
    int len;
    unsigned char text[300];
    //----------read usr input
    char* buffer;
    size_t bufsize = 32;
    size_t characters;
    //----------

    if(argc != 2) /*bin file and firmware version for update*/
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
    fseek (ptr, 0 , SEEK_END);
    binary_size = ftell (ptr) ;
    rewind (ptr);

    serial_port_init(&fd);
    // openPort(PORT);  
    // configPort(fd);    


/***************************************************************/
    // buffer = (char*) malloc (bufsize * sizeof(char));

    // if(buffer == NULL)
    // {
    //     perror("unable to allocate buffer");
    //     exit(1);
    // }

    // characters = getline(&buffer, &bufsize, stdin);
    // len = strlen(buffer);
    // for(int i = 0; i < len; i++)
    // {
    //     write(fd, &buffer[i] ,1);
    //     time = clock();
    //     printf("wrote %d bytes over UART\n", len);

    //     /*10000 = 10milli seconds brecase CLOCKS_PER_SEC == ((__clock_t) 1000000)*/
    //     while(!Time_Interval(time, 10000))  /*stm32 uart cannot catch up if stm32 is doing echoing*/
    //         ;
    // }
/*****************************************************************/
    bootloader_init();
    // while(1)
    // {
    //     switch (state)
    //     {
    //     case TRANSMIT:
    //         len = sizeof(frame_format_t);
    //         buffer = (char *)&transmit_frame;
    //         for(int i = 0; i < len; i++)
    //         {
    //             if(write(fd, &buffer[i] ,1) < 0)
    //                 perror("Error transmitting");
    //         }
    //         state = RECEIVE;
    //         break;
    //     case RECEIVE:
    //         /* code */
    //         memset(text, 0, 300);
    //         len = read(fd, text, sizeof(frame_format_t));
    //         state = PRINTF;
    //         break;
    //     case PRINTF:
    //         printf("Received %d bytes\n", len);
    //         for(i=0 ; i<300; i++)
    //             printf("0x%X ", text[i]);
    //         // printf("Received string: %s\n", text);
    //         close(fd); /* Close the serial port */
    //         fclose(ptr); /* Close file*/
    //         return 0;
    //         // break;
    //     default:
    //         break;
    //     }
    // }
    int total_rec = 0;
    int read_ret_val = 0;
    char temp[sizeof(frame_format_t)];
    uint8_t read_counter = 0;
    while(1)
    {
        switch (state)
        {
        case TRANSMIT:
            len = sizeof(frame_format_t);
            buffer = (char *)&transmit_frame;
            for(int i = 0; i < len; i++)
            {
                if(write(fd, &buffer[i] ,1) < 0)
                    perror("Error transmitting");
            }
            state = RECEIVE;
            break;
        case RECEIVE:
            /* code */
            memset(text, 0, 300);
            while( total_rec < sizeof(frame_format_t) )
            {
                read_ret_val = read(fd, temp, sizeof(frame_format_t));
                if (read_ret_val != -1)
                {
                    if ( (total_rec + read_ret_val) >= sizeof(frame_format_t))
                    { 
                        read_ret_val = sizeof(frame_format_t) - total_rec;
                    }
                    memcpy(&text[total_rec], temp, read_ret_val);
                    total_rec += read_ret_val;
                }
                else
                {
                    perror("error reading serial line: ");
                }
            }
            // len = read(fd, text, sizeof(frame_format_t));
            state = SECOND_READ;
            break;
        case SECOND_READ:
            printf("Received %d bytes\n", len);
            for(i=0 ; i<300; i++)
                printf("0x%X ", text[i]);
            // printf("Received string: %s\n", text);
            total_rec = 0;
            if(read_counter == 0)
            {
                state = TRANSMIT;
                read_counter++;
            }else
                state = FINISH;
            break;
        case FINISH:
            close(fd); /* Close the serial port */
            fclose(ptr); /* Close file*/
            return 0;
        default:
            break;
        }
    }

    /*Write to serial port*/
    // bootloader_init();
    // strcpy(text, "Hello from_PC\r\n");
    // len = strlen(text);
    // for(int i; i < len; i++)
    // {
    //     write(fd, &text[i] ,1);
    //     time = clock();
    //     printf("wrote %d bytes over UART\n", len);
    //     while(!Time_Interval(time, 5000))
    //         ;
    // }
    // len = write(fd, text ,len);
    // printf("wrote %d bytes over UART\n", len);
    // len = write(fd, &transmit_frame ,sizeof(frame_format_t));
    // printf("wrote %ld bytes over UART\n", sizeof(frame_format_t));
    // printf("you have 5s to send me some input data\n");
    // sleep(5);

    // for(int i = 0; i < len; i++)
    //     printf("0x%02X ", text[i]);

    /*Read from serial port*/
    memset(text, 0, 255);
    len = read(fd, text, 255);
    printf("Received %d bytes\n", len);
    printf("Received string: %s\n", text);

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