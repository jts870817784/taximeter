#ifndef _PACKAGEDEAL_H_
#define _PACKAGEDEAL_H_

#include "config.h"
#include "hc12.h"




/*******************************************************************/
//                     ���ݰ�Э��                                  //

#define PACKET_HEADER 0xA5
#define PACKET_RETAIL 0x5A

#define CMD_MAX_NUM   16
#define PACKET_MAX_LENGTH 32

typedef enum
{
	//����Ϊ�յ�ָ���
	PACKET_CMD_STOP=0XFF,        //��ͣ���ر���������
	PACKET_CMD_WAITE=0X00,       //�ȴ�ң���ź�ָ��
	PACKET_CMD_REQUEST_ACK,      //Ҫ�󷵻�Ӧ��
	PACKET_CMD_REQUEST_STATULS,  //Ҫ�󷵻�״̬
	PACKET_CMD_REQUEST_DBUG_INFO,//Ҫ�󷵻ص�����Ϣ
	PACKET_CMD_SET_SYSTEM_ERROR, //Ҫ������ϵͳ��̬
	
#define PACKET_CMD_MAX PACKET_CMD_SET_SYSTEM_ERROR
	//����Ϊ��һָ�����ֻ����4�ֽ�����
	PACKET_CMD_SET_PID,          //Ҫ������PID����
	
	
	
	//����Ϊ�յ����ݰ�
	PACKET_DATA_STATULS,         //״̬���ݰ�
	PACKET_DATA_DBUG_INFO,       //�������ݰ�
	PACKET_DATA_REMOTE,          //ң�����ݰ�
	PACKET_DATA_ACK              //ACKӦ��
#define PAKCET_DATA_LAST PACKET_DATA_ACK
}PACKET_CMD;

#define LINK_STATULS 0X8000      //���ӱ���
/********************************************************************/


#define GetCmdStatus(cmd)   (CmdStatus&(0x01<<(cmd)))       //��ȡ����״̬
#define ClearCmdStatus(cmd) (CmdStatus&=(~(0x01<<(cmd))))   //�������״̬
#define SetCmdStatus(cmd)   (CmdStatus|=(0x01<<(cmd)))      //��������״̬




u8 RxCmdRequestACK(void);
u8 RxDataACK(void);

u8 RxCmdWait(void);
u8 RxCmdRequestStatus(void);
u8 RxCmdRequestDbugInfo(void);
u8 RxCmdSetSystemError(void);
u8 RxCmdSetPid(void);
u8 RxDataRemote(void);

u8 RxDataStatuls(void);
u8 RxDataDbugInfo(void);


void TxCmd(PACKET_CMD cmd);
void TxCmdSetPid(u8 select,float arg);
void TxDataStatuls(u16 pwm,float anglex,float angley,u8 power);
void TxDataDbugInfo(float anglex,float angley,float in_x,float in_y);
void TxDataRemote(u16 power,float anglex,float angley);

void HC12RxPackage(u8 dat);

#endif
