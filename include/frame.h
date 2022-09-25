#ifndef FRAME_H_
#define FRAME_H_


#include "stdint.h"


//delimeters
#define BL_START_OF_FRAME  0x45444459 //EDDY 
#define BL_END_OF_FRAME    0x46414952 //FAIR 

//frame IDS
#define BL_HEADER       0xFEEDEDDE //sends info about firmware: size , version 
#define BL_STATUS_CHECK 0x4b4b4b4b
#define BL_START_UPDATE 0xBA5EBA11
#define BL_PAYLOAD      0xDEADBEEF
#define BL_UPDATE_DONE  0xDEADDADE
#define BL_ACK_FRAME    0x45634AED
#define BL_NACK_FRAME   0x43636AEA
#define PAYLOAD_LEN 256

//frame formated struct
typedef struct __attribute__((packed))
{
    uint32_t start_of_frame;
    uint32_t frame_id;
    uint16_t payload_len;
    uint8_t payload[PAYLOAD_LEN];
    uint32_t crc32;
    uint32_t end_of_frame;
}frame_format_t;

typedef enum bootloader_state
{
    STATE_IDLE = 0,
    STATE_START_UPDATE,
    STATE_UPDATING
}bootloader_state;

extern frame_format_t transmit_frame;
void bootloader_init(void);

#endif  /*FRANE_H_*/