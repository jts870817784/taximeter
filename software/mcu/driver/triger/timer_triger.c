/*

	�ô�������һ��������ʱ��������ʹ��֮ǰӦ���ȶ�����г�ʼ����
����һ��������ʹ��CreateTriger������ȡID�������ô���ʱ�䡣Ȼ��
��ͨ��IsTriger�����ж��Ƿ񴥷����ж��������£�
		  ��������ID�ţ��򷵻�TRIGER_ERRO
          ��δ�������򷵻�TRIGER_NO
		  ��������ģʽ�������򷵻س�������ʱ��ֵ��������������0
          �����½�ģʽ�������򷵻�TRIGER_YES��������������Ϊval

*/


#include "timer_triger.h"


u16 triger_buf[TRIGER_MAXNUM] = {0};   //������������
TrigerMode triger_mode[TRIGER_MAXNUM]; //������ģʽѡ��
u16 triger_val[TRIGER_MAXNUM];         //������ע��ʱ��ֵ
u8 triger_number = 0;                  //����������

void NVIC_init()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}

void initTimerTriger(u8 ms)//��̴���ʱ�䵥λ����
{                            //ms���Ϊ6553ms
	u8 i;
	TIM_TimeBaseInitTypeDef TIM_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	TIM_InitStructure.TIM_ClockDivision = 0;
	TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_InitStructure.TIM_Period = ms*10-1;
	TIM_InitStructure.TIM_Prescaler = 7200;
	TIM_TimeBaseInit(TIM6,&TIM_InitStructure);
	NVIC_init();
	TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
	for(i=0;i<TRIGER_MAXNUM;i++)
	{
		triger_buf[i] = 0;
		triger_mode[i] = STOP;
	}
	triger_number = 0;
	TIM_Cmd(TIM6,ENABLE);
}

/*
������:CreateTriger
�����б�:
	mode:ѡ�����ģʽ������ģʽ����������ģʽ
	val:����val*msΪ����ʱ��
��������:
	���ɹ��򷵻ش�����ID�ţ����򷵻�0xff
*/
u8 registerTriger(TrigerMode mode,u16 val)
{
	if(triger_number >= TRIGER_MAXNUM)
	{
		return 0xff;
	}
	else
	{
        triger_val[triger_number] = val;
		triger_buf[triger_number] = val;
		triger_mode[triger_number] = mode;
		if(mode == TRIGER_UP)
			triger_buf[triger_number] = 0;
		return triger_number++;
	}
}



/*
������: IsTriger
�����б�:
	id:������ID��
�������ã���ѯ�Ƿ���ʱ����
�������أ���������ID�ţ��򷵻�TRIGER_ERRO
          ��δ�������򷵻�TRIGER_NO
		  ��������ģʽ�������򷵻س�������ʱ��ֵ��������������0
          �����½�ģʽ�������򷵻�TRIGER_YES��������������Ϊval
*/
u16 isTriger(u8 id)
{
    u16 val = triger_val[id];
    
	if(id>TRIGER_MAXNUM)
		return TRIGER_ERRO;
	if(triger_mode[id] == STOP)
		return TRIGER_ERRO;
	else if(triger_mode[id] == TRIGER_UP)
	{
		if(val>triger_buf[id])
			return TRIGER_NO;
		val = triger_buf[id] - val;
		triger_buf[id] = 0;
		return val;
	}
	else
	{
		if(triger_buf[id]!=0)
			return TRIGER_NO;
		triger_buf[id] = val;
		return TRIGER_YES;
	}
}
//#include <config.h>
//#define T1 PCout(6)
//#define T2 PCout(7)

void TIM6_IRQHandler(void)
{
//	static u8 flag = 0;
	u8 i;
	if(TIM_GetITStatus(TIM6,TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
		for(i=0;i<triger_number;i++)
		{
			if(triger_mode[i]==TRIGER_DOWN)
			{
				if(triger_buf[i]>0)triger_buf[i]--;
			}
			if(triger_mode[i]==TRIGER_UP)
			{
				if(triger_buf[i]<TRIGER_MAX)triger_buf[i]++;
			}
		}
//		if(flag == 0)
//			T1 = 0;
//		else 
//			T1 = 1;
//		flag = ~flag;
	}
}


