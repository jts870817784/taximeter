#include "stm32f10x.h"  
#include "led.h"
#include "key.h"
#include "bsp_usart.h"
#include "mpu6050.h"
#include "bsp_i2c.h"
#include "delay.h"
#include "myiic.h"
#include "24cxx.h" 
#include "DS1307.h"
#include "lcd12864.H"
#include "24cxx.h"
#include "bsp_TiMbase.h" 

extern volatile uint16_t time;

void fram();

int main(void)
{	
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);  //ѡ����1
	led_gpio_config();
	EXTI_Key_Config();	//�����ж�
	USART_Config();     //��USART2����
	i2c_GPIO_Config();  //MPU6050��IIC
	MPU6050_Init();     //��ʼ��MPU6050
	IIC_Init();			    //Tiny_RTC��IIC��ʼ�� 
	lcd_GPIO_init();
	Lcd_Init();
	BASIC_TIM_Init();   //������ʱ��Time2

    fram();
  	//Usart2����OK
//	printf("\r\n ��ӭʹ��Ұ��  STM32 �����塣\r\n");		 
//	printf("\r\n ����һ��MPU6050�������� \r\n");

/*
  	//MPU6050����OK
	short Accel[3];
	short Gyro[3];
	float Temp;
	if (MPU6050ReadID() == 1)
	{	
		MPU6050ReadAcc(Accel);			
		printf("\r\n���ٶȣ� %8d%8d%8d    ",Accel[0],Accel[1],Accel[2]);
		MPU6050ReadGyro(Gyro);
		printf("�����ǣ� %8d%8d%8d    ",Gyro[0],Gyro[1],Gyro[2]);
		MPU6050_ReturnTemp(&Temp); 
		printf("�¶ȣ� %8.2f",Temp);
	}
*/
	
	  //Tiny_RTC����OK
//	while(1)
//	{
//		delay_ms(999);//����һ�����һ������
//    DS1307_ReadRtc((u8*)&calendar);
//    printf("%d��-%d��-%d��",2021+calendar.w_year,calendar.w_month,calendar.w_date);
//		printf("  %dʱ-%d��-%d��\r\n",calendar.hour,calendar.min,calendar.sec);
//	}



		//LCD_12864B����OK
//		LCD_Display_Words(0,0,"I love you."); //���㻭��
//		while(1)
//		{
//			;
//		}

	
	
		//��ʱ������OK
//		while(1)
//		{
//			if(time==1000)//��ʱһ���ѵ�
//			{
//				led_1=!led_1;
//				time=0;	
//			}
//		}

		//Bluetooth
		

}
