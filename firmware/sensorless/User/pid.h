#ifndef __PID_H
#define __PID_H

#include "common.h"

typedef struct
{
    float kp;
    float ki;
    float kd;
    float iTerms;
    float lastInput;
    char positiveFeedback;
    float outMax;
    float outMin;
} PID;

void pidInit(PID *pid, float kp, float ki, float kd, float iTerms, float lastInput,
             bool positiveFeedback, float outMax, float outMin);
unsigned int compute(PID *pid, float setPoint, float input);
void setTunings(PID *pid, float Kp, float Ki, float Kd);
#endif