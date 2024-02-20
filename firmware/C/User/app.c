
#include "app.h"
#include "led.h"
#include "key.h"

static DevState devState;
static KeyState keyState;
static uchar flashCnt;

static void standingBy()
{
    led1On = 1;

    if (keyState == USER1_SHORT)
    {
        WORK_INIT;
    }
}

static void working(void)
{
    if (flashCnt < 5)
        led2On = 1;

    if (keyState == USER1_SHORT)
    {
        STANDBY_INIT;
    }
}

void appRunning()
{
    getKeyState(&keyState);

    if (++flashCnt >= 10)
        flashCnt = 0;

    led1On = 0;
    led2On = 0;

    switch (devState)
    {
    case STANDBY:
        standingBy();
        break;

    case WORK:
        working();
        break;
    }

    LED_drive();
}

void appRunningLoop(void *argument)
{
    devState = STANDBY;
    for (;;)
    {
        appRunning();
        osDelay(100);
    }
}
