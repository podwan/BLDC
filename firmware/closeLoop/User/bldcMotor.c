#include "bldcMotor.h"
#include "foc.h"

float voltage_power_supply;
float voltage_limit;
float voltage_sensor_align;
float velocity_limit;
float current_limit;
float shaft_velocity;
unsigned long open_loop_timestamp;
float target;
/******************************************************************************/
static float velocityOpenloop(float target_velocity);
static float angleOpenloop(float target_angle);
/******************************************************************************/

void motorInit(void)
{
    lowPassFilterInit(&LPF_velocity, 0.02f);
    MagneticSensor_Init(0, UNKNOWN);

    voltage_sensor_align = 2; // V 航模电机设置的值小一点比如0.5-1，云台电机设置的大一点比如2-3

    torque_controller = Type_voltage; // 当前只有电压模式
    controller = Type_angle_openloop; // Type_angle; //Type_torque; //Type_velocity
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

    if (voltage_sensor_align > voltage_limit)
        voltage_sensor_align = voltage_limit;
}

void move(float new_target)
{
    shaft_velocity = shaftVelocity();

    switch (controller)
    {
    // case Type_torque:
    //     if (torque_controller == Type_voltage)
    //         voltage.q = new_target; // if voltage torque control
    //     else
    //         current_sp = new_target; // if current/foc_current torque control
    //     break;
    case Type_angle:
        // angle set point
        shaft_angle_sp = new_target;
        // calculate velocity set point
        shaft_velocity_sp = pidCompute(&P_angle, (shaft_angle_sp - shaft_angle));
        // calculate the torque command
        current_sp = pidCompute(&velocityPID, (shaft_velocity_sp - shaft_velocity)); // if voltage torque control
        // if torque controlled through voltage
        if (torque_controller == Type_voltage)
        {
            voltage.q = current_sp;
            voltage.d = 0;
        }
        break;
    case Type_velocity:
        // velocity set point
        shaft_velocity_sp = new_target;
        // calculate the torque command
        current_sp = pidCompute(&velocityPID, (shaft_velocity_sp - shaft_velocity)); // if current/foc_current torque control
        // if torque controlled through voltage control
        if (torque_controller == Type_voltage)
        {
            voltage.q = current_sp; // use voltage if phase-resistance not provided
            voltage.d = 0;
        }
        break;
    case Type_velocity_openloop:
        // velocity control in open loop
        shaft_velocity_sp = new_target;
        // shaft_velocity_sp = 1;
        voltage.q = velocityOpenloop(shaft_velocity_sp); // returns the voltage that is set to the motor
        voltage.d = 0;
        break;
    case Type_angle_openloop:
        // angle control in open loop
        shaft_angle_sp = new_target;
        voltage.q = angleOpenloop(shaft_angle_sp); // returns the voltage that is set to the motor
        voltage.d = 0;
        break;
    }
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
    shaft_angle = _normalizeAngle(shaft_angle + target_velocity * Ts);
    // for display purposes
    shaft_velocity = target_velocity;

    Uq = voltage_sensor_align;
    // set the maximal allowed voltage (voltage_limit) with the necessary angle
    setPhaseVoltage(Uq, 0, _electricalAngle(shaft_angle, pole_pairs));

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
    if (fabs(target_angle - shaft_angle) > velocity_limit * Ts)
    {
        shaft_angle += SIGN(target_angle - shaft_angle) * velocity_limit * Ts;
        shaft_velocity = velocity_limit;
    }
    else
    {
        shaft_angle = target_angle;
        shaft_velocity = 0;
    }

    Uq = voltage_sensor_align;
    // set the maximal allowed voltage (voltage_limit) with the necessary angle
    setPhaseVoltage(Uq, 0, _electricalAngle(shaft_angle, pole_pairs));

    return Uq;
}
