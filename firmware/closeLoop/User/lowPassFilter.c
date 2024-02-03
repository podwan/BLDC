
#include "lowPassFilter.h"
LowPassFilter LPF_current_q, LPF_current_d, LPF_velocity;
LowPassFilter lpfAngle;


void lowPassFilterInit(LowPassFilter *filter, const float timeConstant)
{
    filter->timeConstant = timeConstant;
    filter->outputLast = 0;
    filter->timeStamp = micros();
}

float lowPassFiltering(LowPassFilter *filter, float input)
{
    unsigned long time = micros();
    float dt = ((float)time - (float)filter->timeStamp) * 1e-6f;
    filter->timeStamp = time;
    if (dt < 0.0f)
        dt = 1e-3f;
    else if (dt > 0.3f)
    {
        filter->outputLast = input;
        return input;
    }
    float alpha = filter->timeConstant / (filter->timeConstant + dt);
    float output = alpha * filter->outputLast + (1.0f - alpha) * input;
    filter->outputLast = output;
    return output;
}
