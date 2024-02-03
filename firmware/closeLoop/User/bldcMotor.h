#ifndef __BLDC_MOTOR_H
#define __BLDC_MOTOR_H

#include "userMain.h"

#define M0_Disable HAL_TIM_Base_Stop(&htim1)
/******************************************************************************/

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
extern float target;
extern float voltage_sensor_align;
extern float voltage_power_supply;
extern float shaft_velocity;
/******************************************************************************/
extern float shaft_angle; //!< current motor angle
extern float electrical_angle;
// extern float shaft_velocity;
extern float current_sp;
extern float shaft_velocity_sp;
extern float shaft_angle_sp;

void motorInit(void);
void move(float new_target);
#endif
