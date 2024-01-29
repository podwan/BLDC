#ifndef __FOC_H
#define __FOC_H

#include "common.h"

#define PWM_GENERATE(p1, p2, p3) \
    {                            \
        TIM1->CCR1 = p1;         \
        TIM1->CCR2 = p2;         \
        TIM1->CCR3 = p3;         \
    }

// typedef enum
// {
//     MOTOR_STOP,
//     OPEN_LOOP,
//     CLOSE_LOOP,
// } MotorMode;
/******************************************************************************/
// dq current structure
typedef struct
{
    float d;
    float q;
} DQCurrent_s;
// phase current structure
typedef struct
{
    float a;
    float b;
    float c;
} PhaseCurrent_s;
// dq voltage structs
typedef struct
{
    float d;
    float q;
} DQVoltage_s;

/******************************************************************************/
/**
 *  Motiron control type
 */
typedef enum
{
    Type_torque,   //!< Torque control
    Type_velocity, //!< Velocity motion control
    Type_angle,    //!< Position/angle motion control
    Type_velocity_openloop,
    Type_angle_openloop,
} MotionControlType;

/**
 *  Motiron control type
 */
typedef enum
{
    Type_voltage,    //!< Torque control using voltage
    Type_dc_current, //!< Torque control using DC current (one current magnitude)
    Type_foc_current //!< torque control using dq currents
} TorqueControlType;

extern TorqueControlType torque_controller;
extern MotionControlType controller;
/******************************************************************************/
extern float shaft_angle; //!< current motor angle
extern float electrical_angle;
// extern float shaft_velocity;
extern float current_sp;
extern float shaft_velocity_sp;
extern float shaft_angle_sp;
extern DQVoltage_s voltage;
extern DQCurrent_s current;

extern float sensor_offset;

/******************************************************************************/
float shaftAngle(void);
float shaftVelocity(void);
float electricalAngle(void);
/******************************************************************************/

void closeAngleLoop(float targetAngle);
void clarke(float iA, float iB, float iC, float *iAlpha, float *iBeta);
void closeSpeedLoop(float currentSpeed, float setSpeed, float theta, float iA, float iB, float iC, float frequence);
void loopFOC(void);
void setPhaseVoltage(float Uq, float Ud, float angle_el);
#endif
