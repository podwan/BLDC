#ifndef SENSOR_H
#define SENSOR_H

#include "userMain.h"

class EncoderBase
{
public:
    enum Direction
    {
        CW = 1,
        CCW = -1,
        UNKNOWN = 0
    };

    virtual void Init();
    virtual void Update();
    virtual float GetLapAngle();
    virtual float GetFullAngle();
    virtual float GetVelocity();
    virtual int32_t GetRotationCount();

    Direction countDirection;

protected:
    virtual void VarInit();
    virtual float GetRawAngle();

    float angleLast;
    uint64_t angleTimestamp;
    float velocityLast;
    uint64_t velocityTimestamp;
    int32_t rotationCount;
    int32_t rotationCountLast;
};

#endif
