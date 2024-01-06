#include "app.h"
#include "foc.h"
#include "sensor.h"
#include "math_utils.h"
#include <math.h>
#include "time_utils.h"

static WorkMode workMode;
static ThreePhaseVoltage _3PhaseVoltage;
float voltage_limit = 3;
float Uq = 3;
float voltage_power_supply = 11;
float shaft_angle = 0, open_loop_timestamp = 0;
float zero_electric_angle = 0, Ualpha, Ubeta = 0, Ua = 0, Ub = 0, Uc = 0, dc_a = 0, dc_b = 0, dc_c = 0;

static void velocityOpenloop(float velocity);

static float electricAngle;
uchar sector;
void setPwm(float Ua, float Ub, float Uc)
{
    float dc_a, dc_b, dc_c;
    // 限制上限
    Ua = _constrain(Ua, 0.0f, voltage_limit);
    Ub = _constrain(Ub, 0.0f, voltage_limit);
    Uc = _constrain(Uc, 0.0f, voltage_limit);
    // 计算占空比
    // 限制占空比从0到1
    dc_a = _constrain(Ua / voltage_power_supply, 0.0f, 1.0f);
    dc_b = _constrain(Ub / voltage_power_supply, 0.0f, 1.0f);
    dc_c = _constrain(Uc / voltage_power_supply, 0.0f, 1.0f);

    SET_PWM(dc_a * PWM_PERIOD, dc_b * PWM_PERIOD, dc_c * PWM_PERIOD);
}
float dc_a, dc_b, dc_c;
float Va, Vb, Ua, Ub, Uc;
void setPhaseVoltage(float Vq, float Vd, float angle_el)
{

    // revPark
    Va = -Vq * SinApprox(angle_el);
    Vb = Vq * CosApprox(angle_el);

    // revClark
    Ua = Va + voltagePowerSupply / 2;
    Ub = (_SQRT3 * Vb - Va) / 2 + voltagePowerSupply / 2;
    Uc = (-Va - _SQRT3 * Vb) / 2 + voltagePowerSupply / 2;

    setPwm(Ua, Ub, Uc);
}

// void setPhaseVoltage(float Uq, float Ud, float angle_el)
// {

//     //  帕克逆变换
//     Ualpha = -Uq * SinApprox(angle_el);
//     Ubeta = Uq * CosApprox(angle_el);

//     // 克拉克逆变换
//     Ua = Ualpha + voltage_power_supply / 2;
//     Ub = (_SQRT3 * Ubeta - Ualpha) / 2 + voltage_power_supply / 2;
//     Uc = (-Ualpha - _SQRT3 * Ubeta) / 2 + voltage_power_supply / 2;
//     setPwm(Ua, Ub, Uc);
// }
uint64_t openLoopTimestamp;
float phaseResistance = NOT_SET;
float estimateVelocity;
float estimateAngle;
int polePairs;
float voltageLimit;
float currentLimit;
float velocityLimit;
float voltagePowerSupply;
// call per 1ms
void appRunning(void *argument)
{
    // voltage_power_supply = 11;
    voltage_limit = 12;
    voltageLimit = 12.0f;
    currentLimit = 0.2f;
    velocityLimit = 20.0f;
    voltagePowerSupply = 12;
    // polePairs = 4;
    while (1)
    {
        // switch (workMode)
        {
            // case VELOCITYF_OPEN_LOOP:
            velocityOpenloop(10);
            //   break;
        }
         osDelay(1);
    }
}

// velocity:rad/s
uint64_t t;
float deltaT;
float voltageQ;
float electricAngle;
uchar sector;
static void velocityOpenloop(float velocity)
{

    t = micros();

    deltaT = (float)(t - openLoopTimestamp) * 1e-6f;

    if (deltaT <= 0 || deltaT > 0.5f)
        deltaT = 1e-3f;

    estimateAngle = normalizeAngle(estimateAngle + velocity * deltaT);

    electricAngle = getElectricalAngle(estimateAngle, POLE_PAIRS);

    voltageQ = ASSERT(phaseResistance) ? currentLimit * phaseResistance : voltageLimit;

    //  SetPhaseVoltage(voltageQ, 0, electricAngle);

    // SVPWM(electricAngle, 6, 0);

    setPhaseVoltage(6, 0, electricAngle);

    openLoopTimestamp = t;
}

// shaft_angle = normalizeAngle(shaft_angle + velocity * 0.001);

// electricAngle = getElectricalAngle(shaft_angle, 4);
// // revPark(electricAngle, &Va, &Vb, 3, 0);
// // revClark(Va, Vb, &_3PhaseVoltage);

// setPhaseVoltage(5, 0, electricAngle);