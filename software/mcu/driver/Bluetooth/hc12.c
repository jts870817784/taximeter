
#include "hc12.h"
#include "stdio.h"
#include "math.h"
#include "24cxx.h"

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
    
    NVIC_InitStructure.NVIC_IRQChannel = HC12_UART_IRQ;         //Ñ¡ÔñUSARTÖÐ¶Ï
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //ÉèÖÃÇÀÕ¼ÓÅÏÈ¼¶
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        //ÉèÖÃÏìÓ¦ÓÅÏÈ¼¶
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //Ê¹ÄÜÍ¨µÀ
    NVIC_Init(&NVIC_InitStructure);
}


void HC12_UART_Config(void)
{
	GPIO_InitTypeDef GPIO_initStructure;
	USART_InitTypeDef USART_initStructure;
	
	RCC_APB2PeriphClockCmd(HC12_RCC_PORT,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(HC12_RCC_UART,ENABLE);
	
	GPIO_initStructure.GPIO_Pin = HC12_RX_PIN;
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//¸¡¿ÕÊäÈë
	GPIO_initStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(HC12_GPIO_PORT,&GPIO_initStructure);
	
	GPIO_initStructure.GPIO_Pin = HC12_TX_PIN;
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//¸´ÓÃÍÆÍêÊä³ö
	GPIO_initStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(HC12_GPIO_PORT,&GPIO_initStructure);
	//ÅäÖÃUSART
	USART_initStructure.USART_BaudRate = HC12_GPIO_BAUD;
	USART_initStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_initStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_initStructure.USART_Parity = USART_Parity_No;
	USART_initStructure.USART_StopBits = USART_StopBits_1;
	USART_initStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(HC12_UART,&USART_initStructure);
	
	NVIC_HC12_USARTEnable();//ÉèÖÃÖÐ¶Ï·Ö×éÓÅÏÈ¼¶
	
	HC12_UART->SR = 0X00;
	HC12_UART->DR = 0X00;
	USART_ITConfig(HC12_UART,USART_IT_RXNE, ENABLE); //¿ªÆô´®¿Ú½ÓÊÜÖÐ¶Ï
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

void HC12RxPackage(u8 dat)
{
	static u8 head_flag=0,sum=0;
    u16 *head = (u16 *)HC12RxBuff;
    u16 *tail = NULL;

    if (head_flag) {
        HC12RxBuff[HC12RxPosition++] = dat;
        sum += dat;
		tail = (u16 *)(HC12RxBuff + HC12RxPosition - 2);
		if (*tail == END_CODE) {
            HC12RxPosition = 0;
			HC12RxFlag = 1;
			head_flag = 0;
			sum = 0;
		} else {
            if(HC12RxBuff[2] <= HC12RxPosition) {
                HC12RxPosition = 0;
                HC12RxFlag = 0;
                head_flag = 0;
                sum = 0;
            }
		}
	} else {
	    HC12RxBuff[0] = HC12RxBuff[1];
        HC12RxBuff[1] = dat;
        if (*head == HEADER_CODE) {
            //æŽ¥æ”¶åˆ°å¸§å¤´
    		head_flag = 1;
    		sum = 0;
    		HC12RxPosition=2;
    		HC12RxBuff[0]=dat;
        }
	}
	
}

void HC12_IRQHandler(void)
{
    if(USART_GetITStatus(HC12_UART,USART_IT_RXNE) == SET)  //¼ì²â½ÓÊÕÖÐ¶Ï±êÖ¾Î»    
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



//******************************ÖØ¶¨Ïò±ê×¼¿âÊäÈëÊä³ö**********************************************//

int fputc(int ch,FILE *f)
{
/*
	USART_SendData(HC12_UART,(u8)ch);
	while(USART_GetFlagStatus(HC12_UART,USART_FLAG_TXE)==RESET);
*/
	return ch;
}

int fgetc(FILE *f)
{
/*
	while((HC12_UART->SR & USART_FLAG_RXNE) == RESET);
	//USART1->RQR |= USART_RQR_RXFRQ;
*/
	return (int)(HC12_UART->DR);
 
}



