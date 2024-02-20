#ifndef __PID_H
#define __PID_H

#include "userMain.h"
/******************************************************************************/
typedef struct
{
    float P;                      //!< Proportional gain
    float I;                      //!< Integral gain
    float D;                      //!< Derivative gain
    float output_ramp;            //!< Maximum speed of change of the output value
    float integral_prev;          //!< last integral component value
    float output_prev;            //!< last pid output value
    float error_prev;             //!< last tracking error value
    float outMax;                 //!< Maximum output value
    float outMin;                 //!< Mininum output value
    unsigned long timestamp_prev; //!< Last execution timestamp
} PIDController;
/******************************************************************************/
extern PIDController PID_velocity, P_angle;
/******************************************************************************/
void PID_init(PIDController *pid, float kp, float ki, float kd, float output_ramp, float outMax, float outMin);
float PID_operator(PIDController *pid, float error);
/******************************************************************************/
#endif
