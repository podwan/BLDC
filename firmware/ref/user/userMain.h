#ifndef __USERMAIN_H
#define __USERMAIN_H

#include "stm32g4xx_hal.h"
#include "stm32g4xx_ll_rtc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "tim.h"
#include "i2c.h"
#include "spi.h"
#include <stdint.h>

typedef unsigned int uint;
typedef unsigned char uchar;

void userMain();
void OnTimerCallback();
#endif
