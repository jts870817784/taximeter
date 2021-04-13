#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"
#include "config.h"
#include "led.h"

extern u8 Trg ;
extern u8 Cont;


#define SW4_IRQHandler   EXTI9_5_IRQHandler
#define SW3_IRQHandler   EXTI0_IRQHandler
#define SW2_IRQHandler   EXTI1_IRQHandler
#define SW1_IRQHandler   EXTI2_IRQHandler

#define PIN_KEY_START   PBin(2)
#define PIN_KEY_STOP    PAin(7)
#define PIN_KEY_PRE     PBin(1)
#define PIN_KEY_NEXT    PBin(0)

typedef enum
{
	KEY_START=0,
	KEY_STOP,
	KEY_PRE,
	KEY_NEXT
}keyval; //¼üÖµÓ³Éä


//#define IS_KEY_TRG(KEY) (Trg&(0x01<<(KEY)))

void EXTI_Key_Config(void);
void keyScan(void);
int IS_KEY_TRG(keyval k);
#endif


