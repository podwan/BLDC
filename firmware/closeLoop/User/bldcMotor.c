#include "bldcMotor.h"
#include "foc.h"

float voltage_power_supply;
float voltage_limit;
float voltage_sensor_align;

unsigned long open_loop_timestamp;
float velocity_limit;
float current_limit;
float shaft_velocity;
LowPassFilter LPF_current_q, LPF_current_d, LPF_velocity;
unsigned long open_loop_timestamp;
float target;
/******************************************************************************/
int alignSensor(void);

float angleOpenloop(float target_angle);
/******************************************************************************/

void motorInit(void)
{
    lowPassFilterInit(&LPF_velocity, 0.02f);
    MagneticSensor_Init();

    voltage_sensor_align = 2; // V 航模电机设置的值小一点比如0.5-1，云台电机设置的大一点比如2-3

    torque_controller = Type_voltage;    // 当前只有电压模式
    controller = Type_velocity_openloop; // Type_angle; //Type_torque; //Type_velocity
    velocityPID.P = 0.1;                 // 0.2, 速度环PI参数，只用P参数方便快速调试
    velocityPID.I = 1;

    velocityPID.outputRamp = 100; // 速度爬升斜率，如果不需要可以设置为0
    LPF_velocity.timeConstant = 0.02;
    target = 0;
}
/******************************************************************************/
void Motor_initFOC(float zero_electric_offset, Direction _sensor_direction)
{
    // int exit_flag = 1;

    if (zero_electric_offset != 0)
    {
        // abosolute zero offset provided - no need to align
        zero_electric_angle = zero_electric_offset;
        // set the sensor direction - default CW
        sensor_direction = _sensor_direction;
    }
    alignSensor(); // 检测零点偏移量和极对数

    // shaft_angle update
    angle_prev = getAngle(); // getVelocity(),make sure velocity=0 after power on
    delay(50);
    shaft_velocity = shaftVelocity(); // 必须调用一次，进入主循环后速度为0
    delay(5);
    shaft_angle = shaftAngle(); // shaft angle
    if (controller == Type_angle)
        target = shaft_angle; // 角度模式，以当前的角度为目标角度，进入主循环后电机静止

    delay(200);
}


float velocityOpenloop(float target_velocity)
{
    unsigned long now_us;
    float Ts, Uq;

    now_us = micros();                           // get current timestamp
    Ts = (now_us - open_loop_timestamp) * 1e-6f; // calculate the sample time from last call
    if (Ts <= 0 || Ts > 0.5f)
        Ts = 1e-3f;               // quick fix for strange cases (micros overflow + timestamp not defined)
    open_loop_timestamp = now_us; // save timestamp for next call

    // calculate the necessary angle to achieve target velocity
    shaft_angle = normalizeAngle(shaft_angle + target_velocity * Ts);
    // for display purposes
    shaft_velocity = target_velocity;

    Uq = voltage_sensor_align;
    // set the maximal allowed voltage (voltage_limit) with the necessary angle
    // setPhaseVoltage(Uq, 0, _electricalAngle(shaft_angle, pole_pairs));

    return Uq;
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
    // case Type_angle:
    //     // angle set point
    //     shaft_angle_sp = new_target;
    //     // calculate velocity set point
    //     shaft_velocity_sp = PIDoperator(&P_angle, (shaft_angle_sp - shaft_angle));
    //     // calculate the torque command
    //     current_sp = PIDoperator(&velocityPID, (shaft_velocity_sp - shaft_velocity)); // if voltage torque control
    //     // if torque controlled through voltage
    //     if (torque_controller == Type_voltage)
    //     {
    //         voltage.q = current_sp;
    //         voltage.d = 0;
    //     }
    //     break;
    // case Type_velocity:
    //     // velocity set point
    //     shaft_velocity_sp = new_target;
    //     // calculate the torque command
    //     current_sp = PIDoperator(&velocityPID, (shaft_velocity_sp - shaft_velocity)); // if current/foc_current torque control
    //     // if torque controlled through voltage control
    //     if (torque_controller == Type_voltage)
    //     {
    //         voltage.q = current_sp; // use voltage if phase-resistance not provided
    //         voltage.d = 0;
    //     }
    //     break;
    case Type_velocity_openloop:
        // velocity control in open loop
        shaft_velocity_sp = new_target;
        voltage.q = velocityOpenloop(shaft_velocity_sp); // returns the voltage that is set to the motor
        voltage.d = 0;
        break;
        // case Type_angle_openloop:
        //     // angle control in open loop
        //     shaft_angle_sp = new_target;
        //     voltage.q = angleOpenloop(shaft_angle_sp); // returns the voltage that is set to the motor
        //     voltage.d = 0;
        //     break;
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
