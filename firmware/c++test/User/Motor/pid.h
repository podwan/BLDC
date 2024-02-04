#ifndef PID_H
#define PID_H

#include "userMain.h"
#include "time_utils.h"
#include "math_utils.h"

class PidController
{
public:
    PidController();

    explicit PidController(float _p, float _i, float _d, float _ramp, float _limit) : p(_p), i(_i), d(_d), outputRamp(_ramp), limit(_limit)
    {
        timeStamp = micros();
    }

    float operator()(float error);

    float p;
    float i;
    float d;
    float outputRamp;
    float limit;

protected:
    float errorLast;
    float outputLast;
    float integralLast;
    unsigned long timeStamp;
};

#endif

