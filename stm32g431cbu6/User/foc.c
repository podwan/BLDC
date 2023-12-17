#include "foc.h"
#include "arm_math.h"

void revParkOperate(float uD, float uQ, float theta, float *vAplha, float *vBeta)
{
    *vAplha = uD * arm_cos_f32(theta) - uQ * arm_sin_f32(theta);
    *vBeta = uD * arm_sin_f32(theta) + uQ * arm_cos_f32(theta);
}

const char sectorRemap[] = {0, 2, 6, 1, 4, 3, 5};

char getSector(float uAlpha, float uBeta)
{
    float u1 = uBeta;
    float u2 = sqrt(3) / 2 * uAlpha - 0.5f * uBeta;
    float u3 = -sqrt(3) / 2 * uAlpha - 0.5f * uBeta;

    char N = 0;
    if (u1 > 0)
        N++;

    if (u2 > 0)
        N += 2;

    if (u3 > 0)
        N += 4;

    return sectorRemap[N];
}

void SVPWM(char sector, float uAlpha, float uBeta, float period, char Udc)
{
    float tFirst = 0, tSecond = 0;

    float X = sqrt(3) * period * uBeta / U_DC;
    float Y = sqrt(3) * period / U_DC * (sqrt(3) * uAlpha / 2 + uBeta / 2);
    float Z = sqrt(3) * period / U_DC * (-sqrt(3) * uAlpha / 2 + uBeta / 2);

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

    if (t > period)
    {
        tFirst = tFirst / t * period;
        tSecond = tSecond / t * period;
    }

    int v1 = (period - tFirst - tSecond) / 4;
    int v2 = v1 + tFirst / 2;
    int v3 = v2 + tSecond / 2;

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

    PWM_GENERATE(pwm1Duty, pwm2Duty, pwm3Duty);
}
