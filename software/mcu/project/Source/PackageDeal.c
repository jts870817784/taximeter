/*

�����ź�Э�����:
      --------------------------------------
      | A5 | CMD | D1 D2 D3 ... | SUM | 5A |
      --------------------------------------
	  | u8 | u8  |              | u8  | u8 |
	  --------------------------------------
֡��ʼ��A5
֡������5A
�������CMD
���ݶΣ�D1 D2 D3 ...
У���룺SUM


**************************************************************************************
����������£�

                ********���������**********

0xFF��ң��������ָ���ͣ����ס���򣬹ر��������裬��ֹһ�ж�����
      ----------------------
      | A5 | FF | SUM | 5A |
      ----------------------

0x00��ң������������ָ�PWM������ͣ���ֹ���ת������
      ----------------------
      | A5 | 00 | SUM | 5A |
      ----------------------

0x01��˫������Ҫ��ش�һ��Ӧ���źš�
      ----------------------
      | A5 | 01 | SUM | 5A |
      ----------------------

0x02��ң����Ҫ�󷵻طɿ�״̬��

      ----------------------
      | A5 | 02 | SUM | 5A |
      ----------------------
	  
0x03��ң����Ҫ�󷵻ص�����Ϣ��
      ----------------------
      | A5 | 03 | SUM | 5A |
      ----------------------

0x04�����÷ɿ�ϵͳ�ǶȾ�̬��
      ----------------------
      | A5 | 04 | SUM | 5A |
      ----------------------
	  

	 
0x05�����÷ɿ�ϵͳPID������Select,arg����
      -------------------------------------
      | A5 | 05 | Seect | arg  | SUM | 5A |
      -------------------------------------
	  | u8 | u8 | u8    | float| u8  | u8 |
	  -------------------------------------
		Select��ѡ�������ڵĲ�����
		arg�����ò���ֵ

		Select���ձ�
			0x00�����в���������Ϊarg��
			
			0x01���ڻ�KP
			0x02���ڻ�KD
			0x03���ڻ�KI
			
			0x04���⻷KP
			0x05���⻷KD
			0x06���⻷KI

-------------------------------------------------------------------------------
                  **********���������ݰ�***********

	  
0x06��ң�������յ�ǰ�ɿ�״̬��PWM��AngleX��AngleY��power����
      ------------------------------------------------------
      | A5 | 06| PWM | AngleX | AngleY | power | SUM | 5A |
      ------------------------------------------------------
	  | u8 | u8 | u16 | float  | float  | u8    | u8  | u8 |
	  ------------------------------------------------------

0x07��ң�������յ�����Ϣ��AngleX��AngleY��in_x��in_y����
      ------------------------------------------------------
      | A5 | 07 | AngleX | AngleY | in_x | in_y | SUM | 5A |
      ------------------------------------------------------
	  | u8 | u8 | float  | float  | float| float| u8  | u8 |
	  ------------------------------------------------------


0x08��ң������֡��power��anglex��angley����
      -------------------------------------------------
      | A5 | 08 | power | anglex | angley  | SUM | 5A |
      |-----------------------------------------------|
	  | u8 | u8 | u16   | float  | float   | u8  | 5A |
	  -------------------------------------------------

0x09��˫������ʹ�ý���Ӧ��
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


/********************�����Ƿ���ָ���Լ����ݺ���***************************/
void SumAndTx(u8 n)//У����+����(n=У����λ��)
{
	u8 i;
	HC12TxBuff[0]=PACKET_HEADER;      //֡��ʼ
	HC12TxBuff[n]=0;                  //У��������0��Ȼ��̶�8λ�ۼ�
	for(i=0;i<n;i++)
		HC12TxBuff[n]+=HC12TxBuff[i];
	HC12TxBuff[n+1]=PACKET_RETAIL;    //����֡����
	HC12TxLength = n+2;               //֡��
	HC12TxPackage();                  //��������
}

//����ָ�����ACK���ݰ�
void TxCmd(PACKET_CMD cmd)
{
	if((cmd<=PACKET_CMD_MAX)||cmd == PACKET_DATA_ACK)//������ACK�Ƚ����⣬���Ե���ָ��
	{
		HC12TxBuff[1]=cmd;
		SumAndTx(2);
	}
}

//0x05�����÷ɿ�ϵͳPID������Select,arg����
//-------------------------------------
//| A5 | 05 | Seect | arg  | SUM | 5A |
//-------------------------------------
//| u8 | u8 | u8    | float| u8  | u8 |
//-------------------------------------
void TxCmdSetPid(u8 select,float arg)
{
	HC12TxBuff[1]=PACKET_CMD_SET_PID;   //����
	HC12TxBuff[2]=select; //ѡ�����õĲ���
	*((float*)(HC12TxBuff+3))=arg;
	SumAndTx(3+sizeof(float));
}

//0x06��ң�������յ�ǰ�ɿ�״̬��PWM��AngleX��AngleY��power����
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

//0x06��ң�������յ�ǰ�ɿ�״̬��PWM��AngleX��AngleY��power����
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

//0x08��ң������֡��power��anglex��angley����
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
		//����Ƿ�֡β
		if(dat == PACKET_RETAIL)
		{
			HC12RxBuff[HC12RxPosition] = dat;
			//���յ�֡β��У������
			if(HC12RxBuff[HC12RxPosition-1]==(u8)(sum-HC12RxBuff[HC12RxPosition-1]))
			{
				if(HC12RxBuff[1]<PAKCET_DATA_LAST)
					//֡�������,ִ��ָ���
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
		//�ѽ��յ�֡ͷ����֡β�����ڽ��յ�������
		else
		{
			//�޶�֡��󳤶�
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
		//���յ�֡ͷ
		head_flag = 1;
		sum = PACKET_HEADER;
		HC12RxPosition=1;
		HC12RxBuff[0]=dat;
	}
	
}

/******************************************************************************/
/*                     */
u8 RxCmdRequestACK()//�յ�Ҫ��Ӧ������
{
	TxCmd(PACKET_DATA_ACK);        //����Ӧ���
	SetCmdStatus(PACKET_DATA_ACK); //����Ϊ�յ���������״̬
	return 0;
}
u8 RxDataACK()//�յ�Ӧ�����ݰ�
{
	SetCmdStatus(PACKET_DATA_ACK); //����Ϊ�յ���Ӧ״̬
	return 0;
}


u8 RxCmdWait()//�յ��ȴ�ָ��
{
	SetCmdStatus(PACKET_CMD_WAITE);
	return 0;
}

u8 RxCmdRequestStatus()//�յ�Ҫ��ش�״ָ̬��
{
	SetCmdStatus(PACKET_CMD_REQUEST_STATULS);
	return 0;
}
u8 RxCmdRequestDbugInfo()//�յ�Ҫ��ش�������Ϣ
{
	SetCmdStatus(PACKET_CMD_REQUEST_DBUG_INFO);
	return 0;
}

u8 RxCmdSetSystemError()//Ҫ������ϵͳ��̬���
{

	//���Ҫ������Ӧ�Ļ���������ʵû�б�Ҫ������־λ
	SetCmdStatus(PACKET_CMD_SET_SYSTEM_ERROR);
	

	/*
	
			������Խ�������ݴ���
	
	*/
	
	
	return 0;
}

u8 RxCmdSetPid()//����PID����
{
	u8 select;
	float arg;
	//���Ҫ������Ӧ�Ļ���������ʵû�б�Ҫ������־λ
	SetCmdStatus(PACKET_CMD_SET_PID);
	
	select = HC12RxBuff[2];
	arg = *(float*)(HC12RxBuff+3);
	/*
	
			������Խ�������ݴ���
	
	*/
	
	
	return 0;
}

u8 RxDataRemote()//�յ�ң������
{
	u16 pwm;
	float anglex,angley;
	//���Ҫ������Ӧ�Ļ���������ʵû�б�Ҫ������־λ
	SetCmdStatus((u8)PACKET_DATA_REMOTE);
	
	pwm = *(u16*)(HC12RxBuff+2);
	anglex=*(float*)(HC12RxBuff+4);
	angley=*(float*)(HC12RxBuff+8);
	
	/*
	
			������Խ�������ݴ���
	
	*/
	
	
	
	return 0;
}



u8 RxDataStatuls()//�յ�״̬
{
	u16 pwm;
	u8 power;
	float anglex,angley;
	//���Ҫ������Ӧ�Ļ���������ʵû�б�Ҫ������־λ
	SetCmdStatus(PACKET_DATA_STATULS);
	pwm = *(u16*)(HC12RxBuff+2);
	anglex = *(float*)(HC12RxBuff+4);
	angley = *(float*)(HC12RxBuff+8);
	power = HC12RxBuff[12];
	
	/*
	
			������Խ�������ݴ���
	
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
	
			������Խ�������ݴ���
	
	*/
	
	
	return 0;
}

