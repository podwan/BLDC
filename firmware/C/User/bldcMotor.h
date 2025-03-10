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
    VELOCITY, //!< Velocity motion control
    ANGLE,    //!< Position/angle motion control
    VELOCITY_OPEN_LOOP,
    ANGLE_OPEN_LOOP,
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
extern MotionControlType controlMode;
extern float target;
extern float voltageUsedForSensorAlign;
extern float voltagePowerSupply;
extern float shaftVelocity;
/******************************************************************************/
extern float shaftAngle; //!< current motor angle
extern float electricalAngle;
// extern float shaftVelocity;
extern float setPointCurrent;
extern float setPointVelocity;
extern float setPointAngle;

void motorInit(void);
void move(void);
void loopFOC(void);
void ThreadCtrlLoop(void *argument);
#endif
