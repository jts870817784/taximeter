#ifndef _DS1307_H_
#define _DS1307_H_

#include "sys.h"

//ʱ��ṹ��
typedef struct 
{
	vu8 sec;
	vu8 min;
	vu8 hour;			
	//������������
  vu8  week;	
  vu8  w_date;
	vu8  w_month;
	vu8 w_year;		 
}_calendar_obj;				

extern _calendar_obj calendar;	//�����ṹ��
extern u8 Set_Rtc_Code[7];

void DS1307_WriteReg(u8 addr,u8 data);
u8 DS1307_ReadReg(u8 addr);
void DS1307_ReadRtc(u8 *R_Data);
void DS1307_SetRtc(u8 *p);


#endif

