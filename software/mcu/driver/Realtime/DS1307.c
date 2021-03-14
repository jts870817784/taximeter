#include "DS1307.h"
#include "myiic.h"

_calendar_obj calendar;	//日历结构体

static u8 HEX2BCD(u8 data)
{
  return (data / 10 * 16) + (data % 10);
}

static u8 BCD2HEX(u8 data)
{
  return (data / 16 * 10) + data % 16;
}

/*
  ***********************DS1307调试********************
  ****DS1307实时时钟芯片地址0xD0,读0xD1  写0xD0********
*/ 

u8 Set_Rtc_Code[7] = {0x1E,0x2A,0x16,0x03,0x18,0x02,0x00};//秒分时、星期、日月年初始化
u8 Rtc_Addr[7] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06};

void DS1307_WriteReg(u8 addr,u8 data)
{
  u8 temp;
  
  temp = HEX2BCD(data);
  
  IIC_Start();
  
  IIC_Send_Byte(0xD0);
  IIC_Wait_Ack();
  
  IIC_Send_Byte(addr);
  IIC_Wait_Ack();
 
  IIC_Send_Byte(temp);
  IIC_Wait_Ack();
  
  IIC_Stop();
}

u8 DS1307_ReadReg(u8 addr)
{
  u8 temp;
  u8 data;
  
  IIC_Start();
  
  IIC_Send_Byte(0xD0);
  IIC_Wait_Ack();
  
  IIC_Send_Byte(addr);
  IIC_Wait_Ack();

  IIC_Start();
  
  IIC_Send_Byte(0xD1);
  IIC_Wait_Ack();
  
  data = IIC_Read_Byte(0);
  IIC_Stop();
  
  temp = BCD2HEX(data);

  return temp;
}

void DS1307_ReadRtc(u8 *R_Data)
{
  u8 *p;
  p = Rtc_Addr;
  for(u8 i = 0;i < 7;i++)
  {
    *R_Data = DS1307_ReadReg(*p);
    R_Data++;
    p++;
  }
}

void DS1307_SetRtc(u8 *p)
{
  u8 temp;
  u8 data;
  IIC_Start();
  IIC_Send_Byte(0xD0);
  IIC_Wait_Ack();
  
  IIC_Send_Byte(0x00);
  IIC_Wait_Ack();
  
  for(u8 i = 0; i < 7;i++)
  {
    data = *p;
    temp = HEX2BCD(data);
    IIC_Send_Byte(temp);
    IIC_Wait_Ack();
    p++;
  }
  IIC_Stop();
}


