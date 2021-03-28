#ifndef _TIMER_TRIGER_H_
#define _TIMER_TRIGER_H_


#include <stm32f10x.h>

typedef enum 
{
	STOP = 0,
	TRIGER_UP,
	TRIGER_DOWN
}TrigerMode;

#define TRIGER_MAXNUM 10
#define TRIGER_ERRO 0XFFFF
#define TRIGER_NO   0XFFFE
#define TRIGER_MAX  0XFFFD
#define TRIGER_YES  0X0000

extern void initTimerTriger(u8 ms);
extern u8 registerTriger(TrigerMode mode, u16 val);
extern u16 isTriger(u8 id);

#endif
