###  Linux CLI bin file Sender

- The serial communucation operates at blocking mode, baudrate: 115200 not yet selectable

- the parameter has to be the path of a bin file taht you want to send over serial comport

- Currently the program only takes /dev/ttyUSB0

- not yet able to send address to the target

- read(file_descriptor, text, sizeof(frame_format_t)); only takes 64bytes

Header Format - Transmit
-------------------------------------------------------------------------------------------
|-SOF-|-BL_HEADER-|-Len-|-Address-|-Size-|-Version-|-FirmwareChecksum-|-FrameChecksum-|-EOF-|
---1-------1---------2-------4-------4--------3-------------4-----------------1----------1--
------------------------------------------------------------------------------------------

 