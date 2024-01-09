#include "sensor.h"

bool adc_isr_flag = 0;
bool opa_cali_flag = 0;

float positionEstimate(float uD, float uQ, float iD, float iQ)
{
    float omega = (-uD + Rq * iD) / (Lq * iQ);
    return omega;
}