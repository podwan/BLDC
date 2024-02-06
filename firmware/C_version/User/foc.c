#include "foc.h"
#include "bldcMotor.h"
#include "math_utils.h"

DQVoltage_s voltage;
DQCurrent_s current;

/******************************************************************************/

void clarke(float iA, float iB, float iC, float *iAlpha, float *iBeta)
{
    *iAlpha = (2 / 3) * (iA - iB / 2 - iC / 2);
    *iBeta = (1 / SQRT(3)) * (iB - iC);
}

void park(float iAlpha, float iBeta, float theta, float *iD, float *iQ)
{
    *iD = iAlpha * CosApprox(theta) + iBeta * SinApprox(theta);
    *iQ = -iAlpha * SinApprox(theta) + iBeta * CosApprox(theta);
}

void revParkOperate(float uD, float uQ, float theta, float *uAlpha, float *uBeta)
{
    *uAlpha = uD * CosApprox(theta) - uQ * SinApprox(theta);
    *uBeta = uD * SinApprox(theta) + uQ * CosApprox(theta);
}

const char sectorRemap[] = {0, 2, 6, 1, 4, 3, 5};

void SVPWM(float uAlpha, float uBeta)
{
    uchar sector;
    static float A, B, C;
    A = uBeta;
    B = _SQRT3 / 2.0f * uAlpha - uBeta / 2.0f;
    C = -_SQRT3 / 2.0f * uAlpha - uBeta / 2.0f;

    char N = 0;

    if (A > 0)
        N++;

    if (B > 0)
        N += 2;

    if (C > 0)
        N += 4;

    sector = sectorRemap[N];

    float X = _SQRT3 * PWM_PERIOD / U_DC * uBeta;
    float Y = _SQRT3 * PWM_PERIOD / U_DC * (_SQRT3 * uAlpha / 2.0f + uBeta / 2.0f);
    float Z = _SQRT3 * PWM_PERIOD / U_DC * (-_SQRT3 * uAlpha / 2.0f + uBeta / 2.0f);
    float tFirst = 0, tSecond = 0;
    switch (sector)
    {
    case 1:
        tFirst = -Z;
        tSecond = X;
        break;
    case 2:
        tFirst = Z;
        tSecond = Y;
        break;
    case 3:
        tFirst = X;
        tSecond = -Y;
        break;
    case 4:
        tFirst = -X;
        tSecond = Z;
        break;

    case 5:
        tFirst = -Y;
        tSecond = -Z;
        break;

    case 6:
        tFirst = Y;
        tSecond = -X;
        break;
    }

    float t = tFirst + tSecond;

    if (t > PWM_PERIOD)
    {
        tFirst = tFirst / t * PWM_PERIOD;
        tSecond = tSecond / t * PWM_PERIOD;
    }

    int v1 = (PWM_PERIOD - tFirst - tSecond) / 2.0f;
    int v2 = v1 + tFirst;
    int v3 = v2 + tSecond;

    int pwm1Duty, pwm2Duty, pwm3Duty;

    switch (sector)
    {
    case 1:
        pwm1Duty = v1;
        pwm2Duty = v2;
        pwm3Duty = v3;
        break;

    case 2:
        pwm1Duty = v2;
        pwm2Duty = v1;
        pwm3Duty = v3;
        break;

    case 3:
        pwm1Duty = v3;
        pwm2Duty = v1;
        pwm3Duty = v2;
        break;

    case 4:
        pwm1Duty = v3;
        pwm2Duty = v2;
        pwm3Duty = v1;
        break;

    case 5:
        pwm1Duty = v2;
        pwm2Duty = v3;
        pwm3Duty = v1;
        break;

    case 6:
        pwm1Duty = v1;
        pwm2Duty = v3;
        pwm3Duty = v2;
        break;
    }
    //    temp[2] = pwm1Duty;
    //    temp[3] = pwm2Duty;
    //    temp[4] = pwm3Duty;

    PWM_GENERATE(pwm1Duty, pwm2Duty, pwm3Duty);
}
#if 1
void setPhaseVoltage(float Uq, float Ud, float angle_el)
{
    Uq = CONSTRAINT(Uq, 0, uQ_MAX);

    angle_el = _normalizeAngle(angle_el);
    float uAlpha, uBeta;

    revParkOperate(Ud, Uq, angle_el, &uAlpha, &uBeta);
    SVPWM(uAlpha, uBeta);
}
#else
void setPhaseVoltage(float Uq, float Ud, float angle_el)
{
    float Uout;
    uint32_t sector;
    float T0, T1, T2;
    float Ta, Tb, Tc;

    if (Ud) // only if Ud and Uq set
    {       // SQRT is an approx of sqrt (3-4% error)
        Uout = SQRT(Ud * Ud + Uq * Uq) / voltage_power_supply;
        // angle normalisation in between 0 and 2pi
        // only necessary if using SinApprox and _cos - approximation functions
        angle_el = _normalizeAngle(angle_el + atan2(Uq, Ud));
    }
    else
    { // only Uq available - no need for atan2 and sqrt
        Uout = Uq / voltage_power_supply;
        // angle normalisation in between 0 and 2pi
        // only necessary if using SinApprox and _cos - approximation functions
        angle_el = _normalizeAngle(angle_el + _PI_2);
    }

    sector = (angle_el / _PI_3) + 1;
    T1 = _SQRT3 * SinApprox(sector * _PI_3 - angle_el) * Uout;
    T2 = _SQRT3 * SinApprox(angle_el - (sector - 1.0f) * _PI_3) * Uout;
    T0 = 1 - T1 - T2;

    // calculate the duty cycles(times)
    switch (sector)
    {
    case 1:
        Ta = T1 + T2 + T0 / 2;
        Tb = T2 + T0 / 2;
        Tc = T0 / 2;
        break;
    case 2:
        Ta = T1 + T0 / 2;
        Tb = T1 + T2 + T0 / 2;
        Tc = T0 / 2;
        break;
    case 3:
        Ta = T0 / 2;
        Tb = T1 + T2 + T0 / 2;
        Tc = T2 + T0 / 2;
        break;
    case 4:
        Ta = T0 / 2;
        Tb = T1 + T0 / 2;
        Tc = T1 + T2 + T0 / 2;
        break;
    case 5:
        Ta = T2 + T0 / 2;
        Tb = T0 / 2;
        Tc = T1 + T2 + T0 / 2;
        break;
    case 6:
        Ta = T1 + T2 + T0 / 2;
        Tb = T0 / 2;
        Tc = T1 + T0 / 2;
        break;
    default: // possible error state
        Ta = 0;
        Tb = 0;
        Tc = 0;
    }

    // TIM_SetCompare1(TIM1, Ta * PWM_Period);
    // TIM_SetCompare2(TIM1, Tb * PWM_Period);
    // TIM_SetCompare3(TIM1, Tc * PWM_Period);

    PWM_GENERATE(Ta * PWM_PERIOD, Tb * PWM_PERIOD, Tc * PWM_PERIOD);
}
#endif
 