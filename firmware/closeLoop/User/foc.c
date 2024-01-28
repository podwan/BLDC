#include "foc.h"
#include "bldcMotor.h"
PID velocityPID;
/******************************************************************************/
float shaft_angle; //!< current motor angle
float electrical_angle;

float current_sp;
float shaft_velocity_sp;
float shaft_angle_sp;
DQVoltage_s voltage;
DQCurrent_s current;

TorqueControlType torque_controller;
MotionControlType controller;

float sensor_offset = 0; // 似乎没用
float zero_electric_angle;
/******************************************************************************/
// shaft angle calculation
float shaftAngle(void)
{
    // if no sensor linked return previous value ( for open loop )
    // if(!sensor) return shaft_angle;
    return SENSOR_DIRECTION * getAngle() - sensor_offset;
}
// shaft velocity calculation
float shaftVelocity(void)
{
    // if no sensor linked return previous value ( for open loop )
    // if(!sensor) return shaft_velocity;
    return SENSOR_DIRECTION * lowPassFiltering(&LPF_velocity, getVelocity());
    // return sensor_direction*getVelocity();
}
/******************************************************************************/
float electricalAngle(void)
{
    return normalizeAngle((shaft_angle + sensor_offset) * POLE_PAIRS - zero_electric_angle);
}
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
    temp[2] = pwm1Duty;
    temp[3] = pwm2Duty;
    temp[4] = pwm3Duty;

    // PWM_GENERATE(pwm1Duty, pwm2Duty, pwm3Duty);
    PWM_GENERATE(pwm1Duty, pwm2Duty, pwm3Duty);
}

void setTorque(float uQ, float angle_el)
{

    uQ = CONSTRAINT(uQ, 0, uQ_MAX);

    angle_el = normalizeAngle(angle_el);
    float uAlpha, uBeta;

    revParkOperate(0, uQ, angle_el, &uAlpha, &uBeta);

    SVPWM(uAlpha, uBeta);
}
void openSpeedLoop(float uQ, uint speed)
{
    static float angle_el, thetaAdd;
    thetaAdd = speed * _2PI / 60.0f / FREQUENCE * POLE_PAIRS;
    angle_el += thetaAdd;

    angle_el = normalizeAngle(angle_el);

    setTorque(uQ, angle_el);
}

void setPhaseVoltage(float Uq, float Ud, float angle_el)
{
    Uq = CONSTRAINT(Uq, 0, uQ_MAX);

    angle_el = normalizeAngle(angle_el);
    float uAlpha, uBeta;

    revParkOperate(Ud, Uq, angle_el, &uAlpha, &uBeta);
    SVPWM(uAlpha, uBeta);
}
/******************************************************************************/
void loopFOC(void)
{
    if (controller == Type_angle_openloop || controller == Type_velocity_openloop)
        return;

    shaft_angle = shaftAngle();           // shaft angle
    electrical_angle = electricalAngle(); // electrical angle - need shaftAngle to be called first

    switch (torque_controller)
    {
    case Type_voltage: // no need to do anything really
        break;
    case Type_dc_current:
        break;
    case Type_foc_current:
        break;
    default:
        printf("MOT: no torque control selected!\r\n");
        break;
    }
    // set the phase voltage - FOC heart function :)
    setPhaseVoltage(voltage.q, voltage.d, electrical_angle);
}
