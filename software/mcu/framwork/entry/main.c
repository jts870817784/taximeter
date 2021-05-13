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
#include "hc12.h"
#include "config.h"

#define DEBUG

extern volatile uint16_t time;
extern _calendar_obj g_date;

extern void fram(void);
extern void initFramWork(void);

void bspInit()
{
    delay_init();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);  //选择组1
    led_gpio_config();
    EXTI_Key_Config();  //按键中断
//    USART_Config();     //打开USART2串口
    i2c_GPIO_Config();  //MPU6050的IIC
    MPU6050_Init();     //初始化MPU6050
    IIC_Init();             //Tiny_RTC的IIC初始化 
    lcd_GPIO_init();
	delay_ms(50);
    Lcd_Init();
    BASIC_TIM_Init();
	delay_ms(50);
    bspflashInit();
    HC12_UART_Config();
    initFramWork();
    DS1307_ReadRtc((u8*)&g_date);
#ifdef DEBUG
	{
#else
    if (g_date.hour > 19) {
#endif
        LCD_Clear();
        LCD_Display_Words(0, 0, "夜间行驶!");
		LCD_Display_Words(0, 1, "请注意安全！");
		delay_ms(2000);
        LCD_Clear();
    }
}

int main(void)
{
    bspInit();
    fram();
}
