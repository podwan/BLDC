#ifndef __USERMAIN_H
#define __USERMAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32g4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os2.h"

    void userMain();
#ifdef __cplusplus
}
#endif

#endif