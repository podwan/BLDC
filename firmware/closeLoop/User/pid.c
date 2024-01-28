#include "pid.h"

void pidInit(PID *pid, float kp, float ki, float kd, float outputRamp, float outMax, float outMin, float dt)
{
   pid->P = kp;
   pid->I = ki;
   pid->D = kd;
   pid->iTerm = 0;
   pid->errorLast = 0;
   pid->outputLast = 0;
   pid->outputRamp = outputRamp;
   pid->outMax = outMax;
   pid->outMin = outMin;
   pid->dt = dt;
}

float pidCompute(PID *pid, float error)
{

   /*Compute all the working error variables*/
   float output;

   float pTerm = pid->P * error; // P

   pid->iTerm += pid->I * (error + pid->errorLast) * 0.5f * pid->dt; // I
   CONSTRAINT(pid->iTerm, pid->outMin, pid->outMax);

   float dTerm = pid->D * (error - pid->errorLast) / pid->dt; // D
   /*Compute PID output*/
   output = pTerm + pid->iTerm + dTerm;
   CONSTRAINT(output, pid->outMin, pid->outMax);

   if (pid->outputRamp > 0)
   {
      // Limit the acceleration by ramping the output
      float outputRate = (output - pid->outputLast) / pid->dt;
      if (outputRate > pid->outputRamp)
         output = pid->outputLast + pid->outputRamp * pid->dt;
      else if (outputRate < -pid->outputRamp)
         output = pid->outputLast - pid->outputRamp * pid->dt;
   }

   /*Remember some variables for next time*/
   pid->outputLast = output;
   pid->errorLast = error;

   return output;
}

// void setTunings(PID *pid, float Kp, float Ki, float Kd)
// {
//    pid->P = Kp;
//    pid->I = Ki;
//    pid->D = Kd;
// }
