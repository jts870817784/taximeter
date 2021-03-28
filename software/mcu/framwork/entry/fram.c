#include "stdio.h"
#include "timer_triger.h"
#include "key.h"

/* 触发任务间隔，单位ms */
#define UPDATA_MPU_TIME        6
#define UPDATA_LCD_TIME        20
#define UPDATA_BLUE_TOOTH_TIME 2
#define UPDATA_KEY_TTIME       10

#define UPDATA_TASK_TIME       2
#define MENU_TASK_TIME         2

typedef enum tagRunStatus {
    RUNNING = 0,
    IDLE
} runStatus;

u8 runStatus;

void updataMpu()
{
    ;
}

void updataLcd()
{
    ;
}

void updataRtc()
{
    ;
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

void menuTask()
{
    ;
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






