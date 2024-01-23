#ifndef __PID_H
#define __PID_H

#include "common.h"

typedef struct
{
    float kp;
    float ki;
    float kd;
    float iTerm;
    float outputRamp;
    float outputLast;
    float errorLast;
    float outMax;
    float outMin;
    float dt;
} PID;

void pidInit(PID *pid, float kp, float ki, float kd, float outputRamp, float outMax, float outMin, float dt);
float pidCompute(PID *pid, float error);
void setTunings(PID *pid, float Kp, float Ki, float Kd);
#endif