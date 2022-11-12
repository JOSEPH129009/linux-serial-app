# Linux CLI bin file Sender App

- The serial communucation operates at blocking mode, baudrate: 115200 not yet selectable

- the first argument has to be the path of a bin file that you want to send over serial comport and the second argument has to be version.
    - e.g. command: ./build/serial.elf /home/josephjoe1209/Documents/user_app/build/user_app.bin --version=1.0.2
- Currently the program only takes /dev/ttyUSB0

- progarm only sends 0x08020000 to MCU for update

# Serial Frame

### @brief Frame Format:

    |-SOF-|-Frame_ID-|-Len-|-Data-|-FrameChecksum-|-EOF-|
    |--1--|----1-----|--2--|---n--|------1--------|--1--|

- #define BL_START_OF_FRAME  0x55 
- #define BL_END_OF_FRAME    0xFF  
- FrameChecksum: 2"s complement of all Data added together 

### @brief HEADER Format:

    |-SOF-|-BL_HEADER-|-Len-|-Address-|-Size-|-Version-|-FirmwareChecksum-|-FrameChecksum-|-EOF-|
    |- 1 -|----1------|--2--|---4-----|--4---|---3-----|--------4---------|------4--------|--1--|

- #define BL_HEADER       0x01 //sends info about firmware: size , version 
- sent by desktop app


### @brief STATUS CHECK Format:
- #define BL_STATUS_CHECK 0x02 //not used yet

### @brief START UPDATE Format :
- #define BL_START_UPDATE 0x03 //not used yet

### @brief PAYLOAD Format:

    |-SOF-|-Frame_ID-|-Len-|-Data-|-FrameChecksum-|-EOF-|
    |--1--|----1-----|--2--|---n--|------1--------|--1--|

- #define BL_PAYLOAD      0x04
- sent by desktop app

### @brief UPDATE_DONE Format:

    |-SOF-|-BL_UPDATE_DONE-|-Len-|-FrameChecksum-|-EOF-|
    |--1--|--------1-------|--2--|-------1-------|--1--|

- #define BL_UPDATE_DONE  0x05
- sent by desktop app.the legnth is 0.

### @brief ACK Format:

    |-SOF-|-BL_ACK_FRAME-|-Len-|-FrameChecksum-|-EOF-|
    |--1--|------1-------|--2--|--------1------|--1--|

- #define BL_ACK_FRAME    0x06
- sent by uC. There is no data for this frame so the length is always 0.


### @brief NACK Format:

    |-SOF-|-BL_NACK_FRAME-|-Len-|-ErrorFlag-|-FrameChecksum-|-EOF-|
    |--1--|-------1-------|--2--|----4------|-------1-------|--1--|

- #define BL_NACK_FRAME   0x07
- sent by uC.the legnth is 4, 4byte Error Flag MSB first

# Link

[**user app**](https://github.com/JOSEPH129009/user-app) \
[**stmh743zi bootloader app**](https://github.com/JOSEPH129009/stm32h7_bootload_app) 
 