/*

接收信号协议解析:
      --------------------------------------
      | A5 | CMD | D1 D2 D3 ... | SUM | 5A |
      --------------------------------------
	  | u8 | u8  |              | u8  | u8 |
	  --------------------------------------
帧起始：A5
帧结束：5A
命令符：CMD
数据段：D1 D2 D3 ...
校验码：SUM


**************************************************************************************
命令解析如下！

                ********以下命令包**********

0xFF：遥控器发出指令，急停，锁住程序，关闭所有外设，禁止一切动作！
      ----------------------
      | A5 | FF | SUM | 5A |
      ----------------------

0x00：遥控器发出待机指令（PWM调至最低，禁止电机转动）。
      ----------------------
      | A5 | 00 | SUM | 5A |
      ----------------------

0x01：双方均可要求回传一个应答信号。
      ----------------------
      | A5 | 01 | SUM | 5A |
      ----------------------

0x02：遥控器要求返回飞控状态。

      ----------------------
      | A5 | 02 | SUM | 5A |
      ----------------------
	  
0x03：遥控器要求返回调试信息。
      ----------------------
      | A5 | 03 | SUM | 5A |
      ----------------------

0x04：设置飞控系统角度静态误差。
      ----------------------
      | A5 | 04 | SUM | 5A |
      ----------------------
	  

	 
0x05：设置飞控系统PID参数（Select,arg）。
      -------------------------------------
      | A5 | 05 | Seect | arg  | SUM | 5A |
      -------------------------------------
	  | u8 | u8 | u8    | float| u8  | u8 |
	  -------------------------------------
		Select：选择所调节的参数。
		arg：设置参数值

		Select对照表
			0x00：所有参数都设置为arg。
			
			0x01：内环KP
			0x02：内环KD
			0x03：内环KI
			
			0x04：外环KP
			0x05：外环KD
			0x06：外环KI

-------------------------------------------------------------------------------
                  **********以下是数据包***********

	  
0x06：遥控器接收当前飞控状态（PWM，AngleX，AngleY，power）。
      ------------------------------------------------------
      | A5 | 06| PWM | AngleX | AngleY | power | SUM | 5A |
      ------------------------------------------------------
	  | u8 | u8 | u16 | float  | float  | u8    | u8  | u8 |
	  ------------------------------------------------------

0x07：遥控器接收调试信息（AngleX，AngleY，in_x，in_y）。
      ------------------------------------------------------
      | A5 | 07 | AngleX | AngleY | in_x | in_y | SUM | 5A |
      ------------------------------------------------------
	  | u8 | u8 | float  | float  | float| float| u8  | u8 |
	  ------------------------------------------------------


0x08：遥控数据帧（power，anglex，angley）。
      -------------------------------------------------
      | A5 | 08 | power | anglex | angley  | SUM | 5A |
      |-----------------------------------------------|
	  | u8 | u8 | u16   | float  | float   | u8  | 5A |
	  -------------------------------------------------

0x09：双方均可使用接收应答
      ----------------------
      | A5 | 09 | SUM | 5A |
      ----------------------
*/






#include "PackageDeal.h"
#include "stdio.h"

u16 CmdStatus=0;
/*
CmdStatus
--------------------------------------------------------------------------------------
|        15         | 14 | 13 | 12 | 11 | 10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|------------------------------------------------------------------------------------|
|   LINK_STATULS    |            reserve     |GAK|RMT|DGI|STA|SPD|SER|RDI|RS |RAK|RWA|
--------------------------------------------------------------------------------------
*/



u8 (*pfMenu[CMD_MAX_NUM])(void)=
{
	RxCmdWait,
	RxCmdRequestACK,
	RxCmdRequestStatus,
	RxCmdRequestDbugInfo,
	RxCmdSetSystemError,
	RxCmdSetPid,
	
	RxDataStatuls,
	RxDataDbugInfo,
	RxDataRemote,
	RxDataACK
};


/********************以下是发送指令以及数据函数***************************/
void SumAndTx(u8 n)//校验码+发送(n=校验码位置)
{
	u8 i;
	HC12TxBuff[0]=PACKET_HEADER;      //帧起始
	HC12TxBuff[n]=0;                  //校验码先清0，然后固定8位累加
	for(i=0;i<n;i++)
		HC12TxBuff[n]+=HC12TxBuff[i];
	HC12TxBuff[n+1]=PACKET_RETAIL;    //设置帧结束
	HC12TxLength = n+2;               //帧长
	HC12TxPackage();                  //开启发送
}

//发送指令包和ACK数据包
void TxCmd(PACKET_CMD cmd)
{
	if((cmd<=PACKET_CMD_MAX)||cmd == PACKET_DATA_ACK)//数据码ACK比较特殊，可以当成指令
	{
		HC12TxBuff[1]=cmd;
		SumAndTx(2);
	}
}

//0x05：设置飞控系统PID参数（Select,arg）。
//-------------------------------------
//| A5 | 05 | Seect | arg  | SUM | 5A |
//-------------------------------------
//| u8 | u8 | u8    | float| u8  | u8 |
//-------------------------------------
void TxCmdSetPid(u8 select,float arg)
{
	HC12TxBuff[1]=PACKET_CMD_SET_PID;   //命令
	HC12TxBuff[2]=select; //选择设置的参数
	*((float*)(HC12TxBuff+3))=arg;
	SumAndTx(3+sizeof(float));
}

//0x06：遥控器接收当前飞控状态（PWM，AngleX，AngleY，power）。
//------------------------------------------------------
//| A5 | 06| PWM | AngleX | AngleY | power | SUM | 5A |
//------------------------------------------------------
//| u8 | u8 | u16 | float  | float  | u8    | u8  | u8 |
//------------------------------------------------------
void TxDataStatuls(u16 pwm,float anglex,float angley,u8 power)
{
	u8 i=2;
	HC12TxBuff[1]=PACKET_DATA_STATULS;                    
	*((u16*)(HC12TxBuff+i))=pwm;
	i+=sizeof(u16);            
	*((float*)(HC12TxBuff+i))=anglex;
	i+=sizeof(float);              
	*((float*)(HC12TxBuff+i))=angley;
	i+=sizeof(float);
	*((u16*)(HC12TxBuff+i))=power;
	i+=sizeof(u16);
	SumAndTx(i);
}

//0x06：遥控器接收当前飞控状态（PWM，AngleX，AngleY，power）。
//------------------------------------------------------
//| A5 | 07 | AngleX | AngleY | in_x | in_y | SUM | 5A |
//------------------------------------------------------
//| u8 | u8 | float  | float  | float| float| u8  | u8 |
//------------------------------------------------------
void TxDataDbugInfo(float anglex,float angley,float in_x,float in_y)
{
	u8 i=2;
	HC12TxBuff[1]=PACKET_DATA_DBUG_INFO;
	*((float*)(HC12TxBuff+i))=anglex;
	i+=sizeof(float);
	*((float*)(HC12TxBuff+i))=angley;
	i+=sizeof(float);
	*((float*)(HC12TxBuff+i))=in_x;
	i+=sizeof(float);
	*((float*)(HC12TxBuff+i))=in_y;
	i+=sizeof(float);
	SumAndTx(i);
}

//0x08：遥控数据帧（power，anglex，angley）。
//-------------------------------------------------
//| A5 | 08 | power | anglex | angley  | SUM | 5A |
//|-----------------------------------------------|
//| u8 | u8 | u16   | float  | float   | u8  | 5A |
//-------------------------------------------------
void TxDataRemote(u16 power,float anglex,float angley)
{
	u8 i=2;
	HC12TxBuff[1]=PACKET_DATA_REMOTE;
	*((u16*)(HC12TxBuff+i))=power;
	i+=sizeof(u16);       
	*((float*)(HC12TxBuff+i))=anglex;
	i+=sizeof(float);
	*((float*)(HC12TxBuff+i))=angley;
	i+=sizeof(float);
	SumAndTx(i);
}


void HC12RxPackage(u8 dat)
{
	static u8 head_flag=0,sum=0;
	if(head_flag)
	{
		//检测是否到帧尾
		if(dat == PACKET_RETAIL)
		{
			HC12RxBuff[HC12RxPosition] = dat;
			//接收到帧尾，校验数据
			if(HC12RxBuff[HC12RxPosition-1]==(u8)(sum-HC12RxBuff[HC12RxPosition-1]))
			{
				if(HC12RxBuff[1]<PAKCET_DATA_LAST)
					//帧接收完毕,执行指令动作
					pfMenu[HC12RxBuff[1]]();
				head_flag = 0;
				HC12RxPosition=0;
			}
			else
			{
				HC12RxPosition++;
				sum+=dat;
			}
		}
		//已接收到帧头，非帧尾，现在接收的是数据
		else
		{
			//限定帧最大长度
			if(HC12RxPosition>=PACKET_MAX_LENGTH-1)
			{
				head_flag = 0;
				return;
			}
			sum+=dat;
			HC12RxBuff[HC12RxPosition++]=dat;
		}
	}
	else if(dat == PACKET_HEADER)
	{
		//接收到帧头
		head_flag = 1;
		sum = PACKET_HEADER;
		HC12RxPosition=1;
		HC12RxBuff[0]=dat;
	}
	
}

/******************************************************************************/
/*                     */
u8 RxCmdRequestACK()//收到要求应答命令
{
	TxCmd(PACKET_DATA_ACK);        //发送应答包
	SetCmdStatus(PACKET_DATA_ACK); //设置为收到主机请求状态
	return 0;
}
u8 RxDataACK()//收到应答数据包
{
	SetCmdStatus(PACKET_DATA_ACK); //设置为收到回应状态
	return 0;
}


u8 RxCmdWait()//收到等待指令
{
	SetCmdStatus(PACKET_CMD_WAITE);
	return 0;
}

u8 RxCmdRequestStatus()//收到要求回传状态指令
{
	SetCmdStatus(PACKET_CMD_REQUEST_STATULS);
	return 0;
}
u8 RxCmdRequestDbugInfo()//收到要求回传调试信息
{
	SetCmdStatus(PACKET_CMD_REQUEST_DBUG_INFO);
	return 0;
}

u8 RxCmdSetSystemError()//要求设置系统静态误差
{

	//如果要快速响应的话，这里其实没有必要创建标志位
	SetCmdStatus(PACKET_CMD_SET_SYSTEM_ERROR);
	

	/*
	
			这里可以解包做数据处理
	
	*/
	
	
	return 0;
}

u8 RxCmdSetPid()//设置PID参数
{
	u8 select;
	float arg;
	//如果要快速响应的话，这里其实没有必要创建标志位
	SetCmdStatus(PACKET_CMD_SET_PID);
	
	select = HC12RxBuff[2];
	arg = *(float*)(HC12RxBuff+3);
	/*
	
			这里可以解包做数据处理
	
	*/
	
	
	return 0;
}

u8 RxDataRemote()//收到遥控数据
{
	u16 pwm;
	float anglex,angley;
	//如果要快速响应的话，这里其实没有必要创建标志位
	SetCmdStatus((u8)PACKET_DATA_REMOTE);
	
	pwm = *(u16*)(HC12RxBuff+2);
	anglex=*(float*)(HC12RxBuff+4);
	angley=*(float*)(HC12RxBuff+8);
	
	/*
	
			这里可以解包做数据处理
	
	*/
	
	
	
	return 0;
}



u8 RxDataStatuls()//收到状态
{
	u16 pwm;
	u8 power;
	float anglex,angley;
	//如果要快速响应的话，这里其实没有必要创建标志位
	SetCmdStatus(PACKET_DATA_STATULS);
	pwm = *(u16*)(HC12RxBuff+2);
	anglex = *(float*)(HC12RxBuff+4);
	angley = *(float*)(HC12RxBuff+8);
	power = HC12RxBuff[12];
	
	/*
	
			这里可以解包做数据处理
	
	*/
	
	
	return 0;
}

u8 RxDataDbugInfo()
{
	float anglex,angley,in_x,in_y;
	SetCmdStatus(PACKET_DATA_DBUG_INFO);
	anglex = *(float*)(HC12RxBuff+2);
	angley = *(float*)(HC12RxBuff+6);
	in_x = *(float*)(HC12RxBuff+10);
	in_y = *(float*)(HC12RxBuff+14);
	
	/*
	
			这里可以解包做数据处理
	
	*/
	
	
	return 0;
}

