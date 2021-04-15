#ifndef _HC12_H_
#define _HC12_H_

/*******************************************************************/
//                         代码移植区                              //
#include "config.h"

#define HC12_UART     USART1

#define HC12_UART_IRQ USART1_IRQn

#define HC12_IRQHandler USART1_IRQHandler

#define HC12_RCC_PORT  RCC_APB2Periph_GPIOA
#define HC12_RCC_UART  RCC_APB2Periph_USART1

#define HC12_GPIO_PORT GPIOA 
#define HC12_RX_PIN    GPIO_Pin_10
#define HC12_TX_PIN    GPIO_Pin_9

#define HC12_GPIO_BAUD 9600
/*******************************************************************/



/*******************************************************************/
//                       函数变量声明区                            //
void HC12_UART_Config(void);
u8 HC12TxPackage(void);

extern u8 HC12TxBuff[128];
extern u8 HC12RxBuff[128];

extern u8 HC12TxPosition;
extern u8 HC12RxPosition;

extern u8 HC12TxLength;
extern u8 HC12RxLength;
extern u8 HC12RxFlag;
/*******************************************************************/








#endif

