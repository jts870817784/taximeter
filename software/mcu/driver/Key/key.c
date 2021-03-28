#include "key.h"
#include "delay.h"

uint8_t flag=0;
u8 Trg = 0x00;
u8 Cont = 0x00;

void key_gpio_config(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;  //PA�˿���7��
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;  //PB�˿���0 1 2��
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}

static void Key_NVIC_Config(void)
{
	NVIC_InitTypeDef  NVIC_InitStruct;
	
	//����sw1
	NVIC_InitStruct.NVIC_IRQChannel = EXTI2_IRQn;  //Line 2
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;  //ѡ����ռ���ȼ�Ϊ1
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;   //ѡ�������ȼ�Ϊ1
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;  //ʹ���ж�ͨ��
	NVIC_Init(&NVIC_InitStruct);
	
	//����sw2
	NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;  //Line 1
	NVIC_Init(&NVIC_InitStruct);
	
	//����sw3
	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;  //Line 0
	NVIC_Init(&NVIC_InitStruct);
	
	//����sw4
	NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;  //Line 0
	NVIC_Init(&NVIC_InitStruct);
	
}


void Exti_gpio_config(void)
{
	EXTI_InitTypeDef  EXTI_InitStruct;
	
	//SW_1
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource2); //ѡ��EXTI�ź�ԴPB2
	EXTI_InitStruct.EXTI_Line = EXTI_Line2;  //Line 2
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;  //�ж�ģʽ
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;  //�½��ش���
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;  //ʹ���ж�
	EXTI_Init(&EXTI_InitStruct);
	
	//SW_2
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1); //ѡ��EXTI�ź�ԴPB1
	EXTI_InitStruct.EXTI_Line = EXTI_Line1;  //Line 1
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;  //�ж�ģʽ
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;  //�½��ش���
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;  //ʹ���ж�
	EXTI_Init(&EXTI_InitStruct);
	
	//SW_3
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0); //ѡ��EXTI�ź�ԴPB0
	EXTI_InitStruct.EXTI_Line = EXTI_Line0;  //Line 0
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;  //�ж�ģʽ
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;  //�½��ش���
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;  //ʹ���ж�
	EXTI_Init(&EXTI_InitStruct);
	
	//SW_4
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource7); //ѡ��EXTI�ź�ԴPB2
	EXTI_InitStruct.EXTI_Line = EXTI_Line7;  //Line 7
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;  //�ж�ģʽ
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;  //�½��ش���
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;  //ʹ���ж�
	EXTI_Init(&EXTI_InitStruct);
}


void EXTI_Key_Config(void)
{
	key_gpio_config();
	Key_NVIC_Config();
	Exti_gpio_config();
}

//#define SW3_IRQHandler   EXTI0_IRQHandler
//#define SW2_IRQHandler   EXTI1_IRQHandler
//#define SW1_IRQHandler   EXTI2_IRQHandler
//#define SW4_IRQHandler   EXTI9_5_IRQHandler

void SW1_IRQHandler(void)
{
	delay_ms(10);
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2)==0)
	{
		led_1 =(!led_1);
	}
	EXTI_ClearITPendingBit(EXTI_Line2);
}

void SW2_IRQHandler(void)
{
	delay_ms(10);
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)==0)
	{
		led_2 =(!led_2);
	}
	EXTI_ClearITPendingBit(EXTI_Line1);
}

void SW3_IRQHandler(void)
{
	delay_ms(10);
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)==0)
	{
		led_3 =(!led_3);
	}
	EXTI_ClearITPendingBit(EXTI_Line0);
}

void SW4_IRQHandler(void)
{
	delay_ms(10);
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7)==0)
	{
		//sw4�������£��ж�û����������
	}
	EXTI_ClearITPendingBit(EXTI_Line7);
}



void keyScan()
{
	static u8 Rd = {0};
	static u8 dk[6] = {0};
	
	//����
	dk[KEY_START]  = (dk[KEY_START]<<1) |(PIN_KEY_START?0:1);
	dk[KEY_STOP]   = (dk[KEY_STOP]<<1)  |(PIN_KEY_STOP ?0:1);
	dk[KEY_PRE]    = (dk[KEY_PRE]<<1)   |(PIN_KEY_PRE  ?0:1);
	dk[KEY_NEXT]   = (dk[KEY_NEXT]<<1)  |(PIN_KEY_NEXT ?0:1);

	//��������
	Rd = (Rd<<1)|(dk[KEY_START]==0xff?0:1);
	Rd = (Rd<<1)|(dk[KEY_STOP] ==0xff?0:1);
	Rd = (Rd<<1)|(dk[KEY_PRE]  ==0xff?0:1);
	Rd = (Rd<<1)|(dk[KEY_NEXT] ==0xff?0:1);
	
	//����
	Trg = Rd & (Rd ^ Cont);
	Cont = Rd;
	
}

