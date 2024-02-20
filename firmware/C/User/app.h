#ifndef __APP_H
#define __APP_H

#include "userMain.h"

typedef enum
{
    POWER_ON,
    POWER_OFF,
    STANDBY,
    WORK,
    CLEAN,
    TEST,
    FAULT,
    VERSION
} DevState;

#define WORK_INIT        \
    {                    \
        devState = WORK; \
        flashCnt = 0;    \
    }

#define STANDBY_INIT        \
    {                       \
        devState = STANDBY; \
        flashCnt = 0;       \
    }

void appRunningLoop(void *argument);

#endif
