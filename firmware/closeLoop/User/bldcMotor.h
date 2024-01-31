#ifndef __BLDC_MOTOR_H
#define __BLDC_MOTOR_H

#include "userMain.h"

#define M0_Disable HAL_TIM_Base_Stop(&htim1)
/******************************************************************************/


extern float target;
extern float voltage_sensor_align;
extern float voltage_power_supply;
extern float shaft_velocity;

void motorInit(void);
void move(float new_target);
#endif
