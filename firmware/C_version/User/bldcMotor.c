#include "bldcMotor.h"
#include "foc.h"
#include "lowPassFilter.h"
#include "math_utils.h"
#include "pid.h"
#include "sensor.h"
#include "time_utils.h"

float voltage_power_supply;
float voltage_limit;
float voltageUsedForSensorAlign;
float velocity_limit;
float current_limit;
float shaftVelocity;
unsigned long open_loop_timestamp;
float target;
/******************************************************************************/
float shaftAngle; //!< current motor angle
float electricalAngle;

float setPointCurrent;
float setPointVelocity;
float setPointAngle;
/******************************************************************************/
static float velocityOpenloop(float target_velocity);
static float angleOpenloop(float target_angle);
/******************************************************************************/

TorqueControlType torque_controller;
MotionControlType controller;

void motorInit(void)
{
    lowPassFilterInit(&LPF_velocity, 0.02f);

    lowPassFilterInit(&lpfAngle, 0.03f);

    voltageUsedForSensorAlign = 2; // V 航模电机设置的值小一点比如0.5-1，云台电机设置的大一点比如2-3

    torque_controller = Type_voltage; // 当前只有电压模式
    controller = VELOCITY_OPEN_LOOP;  // ANGLE; //Type_torque; //VELOCITY
                                      // 0.2, 速度环PI参数，只用P参数方便快速调试
    voltage_limit = uQ_MAX;           // V，主要为限制电机最大电流，最大值需小于12/1.732=6.9
    pidInit(&velocityPID, 0.1, 1, 0, 100, uQ_MAX, 0);
    // 速度爬升斜率，如果不需要可以设置为0

    target = 0;

    voltage_power_supply = 12;
    velocity_limit = 20; // rad/s 角度模式时限制最大转速，力矩模式和速度模式不起作用
    pidInit(&P_angle, 20, 0, 0, 0, velocity_limit, 0);
    // velocity control loop controls current
    // 如果是电压模式限制电压，如果是电流模式限制电流
    if (torque_controller == Type_voltage)
        velocityPID.outMax = voltage_limit; // 速度模式的电流限制
    else
        velocityPID.outMax = current_limit;

    if (voltageUsedForSensorAlign > voltage_limit)
        voltageUsedForSensorAlign = voltage_limit;

    MagneticSensor_Init(0, UNKNOWN);

    open_loop_timestamp = micros();
}

void move()
{

    shaftVelocity = getShaftVelocity();
    if (controller == VELOCITY_OPEN_LOOP ||
        controller == ANGLE_OPEN_LOOP)
    {
        shaftAngle = getShaftAngle();
    }
    switch (controller)
    {
    // case Type_torque:
    //     if (torque_controller == Type_voltage)
    //         voltage.q = new_target; // if voltage torque control
    //     else
    //         setPointCurrent = new_target; // if current/foc_current torque control
    //     break;
    case ANGLE:
        // angle set point
        setPointAngle = target;
        // calculate velocity set point
        setPointVelocity = pidCompute(&P_angle, (setPointAngle - shaftAngle));
        // calculate the torque command
        setPointCurrent = pidCompute(&velocityPID, (setPointVelocity - shaftVelocity)); // if voltage torque control
        // if torque controlled through voltage
        break;
    case VELOCITY:
        setPointVelocity = target;
        // calculate the torque command
        setPointCurrent = pidCompute(&velocityPID, (setPointVelocity - shaftVelocity));
        break;
    case VELOCITY_OPEN_LOOP:
        setPointVelocity = target;
        voltage.q = velocityOpenloop(setPointVelocity);
        voltage.d = 0;
        break;
    case ANGLE_OPEN_LOOP:
        setPointAngle = target;
        voltage.q = angleOpenloop(setPointAngle);
        voltage.d = 0;
        break;
    }
}

/******************************************************************************/
void loopFOC(void)
{

    if (controller == VELOCITY_OPEN_LOOP ||
        controller == ANGLE_OPEN_LOOP)
    {
        return;
    }

    shaftAngle = getShaftAngle();           // shaft angle
    electricalAngle = getElectricalAngle(); // electrical angle - need shaftAngle to be called first

    voltage.q = setPointCurrent; // use voltage if phase-resistance not provided
    voltage.d = 0;
    // set the phase voltage - FOC heart function :)
    setPhaseVoltage(voltage.q, voltage.d, electricalAngle);
}

/******************************************************************************/
static float velocityOpenloop(float target_velocity)
{
    unsigned long now_us;
    float Ts, Uq;

    now_us = micros();                           // get current timestamp
    Ts = (now_us - open_loop_timestamp) * 1e-6f; // calculate the sample time from last call
    if (Ts <= 0 || Ts > 0.5f)
        Ts = 1e-3f;               // quick fix for strange cases (micros overflow + timestamp not defined)
    open_loop_timestamp = now_us; // save timestamp for next call

    // calculate the necessary angle to achieve target velocity
    // shaftAngle = _normalizeAngle(shaftAngle + target_velocity * Ts);
    // for display purposes
    // shaftVelocity = target_velocity;

    Uq = voltageUsedForSensorAlign;
    // set the maximal allowed voltage (voltage_limit) with the necessary angle
    //   setPhaseVoltage(Uq, 0, getElectricalAngle());
    setPhaseVoltage(Uq, 0, zero_electric_angle);
    return Uq;
}

/******************************************************************************/
static float angleOpenloop(float target_angle)
{
    unsigned long now_us;
    float Ts, Uq;

    now_us = micros();                           // get current timestamp
    Ts = (now_us - open_loop_timestamp) * 1e-6f; // calculate the sample time from last call
    if (Ts <= 0 || Ts > 0.5f)
        Ts = 1e-3f;               // quick fix for strange cases (micros overflow + timestamp not defined)
    open_loop_timestamp = now_us; // save timestamp for next call

    // calculate the necessary angle to move from current position towards target angle
    // with maximal velocity (velocity_limit)
    if (fabs(target_angle - shaftAngle) > velocity_limit * Ts)
    {
        shaftAngle += SIGN(target_angle - shaftAngle) * velocity_limit * Ts;
        shaftVelocity = velocity_limit;
    }
    else
    {
        shaftAngle = target_angle;
        shaftVelocity = 0;
    }

    Uq = voltageUsedForSensorAlign;
    // set the maximal allowed voltage (voltage_limit) with the necessary angle
    setPhaseVoltage(Uq, 0, getElectricalAngle());

    return Uq;
}
