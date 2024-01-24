

#ifndef MYPROJECT_H
#define MYPROJECT_H

/* Includes ------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include "usart2.h"
#include "delay.h"
#include "timer.h"
#include "i2c.h"
#include "spi3.h"

#include "foc_utils.h"
#include "MagneticSensor.h"
#include "Encoder.h"
#include "Sensor.h"
#include "BLDCMotor.h"
#include "FOCMotor.h"
#include "lowpass_filter.h"
#include "pid.h"

#define M0_Disable   TIM_Cmd(TIM1, DISABLE);        //关闭M0输出

//SPI接口用GPIO1做为CS引脚，M0的A/B为SCL0/SDA0
//设置使用的编码器为1，不使用的为0
#define M0_AS5600    1   //编码器类型，只能选一
#define M0_AS5047P   0
#define M0_TLE5012B  0
#define M0_MA730     0
#define M0_MT6701    0
#define M0_ABZ       0      //ABZ需要设置下面的CPR。当前代码只支持AB信号不支持Z信号。第5节的代码增加了ABZ的支持。
#define M0ABZ_CPR    4000   //需要准确设置ABZ的CPR。AS5047P=4000，TLE5012B=16384

#endif

