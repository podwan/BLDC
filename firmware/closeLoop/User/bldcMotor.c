#include "bldcMotor.h"
#include "foc.h"
float shaft_velocity;
LowPassFilter LPF_current_q, LPF_current_d, LPF_velocity;
unsigned long open_loop_timestamp;
float voltage_sensor_align;
float velocityOpenloop(float target_velocity);
void motorInit(void)
{
    lowPassFilterInit(&LPF_velocity, 0.02f);
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
    //     current_sp = PIDoperator(&PID_velocity, (shaft_velocity_sp - shaft_velocity)); // if voltage torque control
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
    //     current_sp = PIDoperator(&PID_velocity, (shaft_velocity_sp - shaft_velocity)); // if current/foc_current torque control
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
    // setPhaseVoltage(Uq, 0, _electricalAngle(shaft_angle, POLE_PAIRS));

    return Uq;
}
