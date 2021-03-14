#ifndef _HC12_H_
#define _HC12_H_

/*******************************************************************/
//                         代码移植区                              //
#include "config.h"
#include "PackageDeal.h"

#define HC12_UART     USART3

#define HC12_UART_IRQ USART3_IRQn

#define HC12_IRQHandler USART3_IRQHandler

#define HC12_RCC_PORT  RCC_APB2Periph_GPIOB
#define HC12_RCC_UART  RCC_APB1Periph_USART3

#define HC12_GPIO_PORT GPIOB 
#define HC12_RX_PIN    GPIO_Pin_11
#define HC12_TX_PIN    GPIO_Pin_10

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

