#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"
#include "led.h"


#define SW4_IRQHandler   EXTI9_5_IRQHandler
#define SW3_IRQHandler   EXTI0_IRQHandler
#define SW2_IRQHandler   EXTI1_IRQHandler
#define SW1_IRQHandler   EXTI2_IRQHandler


void EXTI_Key_Config(void);









#endif


