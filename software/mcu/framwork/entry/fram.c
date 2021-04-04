#include "stdio.h"
#include "timer_triger.h"
#include "DS1307.h"
#include "key.h"
#include "math.h"
#include "mpu6050.h"
#include "lcd12864.h"

/* 触发任务间隔，单位ms */
#define UPDATA_MPU_TIME        6
#define UPDATA_LCD_TIME        20
#define UPDATA_BLUE_TOOTH_TIME 2
#define UPDATA_KEY_TTIME       10

#define UPDATA_TASK_TIME       2
#define MENU_TASK_TIME         2


#define MAX_ORDER_NUM 128

#define ORDER_BASE_PRICE 800    /* 8块钱起步价 */
#define ORDER_TIME_PRICE 30     /* 0.3￥/分钟 */
#define ORDER_MILE_PRICE 300    /* 3￥/km */

#define ACCEL_G 9.8
#define ACCEL_RATIO (4.0 / 4096)


#define HEADER_CODE 0X55AA
#define END_CODE    0XAA55

#define TEXT_GRID 12

#define TEXT_ORDER_POS_X  5
#define TEXT_ORDER_POS_Y  5
#define TEXT_MONEY_POS_X  82
#define TEXT_MONEY_POS_Y  TEXT_ORDER_POS_Y
#define TEXT_MILE_POS_X   TEXT_ORDER_POS_X
#define TEXT_MILE_POS_Y   (TEXT_ORDER_POS_Y + TEXT_GRID)
#define TEXT_TIME_POS_X   TEXT_MONEY_POS_X
#define TEXT_TIME_POS_Y   TEXT_MILE_POS_Y
#define TEXT_START_POS_X  TEXT_ORDER_POS_X
#define TEXT_START_POS_Y  (TEXT_MILE_POS_Y + TEXT_GRID)
#define TEXT_END_POS_X    TEXT_ORDER_POS_X
#define TEXT_END_POS_Y    (TEXT_START_POS_Y + TEXT_GRID)


#define GET_MONEY(time, mile) (((time) * ORDER_TIME_PRICE) + ((mile) * ORDER_MILE_PRICE) + ORDER_BASE_PRICE)

typedef enum tagRunStatus {
    RUNNING = 0,
    IDLES
} runStatus;

typedef struct tagOrderPacket {
    u16 header;
    u8 typeLength;
    u16 orderNumber;
    _calendar_obj startTime;
    _calendar_obj endTime;
    long mile;
    u16 money;
    u16 dif;
    u16 end;
} orderPacket;

runStatus g_runStatus = IDLES;
_calendar_obj g_timePoint;
_calendar_obj g_date;

orderPacket g_orderData[MAX_ORDER_NUM];

char g_strOrder[16];
char g_strMoney[16];
char g_strMile[16];
char g_strTime[16];
char g_strStart[64];
char g_strEnd[64];

long g_mile = 0;
s8 g_pageNow = 0;
u8 g_pageNum = 0;

float averAccel;
float ax, ay, az;

u16 getTimeSub(_calendar_obj *s, _calendar_obj *e)
{
    return (e->hour - s->hour) * 60 + (e->min - s->min);
}

extern double sqrt(double x);

void updataMpu()
{
    static u8 time = 0;
    short Accel[3];

    time++;    
	if (MPU6050ReadID())
	{	
		MPU6050ReadAcc(Accel);
        ax = Accel[0] * ACCEL_RATIO;
        ay = Accel[1] * ACCEL_RATIO;
        az = Accel[2] * ACCEL_RATIO;

        az = az - ACCEL_G;
        averAccel =  sqrt(ax*ax + ay*ay + az*az);

        g_mile = g_mile + time * UPDATA_MPU_TIME * averAccel; /* 路程等于三维加速度向量的模的积分 */
        
        time = 1;
	}
}

void updataLcd()
{
    LCD_Display_Words(TEXT_ORDER_POS_X, TEXT_ORDER_POS_Y, (uint8_t *)g_strOrder);
    LCD_Display_Words(TEXT_MONEY_POS_X, TEXT_MONEY_POS_Y, (uint8_t *)g_strMoney);
    LCD_Display_Words(TEXT_MILE_POS_X, TEXT_MILE_POS_Y, (uint8_t *)g_strMile);
    LCD_Display_Words(TEXT_TIME_POS_X, TEXT_TIME_POS_Y, (uint8_t *)g_strTime);
    LCD_Display_Words(TEXT_START_POS_X, TEXT_START_POS_Y, (uint8_t *)g_strStart);
    LCD_Display_Words(TEXT_END_POS_X, TEXT_END_POS_Y, (uint8_t *)g_strEnd);    
}

void updataRtc()
{
    DS1307_ReadRtc((u8*)&g_date);
}

void updataBlueToothStatus()
{
    ;
}

void updataKey()
{
    keyScan();
}
void updataTask()
{

    u8 updataMpuId;
    u8 updataLcdId;
    u8 updataBtId;
    u8 updataKeyId;

    updataMpuId = registerTriger(TRIGER_DOWN, UPDATA_MPU_TIME);
    updataLcdId = registerTriger(TRIGER_DOWN, UPDATA_LCD_TIME);
    updataBtId  = registerTriger(TRIGER_DOWN, UPDATA_BLUE_TOOTH_TIME);
    updataKeyId = registerTriger(TRIGER_DOWN, UPDATA_KEY_TTIME);

    if (isTriger(updataMpuId) == TRIGER_YES) {
        updataMpu();
    }
    if (isTriger(updataBtId) == TRIGER_YES) {
        updataBlueToothStatus();
    }
    if (isTriger(updataLcdId) == TRIGER_YES) {
        updataLcd();
    }
    if (isTriger(updataKeyId) == TRIGER_YES) {
        updataKey();
    }
}

void dispRunning()
{
    /* running status display */
    float money = 0;
    u16 timeDif = 0;

    timeDif = getTimeSub(&g_timePoint, &g_date);
    money = GET_MONEY(timeDif, g_mile);
    sprintf(g_strOrder, "  odr:%05d", g_pageNum + 1);
    sprintf(g_strMoney, " mny:%3.1f$", money);
    sprintf(g_strMile, " mile:%4.1fkm", g_mile);
    sprintf(g_strTime, "time:%2dmin", timeDif);
    sprintf(g_strStart, "start:%4d-%2d-%2d  %2d:%2d:%2d", g_timePoint.w_year, g_timePoint.w_month,
        g_timePoint.w_date, g_timePoint.hour, g_timePoint.min, g_timePoint.sec);
    sprintf(g_strStart, "  now:%4d-%2d-%2d  %2d:%2d:%2d",g_date.w_year, g_date.w_month,
        g_date.w_date, g_date.hour, g_date.min, g_date.sec);
}

void dispIdle()
{
    /* idle status display */
    u16 timeDif = getTimeSub(&g_orderData[g_pageNow].startTime, &g_orderData[g_pageNow].endTime);

    sprintf(g_strOrder, "  odr:%05d", g_pageNow);
    sprintf(g_strMoney, " mny:%3.1f$", g_orderData[g_pageNow].money);
    sprintf(g_strMile, " mile:%4.1fkm", g_orderData[g_pageNow].mile);
    sprintf(g_strTime, "time:%2dmin", timeDif);
    sprintf(g_strStart, "start:%4d-%2d-%2d  %2d:%2d:%2d", g_orderData[g_pageNow].startTime.w_year,
        g_orderData[g_pageNow].startTime.w_month, g_orderData[g_pageNow].startTime.w_date, g_orderData[g_pageNow].startTime.hour,
        g_orderData[g_pageNow].startTime.min, g_orderData[g_pageNow].startTime.sec);
    sprintf(g_strStart, "start:%4d-%2d-%2d  %2d:%2d:%2d", g_orderData[g_pageNow].endTime.w_year,
        g_orderData[g_pageNow].endTime.w_month, g_orderData[g_pageNow].endTime.w_date, g_orderData[g_pageNow].endTime.hour,
        g_orderData[g_pageNow].endTime.min, g_orderData[g_pageNow].endTime.sec);
}

u16 calDif(u8 *buf, u8 len)
{
    u16 sum = 0;
    u8 i;
    for (i=0; i<len; i++) {
        sum = sum + buf[i];
    }
    return sum;
}


void fillOrderPacket(orderPacket *odr)
{
    u16 timeDif = 0;
    odr->startTime = g_timePoint;
    odr->endTime = calendar;
    odr->mile = g_mile / 1000;
    odr->orderNumber = ++g_pageNow + 10000;
    timeDif = getTimeSub(&odr->startTime, &odr->endTime);
    odr->money = GET_MONEY(timeDif, odr->mile); 
    odr->header = HEADER_CODE;
    odr->end = END_CODE;
    odr->typeLength = sizeof(orderPacket);
    odr->dif = calDif((u8 *)odr + 2, sizeof(orderPacket) - 6);
}

void menuTask()
{
    if (g_runStatus == IDLES) {
        if (IS_KEY_TRG(KEY_START)) {
            /* start a order */
            g_runStatus = RUNNING;
            g_mile = 0;
            DS1307_ReadRtc((u8*)&g_timePoint); /* record time now. */
            dispRunning();
        } else if (IS_KEY_TRG(KEY_PRE)) {
            g_pageNow--;
            dispIdle();
        } else if (IS_KEY_TRG(KEY_NEXT)) {
            g_pageNow++;
            dispIdle();
        }
    } else if (g_runStatus == RUNNING) {
        if (IS_KEY_TRG(KEY_STOP)) {
            /* stop a order */
            g_runStatus = IDLES;
            DS1307_ReadRtc((u8*)&calendar);
            fillOrderPacket(g_orderData + g_pageNow++);
            g_pageNum++;
            dispIdle();
        } else if (IS_KEY_TRG(KEY_PRE) || IS_KEY_TRG(KEY_NEXT)) {
            dispIdle();
        }
    } else {
        /* never run to here! */;
    }
}

void fram() 
{
    u8 updataTaskId = 0;
    u8 menuTaskId = 0;

    updataTaskId = registerTriger(TRIGER_DOWN, UPDATA_TASK_TIME);
    menuTaskId = registerTriger(TRIGER_DOWN, MENU_TASK_TIME);
    while (1) {

        if (isTriger(updataTaskId) == TRIGER_YES) {
            updataTask();
        }

        if (isTriger(menuTaskId) == TRIGER_YES) {
            menuTask();
        }
    }
}


