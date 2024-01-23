
#include "lowPassFilter.h"

void lowPassFilterInit(LowPassFilter *filter, const float timeConstant, const float dt)
{
    filter->timeConstant = timeConstant;
    filter->dt = dt;
    filter->outputLast = 0;
}

float lowPassFiltering(LowPassFilter *filter, float input, float dt)
{
    float alpha = filter->timeConstant / (filter->timeConstant + filter->dt);
    float output = alpha * filter->outputLast + (1.0f - alpha) * input;
    filter->outputLast = output;
    return output;
}
