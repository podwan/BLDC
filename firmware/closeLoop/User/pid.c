#include "pid.h"
PID velocityPID, P_angle;
void pidInit(PID *pid, float kp, float ki, float kd, float outputRamp, float outMax, float outMin)
{
   pid->P = kp;
   pid->I = ki;
   pid->D = kd;
   pid->iTerm = 0;
   pid->error_prev = 0;
   pid->output_prev = 0;
   pid->outputRamp = outputRamp;
   pid->outMax = outMax;
   pid->outMin = outMin;
   pid->timestamp_prev = micros();
}

float pidCompute(PID *pid, float error)
{
   unsigned long timestamp_now;
   float Ts;
   timestamp_now = micros();
   Ts = (timestamp_now - pid->timestamp_prev) * 1e-6f;
   pid->timestamp_prev = timestamp_now;
   if (Ts <= 0 || Ts > 0.5f)
      Ts = 1e-3f;
   t = Ts;
   /*Compute all the working error variables*/
   float output;

   float pTerm = pid->P * error; // P

   pid->iTerm += pid->I * (error + pid->error_prev) * 0.5f * Ts; // I
   CONSTRAINT(pid->iTerm, pid->outMin, pid->outMax);

   float dTerm = pid->D * (error - pid->error_prev) / Ts; // D
   /*Compute PID output*/
   output = pTerm + pid->iTerm + dTerm;
   CONSTRAINT(output, pid->outMin, pid->outMax);

   if (pid->outputRamp > 0)
   {
      // Limit the acceleration by ramping the output
      float outputRate = (output - pid->output_prev) / Ts;
      if (outputRate > pid->outputRamp)
         output = pid->output_prev + pid->outputRamp * Ts;
      else if (outputRate < -pid->outputRamp)
         output = pid->output_prev - pid->outputRamp * Ts;
   }

   /*Remember some variables for next time*/
   pid->output_prev = output;
   pid->error_prev = error;

   return output;
}
