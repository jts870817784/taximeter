#ifndef _TIMER_TRIGER_H_
#define _TIMER_TRIGER_H_


#include <stm32f10x.h>

typedef enum 
{
	STOP,
	TRIGER_UP,
	TRIGER_DOWN
}TrigerMode;

#define TRIGER_MAXNUM 5
#define TRIGER_ERRO 0XFFFF
#define TRIGER_NO   0XFFFE
#define TRIGER_MAX  0XFFFD
#define TRIGER_YES  0X0000

extern void timer_triger_init(u8 ms);
extern u8 CreateTriger(TrigerMode mode,u16 val);
extern u16 IsTriger(u8 id,u16 val);

#endif
