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

#define DEBUG

/* è§¦å‘ä»»åŠ¡é—´éš”ï¼Œå•ä½ms */
#define UPDATA_MPU_TIME        6
#define UPDATA_LCD_TIME        100
#define UPDATA_BLUE_TOOTH_TIME 2
#define UPDATA_KEY_TTIME       10
#define UPDATA_RTC_TIME        500

#define UPDATA_TASK_TIME       2
#define MENU_TASK_TIME         2

#define ORDER_BASE_PRICE 800    /* 8å—é’±èµ·æ­¥ä»?*/
#define ORDER_TIME_PRICE 30     /* 0.3ï¿?åˆ†é’Ÿ */
#define ORDER_MILE_PRICE 300    /* 3ï¿?km */

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

typedef __packed struct {
    unsigned char year;
    unsigned char month;
    unsigned char day;
    unsigned char hour;
    unsigned char min;
    unsigned char sec;
} rxCenderPacket;

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
long g_time = 0;

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

        g_mile = g_mile + time * UPDATA_MPU_TIME * UPDATA_MPU_TIME * averAccel / 2; /* è·¯ç¨‹ç­‰äºŽä¸‰ç»´åŠ é€Ÿåº¦å‘é‡çš„æ¨¡çš„ç§¯åˆ?*/
        
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

void fillTxPacket(orderPacket *obj)
{
    txPacket *pkt = (txPacket *)HC12TxBuff;

    pkt->header = obj->header;
    pkt->typeLen = sizeof(txPacket);
    pkt->orderNum = obj->orderNumber;
    pkt->s_sec = obj->startTime.sec;
    pkt->s_min = obj->startTime.min;
    pkt->s_hour = obj->startTime.hour;
    pkt->s_day = obj->startTime.w_date;
    pkt->s_week = obj->startTime.week;
    pkt->s_month = obj->startTime.w_month;
    pkt->s_year = obj->startTime.w_year;
    pkt->e_sec = obj->endTime.sec;
    pkt->e_min = obj->endTime.min;
    pkt->e_hour = obj->endTime.hour;
    pkt->e_day = obj->endTime.w_date;
    pkt->e_week = obj->endTime.week;
    pkt->e_month = obj->endTime.w_month;
    pkt->e_year = obj->endTime.w_year;
    pkt->mile = obj->mile;
    pkt->money = (obj->money >> 8 | obj->money << 8) * 10;
    pkt->dif = obj->dif;
    pkt->end = obj->end;
    HC12TxLength = sizeof(txPacket);
}

#define HEX_TO_DEC_UCHAR(a) (((a) >> 4) * 10 + ((a) & 0x0F))

void getRxPacket(_calendar_obj *cld)
{
    rxCenderPacket *rxPkt = (rxCenderPacket *)(HC12RxBuff + 2);

    cld->hour    = HEX_TO_DEC_UCHAR(rxPkt->hour);
    cld->min     = HEX_TO_DEC_UCHAR(rxPkt->min);
    cld->sec     = HEX_TO_DEC_UCHAR(rxPkt->sec);
    cld->w_year  = HEX_TO_DEC_UCHAR(rxPkt->year);
    cld->w_month = HEX_TO_DEC_UCHAR(rxPkt->month);
    cld->w_date  = HEX_TO_DEC_UCHAR(rxPkt->day);
    cld->week    = 0;
}

void updataBlueToothStatus()
{
    static u8 index = 0;
    static u8 flag = 1;
    _calendar_obj cld;

    if (HC12RxFlag) {

        if (flag) {
            getRxPacket(&cld);
            DS1307_SetRtc((u8 *)&cld);
            flag = 0;
        }
    
        if (index >= g_pageNum) {
            HC12RxFlag = 0;
            flag = 1;
            index = 0;
        } else {
            fillTxPacket(g_orderData + index);
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
    g_time += timeDif;
    money = GET_MONEY(timeDif, g_mile);
    sprintf(g_strOrder, "#:%05d", g_pageNum);
    sprintf(g_strMoney, " %3.1f$", money / 10.0);
    sprintf(g_strMile, "M:%4.1fkm", g_mile / 10000.0);
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
#ifdef DEBUG
		if (g_time > 1) {
#else
		if (g_time > 4 * 60) {
#endif
			LCD_Clear();
			LCD_Display_Words(0, 0, "ÄúÒÔÁ¬Ðø¹¤×÷4h!");
			LCD_Display_Words(0, 1, "Çë¾¡¿ìÐÝÏ¢!");
			while(1);
			LCD_Clear();
		}
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

