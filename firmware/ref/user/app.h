#ifndef __APP_H
#define __APP_H
#include "userMain.h"

// #define V_LIMIT 12

#define POLE_PAIRS 4

#define SET_PWM(DUTY_U, DUTY_V, DUTY_W) \
    {                                   \
        TIM1->CCR1 = DUTY_U;            \
        TIM1->CCR2 = DUTY_V;            \
        TIM1->CCR3 = DUTY_W;            \
    }

#define _constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

typedef enum
{
    VELOCITYF_OPEN_LOOP,
} WorkMode;

void appRunning(void *argument);
extern float voltagePowerSupply;
#endif
