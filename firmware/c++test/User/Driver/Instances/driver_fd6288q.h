#ifndef DriverFD6288QBase_h
#define DriverFD6288QBase_h

#include "driver_base.h"
#include "math_utils.h"

class DriverFD6288QBase : public DriverBase
{
public:
    explicit DriverFD6288QBase(float _voltagePowerSupply) : DriverBase(_voltagePowerSupply)
    {
    }

    static const uint32_t PWM_FREQUENCY = 20000; // set by STM32CubeMX

private:
    virtual bool ConfigTimerForPwm();
    virtual void SetPwmDutyByRegister(float _dutyA, float _dutyB, float _dutyC);

    virtual bool Init();
    virtual void SetVoltage(float _voltageA, float _voltageB, float _voltageC);
    virtual void SetEnable(bool _enable);
};

#endif
