#ifndef __SENSOR_H
#define __SENSOR_H

#include "userMain.h"

#define DELAY(i) HAL_Delay(i)

typedef struct
{
    float voltageToAmpRatio;
    float zeroOffsetU;
    float zeroOffsetV;
    float zeroOffsetW;
    float currentU;
    float currentV;
    float currentW;
} CurrentSensor;
// ADC_CHANNEL
#define CH_U 1
#define CH_V 2
#define CH_W 3

#define SHUNT_RESISTOR 0.001
#define AMP_GAIN 50

void initCurrentSensor(CurrentSensor *cs);
void getPhaseCurrents( CurrentSensor *cs, uchar sector);
float getElectricalAngle(float shaft_angle, int pole_pairs);
uchar getSector(float electricAngle);
float normalizeAngle(float angle);
#endif
