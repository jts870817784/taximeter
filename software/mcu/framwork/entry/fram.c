#include "stdio.h"
#include "string.h"
#include "timer_triger.h"
#include "DS1307.h"
#include "key.h"
#include "math.h"
#include "mpu6050.h"
#include "lcd12864.h"
#include "24cxx.h"
#include "hc12.h"

/* 触发任务间隔，单位ms */
#define UPDATA_MPU_TIME        6
#define UPDATA_LCD_TIME        20
#define UPDATA_BLUE_TOOTH_TIME 2
#define UPDATA_KEY_TTIME       10
#define UPDATA_RTC_TIME        500

#define UPDATA_TASK_TIME       2
#define MENU_TASK_TIME         2

#define ORDER_BASE_PRICE 800    /* 8块钱起步�?*/
#define ORDER_TIME_PRICE 30     /* 0.3�?分钟 */
#define ORDER_MILE_PRICE 300    /* 3�?km */

#define ACCEL_G 9.8
#define ACCEL_RATIO (4.0 / 4096)

#define TEXT_GRID 1

#define TEXT_ORDER_POS_X  0
#define TEXT_ORDER_POS_Y  0
#define TEXT_MONEY_POS_X  4
#define TEXT_MONEY_POS_Y  TEXT_ORDER_POS_Y
#define TEXT_MILE_POS_X   TEXT_ORDER_POS_X
#define TEXT_MILE_POS_Y   (TEXT_ORDER_POS_Y + TEXT_GRID)
#define TEXT_TIME_POS_X   TEXT_MONEY_POS_X
#define TEXT_TIME_POS_Y   TEXT_MILE_POS_Y
#define TEXT_START_POS_X  TEXT_ORDER_POS_X
#define TEXT_START_POS_Y  (TEXT_MILE_POS_Y + TEXT_GRID)
#define TEXT_END_POS_X    TEXT_ORDER_POS_X
#define TEXT_END_POS_Y    (TEXT_START_POS_Y + TEXT_GRID)


#define GET_MONEY(time, mile) ((((time) * ORDER_TIME_PRICE) + ((mile) / 1000 * ORDER_MILE_PRICE) + ORDER_BASE_PRICE) / 100.0)

typedef enum tagRunStatus {
    RUNNING = 0,
    IDLES
} runStatus;

runStatus g_runStatus = IDLES;
_calendar_obj g_timePoint;
_calendar_obj g_date;

orderPacket g_orderData[ORDER_MAX_DATA_NUM];

char g_strOrder[16];
char g_strMoney[16];
char g_strMile[16];
char g_strTime[16];
char g_strStart[64];
char g_strEnd[64];

long g_mile = 0;
s8 g_pageNow = 0;
u8 g_pageNum = 0;

u8 updataTaskId;
u8 menuTaskId;
u8 updataMpuId;
u8 updataLcdId;
u8 updataBtId;
u8 updataKeyId;
u8 updataRtcId;

float averAccel;
float ax, ay, az;

void updataOdrpktToFlash(void);
void updataOdrpktToLoc(void);


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

        g_mile = g_mile + time * UPDATA_MPU_TIME * UPDATA_MPU_TIME * averAccel / 2; /* 路程等于三维加速度向量的模的积�?*/
        
        time = 0;
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
    static u8 index = 0;
    if (HC12RxFlag) {
        if (index >= g_pageNum) {
            HC12RxFlag = 0;
            index = 0;
        } else {
            memcpy(HC12TxBuff, (void *)(g_orderData+index), sizeof(orderPacket));
            HC12TxLength = sizeof(orderPacket);
            HC12TxPackage();
            index++;
        }
    }
}

void updataKey()
{
    keyScan();
}
void updataTask()
{
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
    if (isTriger(updataRtcId) == TRIGER_YES) {
        updataRtc();
    }
}

void dispRunning()
{
    /* running status display */
    float money = 0;
    u16 timeDif = 0;

    timeDif = getTimeSub(&g_timePoint, &g_date);
    money = GET_MONEY(timeDif, g_mile);
    sprintf(g_strOrder, "#:%05d", g_pageNum);
    sprintf(g_strMoney, " %3.1f$", money / 10.0);
    sprintf(g_strMile, "M:%4.1fkm", g_mile / 10.0);
    sprintf(g_strTime, " T:%2dmin", timeDif);
    sprintf(g_strStart, "s:%02d-%02d %02d:%02d:%02d", g_timePoint.w_month,
        g_timePoint.w_date, g_timePoint.hour, g_timePoint.min, g_timePoint.sec);
    sprintf((char *)g_strEnd, "n:%02d-%02d %02d:%02d:%02d", g_date.w_month,
        g_date.w_date, g_date.hour, g_date.min, g_date.sec);
}

void dispIdle()
{
    /* idle status display */
    u16 timeDif = getTimeSub(&g_orderData[g_pageNow].startTime, &g_orderData[g_pageNow].endTime);

    sprintf(g_strOrder, "#:%05d", g_orderData[g_pageNow].orderNumber);
    sprintf(g_strMoney, " %3.1f$", g_orderData[g_pageNow].money / 10.0);
    sprintf(g_strMile, "M:%4.1fkm", g_orderData[g_pageNow].mile / 10.0);
    sprintf(g_strTime, " T:%2dmin", timeDif);
    sprintf(g_strStart, "s:%02d-%02d %02d:%02d:%02d",
        g_orderData[g_pageNow].startTime.w_month, g_orderData[g_pageNow].startTime.w_date, g_orderData[g_pageNow].startTime.hour,
        g_orderData[g_pageNow].startTime.min, g_orderData[g_pageNow].startTime.sec);
    sprintf(g_strEnd, "e:%02d-%02d %02d:%02d:%02d",
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
    odr->orderNumber = g_pageNum + 10000;
    timeDif = getTimeSub(&odr->startTime, &odr->endTime);
    odr->money = GET_MONEY(timeDif, odr->mile); 
    odr->header = HEADER_CODE;
    odr->end = END_CODE;
    odr->typeLength = sizeof(orderPacket);
    odr->dif = calDif((u8 *)odr + 2, sizeof(orderPacket) - 6);
}

void delOrderPacket()
{
    u8 i;
    orderPacket tmp;

    for (i=g_pageNow + 1; i<g_pageNum; i++) {
        g_orderData[i-1] = g_orderData[i];
    }
    memset((void *)&g_orderData[i-1], 0, sizeof(orderPacket));
    updataOdrpktToFlash();

    g_pageNum--;
    if (g_pageNow >= 0) {
        g_pageNow--;
    }
}

void updataOdrpktToFlash(void)
{
    flashWrite(g_orderData);
}

void updataOdrpktToLoc(void)
{
    flashRead(g_orderData);
}

void idleTask() {
    if (IS_KEY_TRG(KEY_START)) {
        /* start a order */
        g_runStatus = RUNNING;
        g_mile = 0;
		g_pageNow = g_pageNum;
        DS1307_ReadRtc((u8*)&g_timePoint); /* record time now. */
        dispRunning();
    } else if (IS_KEY_TRG(KEY_PRE)) {
        if (g_pageNow > 0) {
            g_pageNow--;
        }
        dispIdle();
    } else if (IS_KEY_TRG(KEY_NEXT)) {
        if (g_pageNow < ORDER_MAX_DATA_NUM - 1 && g_pageNow < g_pageNum - 1) {
            g_pageNow++;
        }
        dispIdle();
    } else if (IS_KEY_TRG(KEY_STOP)) {
        delOrderPacket();
    } else {
        dispIdle();
    }
}

void runingTask()
{
    if (IS_KEY_TRG(KEY_STOP)) {
        /* stop a order */
        g_runStatus = IDLES;
        DS1307_ReadRtc((u8*)&calendar);
        fillOrderPacket(g_orderData + g_pageNum);
        updataOdrpktToFlash();
		g_pageNow = g_pageNum;
        g_pageNum++;
        dispIdle();
    } else if (IS_KEY_TRG(KEY_PRE) || IS_KEY_TRG(KEY_NEXT)) {
//        dispIdle();
    } else {
        dispRunning();
    }
}
void menuTask()
{
    if (g_runStatus == IDLES) {
        idleTask();
    } else if (g_runStatus == RUNNING) {
        runingTask();
    } else {
        /* never run to here! */;
    }
}

void fram() 
{
    updataTaskId = registerTriger(TRIGER_DOWN, UPDATA_TASK_TIME);
    menuTaskId = registerTriger(TRIGER_DOWN, MENU_TASK_TIME);
	updataMpuId = registerTriger(TRIGER_DOWN, UPDATA_MPU_TIME);
    updataLcdId = registerTriger(TRIGER_DOWN, UPDATA_LCD_TIME);
    updataBtId  = registerTriger(TRIGER_DOWN, UPDATA_BLUE_TOOTH_TIME);
    updataKeyId = registerTriger(TRIGER_DOWN, UPDATA_KEY_TTIME);
    updataRtcId = registerTriger(TRIGER_DOWN, UPDATA_RTC_TIME);
    while (1) {

        if (isTriger(updataTaskId) == TRIGER_YES) {
            updataTask();
        }

        if (isTriger(menuTaskId) == TRIGER_YES) {
            menuTask();
        }
    }
}

void initFramWork()
{
    u8 i;
    for (i=0; i<ORDER_MAX_DATA_NUM; i++) {
        if (g_orderData[i].header != HEADER_CODE) {
            break;
        }
    }
    g_pageNum = i;
    g_pageNow = 0;
}

