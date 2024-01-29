#ifndef __COMMON_H
#define __COMMON_H
#include "stm32g4xx_hal.h"
#include "math_utils.h"
#include "time_utils.h"
#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include "lowPassFilter.h"
#include "pid.h"
#include "sensor.h"
#include "tim.h"
#include <math.h>
#include "foc.h"

#define PWM_PERIOD 8000.0f // PWM period * 2
#define U_DC 12.0f
#define uQ_MAX (U_DC / _SQRT3)
#define I_MAX 3         // 最大电流
#define FREQUENCE 10000 // FOC compute frequency which is current sampling frequency

// 设置使用的编码器为1，不使用的为0
#define AS5600 1 // 编码器类型，只能选一
#define AS5047P 0
#define TLE5012B 0
#define MA730 0
#define MT6701 0

typedef unsigned int uint;
typedef unsigned char uchar;

typedef enum
{
    false,
    true
} bool;

#endif
