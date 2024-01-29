#ifndef __BLDC_MOTOR_H
#define __BLDC_MOTOR_H

#include "common.h"
#define M0_Disable HAL_TIM_Base_Stop(&htim1)
/******************************************************************************/

extern LowPassFilter LPF_velocity;
extern float target;
extern float voltage_sensor_align;
void motorInit(void);
void Motor_initFOC(float zero_electric_offset, Direction _sensor_direction);
void move(float new_target);
float velocityOpenloop(float target_velocity);
#endif
