#ifndef FRAME_H_
#define FRAME_H_


#include "stdint.h"

/**
 * @brief Frame Format
 *          -----------------------------------------------
 *          |-SOF-|-Frame_ID-|-Len-|-Data-|-Checksum-|-EOF-|
 *  Bytes:  ---1-------1--------2-----n------1----------1--
 * 
 * STM32H7 n = 32 (8 word, 256bits) 
 */

#define DATA_LEN   2
#define DATA_BEGIN 4

//delimeters
#define BL_START_OF_FRAME  0x55 
#define BL_END_OF_FRAME    0xFF  
/**
 * @brief Header Format
 *          -------------------------------------------------------------------------------------------
 *          |-SOF-|-Frame_ID-|-Len-|-Address-|-Size-|-Version-|-FirmwareChecksum-|-FrameChecksum-|-EOF-|
 *  Bytes:  ---1-------1--------2-------4-------4--------3-------------4-----------------1----------1--
 * 
 */

//frame IDS
#define BL_HEADER       0x01 //sends info about firmware: size , version 
#define BL_STATUS_CHECK 0x02
#define BL_START_UPDATE 0x03
#define BL_PAYLOAD      0x04
#define BL_UPDATE_DONE  0x05
#define BL_ACK_FRAME    0x06
#define BL_NACK_FRAME   0x07
#define PAYLOAD_LEN 256

#define PAYLOAD_LEN 256

//frame formated struct
typedef struct __attribute__((packed))
{
    uint8_t start_of_frame;
    uint8_t frame_id;
    uint16_t payload_len; //LSB first
    uint8_t payload[PAYLOAD_LEN];
    uint8_t checksum; // 2's complement
    uint8_t end_of_frame;
}frame_format_t;

typedef enum bootloader_state
{
    STATE_IDLE = 0,
    STATE_START_UPDATE,
    STATE_UPDATING
}bootloader_state;

extern frame_format_t transmit_frame;
int serial_transmit(int* fd, frame_format_t* t_frame);
void update_payload_prepare(uint32_t len);
void update_done_prepare(void);
void header_update_info_prepare(uint32_t size, uint32_t address, 
                                uint8_t* version, uint32_t firmware_checksun);
int parse_frame(const uint8_t* data);

#endif  /*FRANE_H_*/