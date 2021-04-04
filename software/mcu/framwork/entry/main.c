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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);  //选择组1
	led_gpio_config();
	EXTI_Key_Config();	//按键中断
	USART_Config();     //打开USART2串口
	i2c_GPIO_Config();  //MPU6050的IIC
	MPU6050_Init();     //初始化MPU6050
	IIC_Init();			    //Tiny_RTC的IIC初始化 
	lcd_GPIO_init();
	Lcd_Init();
	BASIC_TIM_Init();   //基本定时器Time2

    fram();
  	//Usart2测试OK
//	printf("\r\n 欢迎使用野火  STM32 开发板。\r\n");		 
//	printf("\r\n 这是一个MPU6050测试例程 \r\n");

/*
  	//MPU6050测试OK
	short Accel[3];
	short Gyro[3];
	float Temp;
	if (MPU6050ReadID() == 1)
	{	
		MPU6050ReadAcc(Accel);			
		printf("\r\n加速度： %8d%8d%8d    ",Accel[0],Accel[1],Accel[2]);
		MPU6050ReadGyro(Gyro);
		printf("陀螺仪： %8d%8d%8d    ",Gyro[0],Gyro[1],Gyro[2]);
		MPU6050_ReturnTemp(&Temp); 
		printf("温度： %8.2f",Temp);
	}
*/
	
	  //Tiny_RTC测试OK
//	while(1)
//	{
//		delay_ms(999);//即隔一秒更新一次数据
//    DS1307_ReadRtc((u8*)&calendar);
//    printf("%d年-%d月-%d日",2021+calendar.w_year,calendar.w_month,calendar.w_date);
//		printf("  %d时-%d分-%d秒\r\n",calendar.hour,calendar.min,calendar.sec);
//	}



		//LCD_12864B测试OK
//		LCD_Display_Words(0,0,"I love you."); //画点画线
//		while(1)
//		{
//			;
//		}

	
	
		//定时器测试OK
//		while(1)
//		{
//			if(time==1000)//计时一秒已到
//			{
//				led_1=!led_1;
//				time=0;	
//			}
//		}

		//Bluetooth
		

}
