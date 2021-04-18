#ifndef _24CXX_H_
#define _24CXX_H_

#include "config.h"
#include "ds1307.h"
#include "stdio.h"

struct tagOrderPacket {
    u16 header;
    u8 typeLength;
    u16 orderNumber;
    _calendar_obj startTime;
    _calendar_obj endTime;
    long mile;
    u16 money;
    u16 dif;
    u16 end;
};
typedef struct tagOrderPacket orderPacket;

typedef __packed struct tagTxPacket {
    unsigned short header;
    unsigned char typeLen;
    unsigned short orderNum;
    unsigned char s_sec;
    unsigned char s_min;
    unsigned char s_hour;
    unsigned char s_day;
    unsigned char s_week;
    unsigned char s_month;
    unsigned char s_year;
    unsigned char e_sec;
    unsigned char e_min;
    unsigned char e_hour;
    unsigned char e_day;
    unsigned char e_week;
    unsigned char e_month;
    unsigned char e_year;
    long mile;
    unsigned short money;
    unsigned short dif;
    unsigned short end;

} txPacket;

#define HEADER_CODE 0X55AA
#define END_CODE    0XAA55


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

