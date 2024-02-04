#ifndef __FOC_H
#define __FOC_H

#include "userMain.h"

#define PWM_PERIOD 8000.0f
#define U_DC 12.0f
#define uQ_MAX (U_DC / _SQRT3)

#define PWM_GENERATE(p1, p2, p3) \
    {                            \
        TIM1->CCR1 = p1;         \
        TIM1->CCR2 = p2;         \
        TIM1->CCR3 = p3;         \
    }

/******************************************************************************/
// dq current structure
typedef struct
{
    float d;
    float q;
} DQCurrent_s;
// phase current structure
typedef struct
{
    float a;
    float b;
    float c;
} PhaseCurrent_s;
// dq voltage structs
typedef struct
{
    float d;
    float q;
} DQVoltage_s;

extern DQVoltage_s voltage;
extern DQCurrent_s current;

/******************************************************************************/
float shaftAngle(void);
float shaftVelocity(void);
float electricalAngle(void);
/******************************************************************************/

void closeAngleLoop(float targetAngle);
void clarke(float iA, float iB, float iC, float *iAlpha, float *iBeta);
void closeSpeedLoop(float currentSpeed, float setSpeed, float theta, float iA, float iB, float iC, float frequence);
void loopFOC(void);
void setPhaseVoltage(float Uq, float Ud, float angle_el);
#endif
