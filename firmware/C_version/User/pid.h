#ifndef __PID_H
#define __PID_H

#include "userMain.h"

typedef struct
{
    float P;
    float I;
    float D;
    float iTerm;
    float outputRamp;
    float output_prev;
    float error_prev;
    float outMax;
    float outMin;
    float timestamp_prev;
} PID;
extern PID velocityPID, P_angle;
void pidInit(PID *pid, float kp, float ki, float kd, float outputRamp, float outMax, float outMin);
float pidCompute(PID *pid, float error);
#endif
