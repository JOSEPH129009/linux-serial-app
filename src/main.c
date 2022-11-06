
/*====================================================================================================*/
    /* Serial Port Programming in C                                                                       */
/*----------------------------------------------------------------------------------------------------*/
    /* Program opens the serial port and closes it                                                        */
/*----------------------------------------------------------------------------------------------------*/
/* Compiler/IDE  : gcc 11.2.0                                                                        */
/* Library       :                                                                                    */
/* Commands      : make                                                           */
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
#include "bit_field.h"
#define TOTAL_PROGRAM_ARGUMENT         3
#define BINFILE                 argc - 2
#define FIRMWARE_VERSION        argc - 1


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
    TRANSMIT_HEADER,
    TRANSMIT_DATA,
    TRANSMIT_DONE,
    RECEIVE,
    PRINT,
    FINISH
};
uint32_t state = 0;
#define UpdateDone         bit0
uint32_t bootloader_flag = 0;
uint32_t step_counter = 0;

int main(int argc, char** argv)
{
    clock_t time;
    //-----------file
    FILE *ptr;
    int i;
    const char* filename;
    //----------serialport
    int file_descriptor; /*File Descriptor*/
    long binary_size;
    uint32_t firmware_checksum = 0;
    int len;
    unsigned char text[300], k;

    if(argc != TOTAL_PROGRAM_ARGUMENT) /*bin file and firmware version for update*/
    {
        perror("wrong argument");
        exit(1);
    }

    filename = get_filename_ext(argv[BINFILE]);
    if(strcmp(filename, "bin"))
    {
        perror("argument is not a bin file");
        exit(1);
    }
    
    ptr = fopen(argv[BINFILE],"rb");  // r for read, b for binary
    if(ptr==NULL)
    {
        perror("Error Reporting");
        exit(1);
    }
    fseek (ptr, 0 , SEEK_END);
    binary_size = ftell (ptr) ;
    rewind (ptr);
    if(binary_size > 0xFFFFFFFFL)
        perror("Memory Size Too Big");
    // printf("bin file size : %ld\r\n", binary_size);

    while(!feof(ptr))
    {
        if(fread(&k,sizeof(char),1,ptr)) //fread function return number of k read 
            firmware_checksum += k;
    }
    rewind (ptr);
    serial_port_init(&file_descriptor);
    // openPort(PORT);  
    // configPort(file_descriptor);    

/***************************************************************/
    //----------read usr input
    // char* buffer;
    // size_t bufsize = 32;
    // size_t characters;
    //----------
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
    //     write(file_descriptor, &buffer[i] ,1);
    //     time = clock();
    //     printf("wrote %d bytes over UART\n", len);

    //     /*10000 = 10milli seconds brecase CLOCKS_PER_SEC == ((__clock_t) 1000000)*/
    //     while(!Time_Interval(time, 10000))  /*stm32 uart cannot catch up if stm32 is doing echoing*/
    //         ;
    // }
/*****************************************************************/
    int total_rec = 0;
    int read_ret_val = 0;
    char temp[sizeof(frame_format_t)];
    int percentage = 0;
    uint8_t version[3] = {0x01,0x00,0x00}; //v1.0.0
    i=0;
    int total_update_counter = 0;
    while(1)
    {
        switch (state)
        {
        case TRANSMIT_HEADER:
            header_update_info_prepare((uint32_t)binary_size, 0x08020000U, &version[0],firmware_checksum);
            if(serial_transmit(&file_descriptor, &transmit_frame) < 0)
                perror("Error transmitting");
            printf("programming: \r\n");
            state = RECEIVE;
            break;        
        case TRANSMIT_DATA:
            if(!feof(ptr))
            {
                if(fread(&k,sizeof(char),1,ptr)) //return number of k read 
                    transmit_frame.payload[i] = k;
                i++;
                if(!(i%256))
                {
                    update_payload_prepare(256);
                    if(serial_transmit(&file_descriptor, &transmit_frame) < 0)
                        perror("Error transmitting");
                    state = RECEIVE;
                }
            }
            else
            {
                i--;
                update_payload_prepare(i);
                if(serial_transmit(&file_descriptor, &transmit_frame) < 0)
                    perror("Error transmitting");
                state = RECEIVE;
            }
            percentage = (float)total_update_counter/binary_size*100.0;
            printf("%d %\r", percentage);
            break;
        case TRANSMIT_DONE:
            update_done_prepare();
            if(serial_transmit(&file_descriptor, &transmit_frame) < 0)
                perror("Error transmitting");
            bootloader_flag = bit_on(bootloader_flag,UpdateDone);
            state = RECEIVE;
            break;   
        case RECEIVE:
            /* code */
            total_update_counter += i;
            i=0;
            memset(text, 0, 300);
            // while( total_rec < sizeof(frame_format_t) )
            // {
            //     read_ret_val = read(file_descriptor, temp, sizeof(frame_format_t));
            //     if (read_ret_val != -1)
            //     {
            //         if ( (total_rec + read_ret_val) >= sizeof(frame_format_t))
            //         { 
            //             read_ret_val = sizeof(frame_format_t) - total_rec;
            //         }
            //         memcpy(&text[total_rec], temp, read_ret_val);
            //         total_rec += read_ret_val;
            //     }
            //     else
            //     {
            //         perror("error reading serial line: ");
            //     }
            // }
            len = read(file_descriptor, text, sizeof(frame_format_t));
            total_rec+=len;
            state = PRINT;
            break;
        case PRINT:
            // printf("Received %d bytes\n", len);
            // for(i=0 ; i<300; i++)
            //     printf("0x%X ", text[i]);
            // printf("total counter : %d\r\n",total_update_counter);
            // printf("\r\n");

            if(parse_frame(text) < 0)
                return 0;
            state = TRANSMIT_DATA;
            if(bootloader_flag & UpdateDone)
            {
                state = FINISH;
                printf("\r\n");
                printf("Firmware update done\r\n");
            }
            else if(total_update_counter == binary_size)
            {
                state = TRANSMIT_DONE;
            }
            break;
        case FINISH:
            close(file_descriptor); /* Close the serial port */
            fclose(ptr); /* Close file*/
            return 0;
        default:
            break;
        }
    }

}
