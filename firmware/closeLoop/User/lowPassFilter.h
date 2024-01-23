#ifndef __LOW_PASS_FILTER_H
#define __LOW_PASS_FILTER_H

#include "common.h"

typedef struct
{
    float timeConstant;
    float dt;
    float outputLast;
} LowPassFilter;

void lowPassFilterInit(LowPassFilter *filter, const float timeConstant, const float dt);
float lowPassFiltering(LowPassFilter *filter, float input, float dt);
#endif