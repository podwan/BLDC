#ifndef __COMMON_H
#define __COMMON_H
#include "stm32g4xx_hal.h"
#include "math_utils.h"
#include "main.h"
#include <stdlib.h>

#define POLE_PAIRS 7
#define PWM_PERIOD 8000.0f // PWM period * 2
#define U_DC 12.0f
#define uQ_MAX (U_DC / _SQRT3)
#define I_MAX 3         // 最大电流
#define FREQUENCE 10000 // FOC compute frequency which is current sampling frequency

typedef unsigned int uint;
typedef unsigned char uchar;

typedef enum
{
    false,
    true
} bool;

#endif