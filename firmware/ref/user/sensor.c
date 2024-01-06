#include "sensor.h"
#include "math_utils.h"

// CS
static float getAdcToVoltage(char _channel)
{
    // return (float)rawAdcVal[_channel] / 4096 * 3.3f;
}

void initCurrentSensor(CurrentSensor *cs)
{
    float voltageToAmpRatio, zeroOffsetU, zeroOffsetV, zeroOffsetW;

    voltageToAmpRatio = 1.0f / SHUNT_RESISTOR / AMP_GAIN; // volts to amps

    cs->voltageToAmpRatio = voltageToAmpRatio;

    for (int i = 0; i < 1000; i++)
    {
        zeroOffsetU += getAdcToVoltage(CH_U);
        zeroOffsetV += getAdcToVoltage(CH_V);
        zeroOffsetW += getAdcToVoltage(CH_W);
        DELAY(1);
    }

    cs->zeroOffsetU = zeroOffsetU / 1000;
    cs->zeroOffsetV = zeroOffsetV / 1000;
    cs->zeroOffsetW = zeroOffsetW / 1000;
}
// call when pwm counter reach the max
void getPhaseCurrents(CurrentSensor *cs, uchar sector)
{
    float currentU, currentV, currentW;

    currentU = (getAdcToVoltage(CH_U) - cs->zeroOffsetU) * cs->voltageToAmpRatio; // Amps
    currentV = (getAdcToVoltage(CH_V) - cs->zeroOffsetV) * cs->voltageToAmpRatio; // Amps
    currentW = (getAdcToVoltage(CH_W) - cs->zeroOffsetW) * cs->voltageToAmpRatio; // Amps

    switch (sector)
    {
    case 1:
    case 6:
        cs->currentU = currentV + currentW;
        cs->currentV = currentV;
        cs->currentW = currentW;
        break;
    case 2:
    case 3:
        cs->currentU = currentU;
        cs->currentV = currentU + currentW;
        cs->currentW = currentW;
        break;
    case 4:
    case 5:
        cs->currentU = currentU;
        cs->currentV = currentV;
        cs->currentW = currentU + currentV;
        break;
    }
}

// ANGLE
float getElectricalAngle(float shaft_angle, int pole_pairs)
{

    // If no sensor linked return previous value (for open-loop case)
    // return encoder ? normalizeAngle((float)(encoder->countDirection * polePairs) * encoder->GetLapAngle() - zeroElectricAngleOffset)
    //                : electricalAngle;

    return normalizeAngle(shaft_angle * pole_pairs);
}

uchar getSector(float electricAngle)
{
    return electricAngle / _PI_3 + 1;
}
