#include "foc.h"
// #include "arm_math.h"
#include "math_utils.h"
#include <cmath>
#include "pid.h"

float i, j;

void clarke(float iA, float iB, float iC, float *iAlpha, float *iBeta)
{
    // iAlpha = (2/3) * (iA - iB / 2 - iC/2);
    *iAlpha = 0.6666667f * (iA - 0.5f * (iB + iC));
    // iBeta = (1/sqrt(3)) * (iB - iC);
    *iBeta = 0.5773502691f * (iB - iC);
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

static float A, B, C;

char getSector(float uAlpha, float uBeta)
{
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

    return sectorRemap[N];
}

void SVPWM(char sector, float uAlpha, float uBeta)
{
    float tFirst = 0, tSecond = 0;

    float X = _SQRT3 * PWM_PERIOD * uBeta / U_DC;
    float Y = _SQRT3 * PWM_PERIOD / U_DC * (_SQRT3 * uAlpha / 2.0f + uBeta / 2.0f);
    float Z = _SQRT3 * PWM_PERIOD / U_DC * (-_SQRT3 * uAlpha / 2.0f + uBeta / 2.0f);

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
    temp[2] = pwm1Duty;
    temp[3] = pwm2Duty;
    temp[4] = pwm3Duty;

    // PWM_GENERATE(pwm1Duty, pwm2Duty, pwm3Duty);
    PWM_GENERATE(pwm1Duty, pwm2Duty, pwm3Duty);
}
/*
parameters:
speed at rpm/min at rpm/min
frequence is about the function call frequence
*/

void openSpeedLoop(float uQ, uint speed)
{
    static float theta, thetaAdd;
    thetaAdd = speed * _2PI / 60.0f / FREQUENCE * POLE_PAIRS;
    theta += thetaAdd;

    if (theta > 6.2831852f)
        theta = 0;

    float uAlpha, uBeta;
    uQ = CONSTRAINT(uQ, 0, uQ_MAX);

    revParkOperate(0, uQ, theta, &uAlpha, &uBeta);

    char sector = getSector(uAlpha, uBeta);

    SVPWM(sector, uAlpha, uBeta);
}

void setTorque(float uQ, float angle_el)
{

    uQ = CONSTRAINT(uQ, 0, uQ_MAX);

    angle_el = normalizeAngle(angle_el);
    float uAlpha, uBeta;

    revParkOperate(0, uQ, angle_el, &uAlpha, &uBeta);

    char sector = getSector(uAlpha, uBeta);

    SVPWM(sector, uAlpha, uBeta);
}
// (PID *pid, float kp, float ki, float kd, bool positiveFB, float outMax, float outMin, float interval)
void closeSpeedLoop(float currentSpeed, float setSpeed, float theta, float iA, float iB, float iC, float frequence)
{
    // float iAlpha, iBeta;
    // clarke(iA, iB, iC, &iAlpha, &iBeta);
    //  float iD, iQ;
    // park(iAlpha, iBeta, theta, &iD, &iQ);

    // PID speedPID, currentPID;
    // float IqRef, uQ;
    // pidInit(&speedPID, SPEED_KP, SPEED_KI, 0, 10000, uQ_MAX, -uQ_MAX, 1 / frequence);
    // uQ = pidCompute(&speedPID, setSpeed - currentSpeed);

    // pidInit(&currentPID, SPEED_KP, SPEED_KI, 0, 0, 0, 0, uQ_MAX, 0);
    // uQ = compute(&currentPID, IqRef, iQ);

    // float uAlpha, uBeta;
    // revParkOperate(0, uQ, theta, &uAlpha, &uBeta);

    // char sector = getSector(uAlpha, uBeta);

    // SVPWM(sector, uAlpha, uBeta);
}

void closeSpeedLoopSensorless(float speed, float iA, float iB, float iC)
{
}

void closeAngleLoop()
{
    float Sensor_Angle = as5600GetAngle();
    float Kp = uQ_MAX / 45.0f;
    //setTorque(CONSTRAINT(Kp * (motor_target - DIR * Sensor_Angle) * 180 / _PI, 0, uQ_MAX), _electricalAngle());
}