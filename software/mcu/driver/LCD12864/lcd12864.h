#ifndef __lcd12864_H_
#define __lcd12864_H_


#include "sys.h"

#define WRITE_CMD	0xF8//д����  
#define WRITE_DAT	0xFA//д����

//�ӿ�(SID: PE1  SCLK: PE0) 
#define SID PBout(14)
#define SCLK PBout(15)


void lcd_GPIO_init(void);
void Lcd_Init(void);

void SendByte(u8 Dbyte);
void LCD_Clear(void);
void LCD_Display_Words(uint8_t x,uint8_t y,uint8_t*str);
void LCD_Display_Picture(uint8_t *img);

void Lcd_WriteData(u8 Dat );
void Lcd_WriteCmd(u8 Cmd );

#endif
