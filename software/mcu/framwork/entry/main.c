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
#include "lcd12864.h"
#include "24cxx.h"
#include "bsp_TiMbase.h" 

extern volatile uint16_t time;

extern void fram(void);

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
}
