#ifndef  __LED_H
#define  __LED_H


#include "stm32f10x.h"
#include "sys.h"

#define led_1  PAout(4)
#define led_2  PAout(5)
#define led_3  PAout(6)




void led_gpio_config(void);




#endif


