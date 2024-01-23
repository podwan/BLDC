#ifndef __FOC_H
#define __FOC_H

#include "common.h"

#define POLE_PAIRS 4
#define PWM_PERIOD 8000.0f // PWM period * 2
#define U_DC 24.0f
#define uQ_MAX (U_DC / _SQRT3)
#define SPEED_KP 0.3389f
#define SPEED_KI 0.0144f
#define I_MAX 3

#define PWM_GENERATE(p1, p2, p3) \
    {                            \
        TIM1->CCR1 = p1;         \
        TIM1->CCR2 = p2;         \
        TIM1->CCR3 = p3;         \
    }

typedef enum
{
    MOTOR_STOP,
    OPEN_LOOP,
    CLOSE_LOOP,
} MotorMode;

// void revParkOperate(float uD, float uQ, float theta, float *uAlpha, float *uBeta);
// char getSector(float uAlpha, float uBeta);
// void SVPWM(char sector, float uAlpha, float uBeta);
void clarke(float iA, float iB, float iC, float *iAlpha, float *iBeta);
void openLoop(float uQ, uint speed, float frequence, uchar polePairs);
void closeSpeedLoop(float currentSpeed, float setSpeed, float theta, float iA, float iB, float iC, float frequence);
#endif
