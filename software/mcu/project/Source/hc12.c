
#include "hc12.h"
#include "stdio.h"
#include "math.h"

u8 HC12TxBuff[128];
u8 HC12RxBuff[128];

u8 HC12TxPosition=0;
u8 HC12RxPosition=0;

u8 HC12TxLength=0;
u8 HC12RxLength=0;

u8 HC12RxFlag = 0;


void NVIC_HC12_USARTEnable(void)  
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = HC12_UART_IRQ;         //选择USART中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //设置抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        //设置响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //使能通道
    NVIC_Init(&NVIC_InitStructure);
}


void HC12_UART_Config(void)
{
	GPIO_InitTypeDef GPIO_initStructure;
	USART_InitTypeDef USART_initStructure;
	
	RCC_APB2PeriphClockCmd(HC12_RCC_PORT,ENABLE);
	RCC_APB1PeriphClockCmd(HC12_RCC_UART,ENABLE);
	
	GPIO_initStructure.GPIO_Pin = HC12_RX_PIN;
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
	GPIO_initStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(HC12_GPIO_PORT,&GPIO_initStructure);
	
	GPIO_initStructure.GPIO_Pin = HC12_TX_PIN;
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//浮空输入
	GPIO_initStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(HC12_GPIO_PORT,&GPIO_initStructure);
	//配置USART
	USART_initStructure.USART_BaudRate = HC12_GPIO_BAUD;
	USART_initStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_initStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_initStructure.USART_Parity = USART_Parity_No;
	USART_initStructure.USART_StopBits = USART_StopBits_1;
	USART_initStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(HC12_UART,&USART_initStructure);
	
	NVIC_HC12_USARTEnable();//设置中断分组优先级
	
	HC12_UART->SR = 0X00;
	HC12_UART->DR = 0X00;
	USART_ITConfig(HC12_UART,USART_IT_RXNE, ENABLE); //开启串口接受中断
	USART_Cmd(HC12_UART,ENABLE);
	
}


u8 HC12TxPackage()
{
	if(HC12TxLength>127)
		return 1;
	HC12TxPosition = 0;
	USART_ITConfig(HC12_UART,USART_IT_TXE,ENABLE);
	
	return 0;
}



void  HC12_IRQHandler(void)
{
    if(USART_GetITStatus(HC12_UART,USART_IT_RXNE) == SET)  //检测接收中断标志位    
    {
		HC12RxPackage(HC12_UART->DR);
    }
	if(USART_GetITStatus(HC12_UART,USART_IT_TXE)==SET)
	{
		
		if(HC12TxPosition<HC12TxLength)
			HC12_UART->DR=HC12TxBuff[HC12TxPosition++];
		else
			USART_ITConfig(HC12_UART,USART_IT_TXE,DISABLE);
	}
}



