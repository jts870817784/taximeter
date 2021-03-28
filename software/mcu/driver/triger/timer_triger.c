/*

	该触发器由一个基本定时器触发，使用之前应当先对其进行初始化，
创建一个触发器使用CreateTriger函数获取ID，并设置触发时间。然后
可通过IsTriger函数判断是否触发。判断条件如下：
		  若不存在ID号，则返回TRIGER_ERRO
          若未触发，则返回TRIGER_NO
		  若在上升模式触发，则返回超出触发时间值，并将缓冲区清0
          若在下降模式触发，则返回TRIGER_YES，并将缓冲区置为val

*/


#include "timer_triger.h"


u16 triger_buf[TRIGER_MAXNUM] = {0};   //触发器缓冲区
TrigerMode triger_mode[TRIGER_MAXNUM]; //触发器模式选择
u16 triger_val[TRIGER_MAXNUM];         //触发器注册时的值
u8 triger_number = 0;                  //触发器数量

void NVIC_init()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}

void initTimerTriger(u8 ms)//最短触发时间单位毫秒
{                            //ms最大为6553ms
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
函数名:CreateTriger
参数列表:
	mode:选择计数模式，向上模式，或者向下模式
	val:设置val*ms为触发时间
返回类型:
	若成功则返回触发器ID号，否则返回0xff
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
函数名: IsTriger
参数列表:
	id:触发器ID号
函数作用：查询是否延时触发
函数返回：若不存在ID号，则返回TRIGER_ERRO
          若未触发，则返回TRIGER_NO
		  若在上升模式触发，则返回超出触发时间值，并将缓冲区清0
          若在下降模式触发，则返回TRIGER_YES，并将缓冲区置为val
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


