#include "pid.h"

void pidInit(PID *pid, float kp, float ki, float kd, float iTerms, float lastInput,
             bool positiveFeedback, float outMax, float outMin)
{
   pid->kp = kp;
   pid->kd = kd;
   pid->ki = ki;
   pid->iTerms = iTerms;
   pid->lastInput = lastInput;
   pid->positiveFeedback = positiveFeedback;
   pid->outMax = outMax;
   pid->outMin = outMin;
}

unsigned int compute(PID *pid, float setPoint, float input)
{

   /*Compute all the working error variables*/

   float output;
   float error = setPoint - input;        // P
   float dInput = input - pid->lastInput; // D
   pid->iTerms += (pid->ki * error);      // I
   if (pid->iTerms > pid->outMax)
      pid->iTerms = pid->outMax;
   else if (pid->iTerms < pid->outMin)
      pid->iTerms = pid->outMin;

   /*Compute PID output*/
   output = pid->kp * error + pid->iTerms - pid->kd * dInput;
   if (output > pid->outMax)
      output = pid->outMax;
   else if (output < pid->outMin)
      output = pid->outMin;

   /*Remember some variables for next time*/
   pid->lastInput = input;

   return (unsigned int)output;
}

void setTunings(PID *pid, float Kp, float Ki, float Kd)
{
   if (Kp < 0 || Ki < 0 || Kd < 0)
      return;
   else
   {
      if (pid->positiveFeedback == 1)
      {
         pid->kp = Kp;
         pid->ki = Ki;
         pid->kd = Kd;
      }
      else
      {
         pid->kp = (0 - pid->kp);
         pid->ki = (0 - pid->ki);
         pid->kd = (0 - pid->kd);
      }
   }
}
