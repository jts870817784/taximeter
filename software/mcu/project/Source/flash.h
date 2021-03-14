#ifndef _FLASH_H_
#define _FLASH_H_


#include "config.h"

#define FLASH_BASE_ADDRESS (0x800FC00)
#define FLASH_VERIFY 0xAAAA

#define FLASH_MAX_BYTE 0x40

#define FLASH_START FLASH_BASE_ADDRESS
#define FLASH_END   (FLASH_START+FLASH_MAX_BYTE) 

#define FLASH_SIZE 64         //所选MCU的FLASH容量大小(单位为K)

#if FLASH_SIZE<256
  #define SECTOR_SIZE           1024    //字节
#else 
  #define SECTOR_SIZE           2048    //字节
#endif

//#define PID_IN_KP_ADDR  ((FLASH_BASE_ADDRESS)+0X04)
//#define PID_IN_KD_ADDR  ((FLASH_BASE_ADDRESS)+0X08)
//#define PID_IN_KI_ADDR  ((FLASH_BASE_ADDRESS)+0X0C)

//#define PID_Out_KP_ADDR ((FLASH_BASE_ADDRESS)+0X10)
//#define PID_Out_KD_ADDR ((FLASH_BASE_ADDRESS)+0X14)
//#define PID_Out_KI_ADDR ((FLASH_BASE_ADDRESS)+0X18)


void FlashSavePid(void);
void FlashReadPid(void);
void VerifyFlash(void);



//存储结构
typedef struct
{
	u16 flash_initial_flag;
	u16 in_max_out;
	float in_kp;
	float in_kd;
	float in_ki;
	
	u16 out_max_out;
	u16 z_max_out;
	
	float out_kp;
	float out_kd;
	float out_ki;
	
	u16 in_max_ki;
	u16 out_max_ki;
	
	float z_kp;
	float z_kd;
	float z_ki;
	
	u16 z_max_ki;
	u16 reserve1;
	
	float in_apart;
	float out_apart;
	float z_apart;
	
}flash_pid_structure;




#endif
