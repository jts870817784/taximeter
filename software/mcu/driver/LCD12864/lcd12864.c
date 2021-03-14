#include "lcd12864.H"
#include "delay.h"

/*! 
*  @brief      GPIO_init
 *  @since      v1.0
 *  @param  None
 *  @author     ZС��
 *  ʹ��ʱ�����޸�����ĳ�ʼ������
 */
void lcd_GPIO_init()
{      
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//ʹ��GPIOEʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  SID=1;
  SCLK=1;
}

/* �ַ���ʾRAM��ַ    4��8�� */
u8 LCD_addr[4][8]={
	{0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87},  		//��һ��
	{0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97},		//�ڶ���
	{0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F},		//������
	{0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F}		//������
	};






/*! 
*  @brief      LCD���з���һ���ֽ�
 *  @since      v1.0
 *  @param  byte   д���ֽ�
 *  @author     ZС��
 */
void SendByte(u8 byte)
{
     u8 i; 
	  for(i = 0;i < 8;i++)
    {
			if((byte << i) & 0x80)  //0x80(1000 0000)  ֻ�ᱣ�����λ
			{
				SID = 1;           // ��������ߵ�ƽ��������1
			}
			else
			{
				SID = 0;         // ��������͵�ƽ��������0
			}
		/*��		
			SID =	(Dbyte << i) & 0x80;
				
			��������Ϊ�˷������
		*/
			SCLK = 0;   //ʱ�����õ�  ����SID�仯
			delay_us(5); //��ʱʹ����д��
			SCLK = 1;    //����ʱ�ӣ��ôӻ���SID
		}   
}


/*! 
 *  @brief      LCDдָ��
 *  @since      v1.0
 *  @param  Cmd   Ҫд���ָ��
 *  @author     ZС��
 */
void Lcd_WriteCmd(u8 Cmd )
{
     delay_ms(1);     //��������û��дLCD��æ�ļ�⣬����ֱ����ʱ1ms��ʹÿ��д�����ݻ�ָ��������1ms ��ɲ���дæ״̬���
     SendByte(WRITE_CMD);            //11111,RW(0),RS(0),0   
     SendByte(0xf0&Cmd);      //����λ
     SendByte(Cmd<<4);   //����λ(��ִ��<<)
}

/*! 
 *  @brief      LCDд����
 *  @since      v1.0
 *  @param  Dat   Ҫд�������
 *  @author     ZС��
 */
void Lcd_WriteData(u8 Dat )
{
     delay_ms(1);     
     SendByte(WRITE_DAT);            //11111,RW(0),RS(1),0
     SendByte(0xf0&Dat);      //����λ
     SendByte(Dat<<4);   //����λ(��ִ��<<)
}
/*! 
 *  @brief      LCD��ʼ��
 *  @since      v1.0
 *  @param  None
 *  @author     ZС��
 */
void Lcd_Init(void)
{ 
delay_ms(50);   	//�ȴ�Һ���Լ죨��ʱ>40ms��
	Lcd_WriteCmd(0x30);        //�����趨:ѡ�����ָ�
delay_ms(1);//��ʱ>100us
	Lcd_WriteCmd(0x30);        //�����趨:ѡ��8bit������
delay_ms(1);	//��ʱ>37us
    Lcd_WriteCmd(0x0c);        //����ʾ
delay_ms(1);	//��ʱ>100us
    Lcd_WriteCmd(0x01);        //�����ʾ�������趨��ַָ��Ϊ00H
delay_ms(30);	//��ʱ>10ms
		Lcd_WriteCmd(0x06);        //�����趨�㣬��ʼ�����
}


/*! 
 *  @brief      ��ʾ�ַ�����
 *  @since      v1.0
 *  @param  x: row(0~3)
 *  @param  y: line(0~7) 
 *  @param 	str: Ҫ��ʾ���ַ�����
 *  @author     ZС��
 */
void LCD_Display_Words(uint8_t x,uint8_t y,uint8_t*str)
{ 
	Lcd_WriteCmd(LCD_addr[x][y]); //д��ʼ���λ��
	while(*str>0)
    { 
      Lcd_WriteData(*str);    //д����
      str++;     
    }
	delay_ms(2);	
}
/*! 
 *  @brief      ��ʾͼƬ
 *  @since      v1.0
 *  @param  *pic   ͼƬ��ַ
 *  @author     
 */
void LCD_Display_Picture(uint8_t *img)
	{
		uint8_t x,y,i;
		Lcd_WriteCmd(0x34);		//�л�������ָ��
		Lcd_WriteCmd(0x34);		//�ر�ͼ����ʾ
		for(i = 0; i < 1; i++)   //������д��
		{
			for(y=0;y<4;y++)   //��ֱYд32��
			{  
				for(x=0;x<16;x++)   //����Xд8��
				{
					Lcd_WriteCmd(0x80 + y);		//�е�ַ
					Lcd_WriteCmd(0x80 + x+i);		//�е�ַ
					Lcd_WriteData(*img ++);		//д��λ�ֽ����� D15��D8   
					Lcd_WriteData(*img ++);		//д��λ�ֽ����� D7��D0
				}
			}
		}
		Lcd_WriteCmd(0x36);//��ͼ����ʾ		
		Lcd_WriteCmd(0x30);        //�л��ػ���ָ��
	}	
/*! 
 *  @brief      ��������
 *  @since      v1.0
 *  @param  None
 *  @author     ZС��
 */
void LCD_Clear(void)
	{
		Lcd_WriteCmd(0x01);			//����ָ��
		delay_ms(2);				//��ʱ�Դ�Һ���ȶ�������1.6ms��
	}
	
