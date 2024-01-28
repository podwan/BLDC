#ifndef __BLDC_MOTOR_H
#define __BLDC_MOTOR_H

#include "common.h"

extern LowPassFilter LPF_velocity;
void move(float new_target);
#endif
