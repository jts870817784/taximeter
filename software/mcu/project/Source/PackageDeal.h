#ifndef _PACKAGEDEAL_H_
#define _PACKAGEDEAL_H_

#include "config.h"
#include "hc12.h"




/*******************************************************************/
//                     数据包协议                                  //

#define PACKET_HEADER 0xA5
#define PACKET_RETAIL 0x5A

#define CMD_MAX_NUM   16
#define PACKET_MAX_LENGTH 32

typedef enum
{
	//以下为收到指令包
	PACKET_CMD_STOP=0XFF,        //急停！关闭所有外设
	PACKET_CMD_WAITE=0X00,       //等待遥控信号指令
	PACKET_CMD_REQUEST_ACK,      //要求返回应答
	PACKET_CMD_REQUEST_STATULS,  //要求返回状态
	PACKET_CMD_REQUEST_DBUG_INFO,//要求返回调试信息
	PACKET_CMD_SET_SYSTEM_ERROR, //要求设置系统静态
	
#define PACKET_CMD_MAX PACKET_CMD_SET_SYSTEM_ERROR
	//以上为单一指令包，只包含4字节内容
	PACKET_CMD_SET_PID,          //要求设置PID参数
	
	
	
	//以下为收到数据包
	PACKET_DATA_STATULS,         //状态数据包
	PACKET_DATA_DBUG_INFO,       //调试数据包
	PACKET_DATA_REMOTE,          //遥控数据包
	PACKET_DATA_ACK              //ACK应答
#define PAKCET_DATA_LAST PACKET_DATA_ACK
}PACKET_CMD;

#define LINK_STATULS 0X8000      //连接标置
/********************************************************************/


#define GetCmdStatus(cmd)   (CmdStatus&(0x01<<(cmd)))       //获取命令状态
#define ClearCmdStatus(cmd) (CmdStatus&=(~(0x01<<(cmd))))   //清除命令状态
#define SetCmdStatus(cmd)   (CmdStatus|=(0x01<<(cmd)))      //设置命令状态




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
