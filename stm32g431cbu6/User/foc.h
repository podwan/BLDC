#ifndef __FOC_H
#define __FOC_H
#include "main.h"
#define PWM_PERIOD 8000 // PWM period
#define U_DC 24

#define PWM_GENERATE(p1, p2, p3) \
    {                            \
        TIM1->CCR1 = p1;         \
        TIM1->CCR2 = p2;         \
        TIM1->CCR3 = p3;         \
    }

#endif