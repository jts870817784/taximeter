#ifndef _24CXX_H_
#define _24CXX_H_

#include "config.h"
#include "ds1307.h"

typedef struct tagOrderPacket {
    u16 header;
    u8 typeLength;
    u16 orderNumber;
    _calendar_obj startTime;
    _calendar_obj endTime;
    long mile;
    u16 money;
    u16 dif;
    u16 end;
} orderPacket;

#define ORDER_PACKET_SIZE ((sizeof(orderPacket) & 0x01) == 0 ? sizeof(orderPacket) :\
    (sizeof(orderPacket) + 1))
#define ORDER_MAX_DATA_NUM 100


#define FLASH_BASE_ADDRESS (0x800FC00)
#define FLASH_VERIFY 0xAAAA


#define FLASH_MAX_BYTE (ORDER_MAX_DATA_NUM * ORDER_PACKET_SIZE)

#define FLASH_START FLASH_BASE_ADDRESS
#define FLASH_END   (FLASH_START+FLASH_MAX_BYTE) 

#define FLASH_SIZE 128

#if FLASH_SIZE<256
  #define SECTOR_SIZE           1024    //×Ö½Ú
#else 
  #define SECTOR_SIZE           2048    //×Ö½Ú
#endif

void FLASH_WriteMoreData(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite);
void FLASH_ReadMoreData(uint32_t startAddress,uint16_t *readData,uint16_t countToRead);
uint8_t getFlashInitStatus  (void);
void bspflashInit(void);



#define flashWrite(ptrWriteData) FLASH_WriteMoreData(FLASH_START, (u16 *)(ptrWriteData), sizeof(orderPacket) * 20)
#define flashRead(ptrWriteData) FLASH_ReadMoreData(FLASH_START, (u16 *)(ptrWriteData), sizeof(orderPacket) * 20)


#endif

