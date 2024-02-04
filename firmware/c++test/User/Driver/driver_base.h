#ifndef DRIVER_BASE_H
#define DRIVER_BASE_H


class DriverBase
{
public:
    explicit DriverBase(float _voltagePowerSupply) :
        voltagePowerSupply(_voltagePowerSupply)
    {}

    float voltagePowerSupply;
    float dutyA;
    float dutyB;
    float dutyC;

    virtual bool Init();
    virtual void SetEnable(bool _enable);
    virtual void SetVoltage(float _voltageA, float _voltageB, float _voltageC) ;
};

#endif
