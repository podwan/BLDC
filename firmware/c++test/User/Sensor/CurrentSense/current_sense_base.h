#ifndef CTRL_FOC_LITE_FW_CURRENT_SENSE_BASE_H
#define CTRL_FOC_LITE_FW_CURRENT_SENSE_BASE_H

#include "User/Driver/driver_base.h"
#include "User/Motor/math_utils.h"


class CurrentSenseBase
{
public:
    // Decided by hardware & configuration
     const static uint16_t ADC_CONVERT_TIME_US = 8;
     const static uint32_t ADC_RESOLUTION = 4096;
   //  const static float ADC_VOLTAGE_RANGE = 3.3f;

    uint8_t sector;
    float pwmDutyA;
    float pwmDutyB;
    float pwmDutyC;

    /*
     * Need to adjust considering ADC sample time,
     * for example Threshold = AAR_ADC / PWM_Period = 1500/1799 ≈ 0.84,
     * so when one phase has output a pwm duty cycle more than 0.84 then
     * this phase's current sense may not be used.
     */
    float dropOnePhaseDutyThreshold;

    virtual void Init();
    virtual PhaseCurrent_t GetPhaseCurrents();
    virtual float GetDcCurrent(float _angleElectrical);
    DqCurrent_t GetFocCurrents(float _angleElectrical);


protected:
    float iAlpha, iBeta;
};


#endif //CTRL_FOC_LITE_FW_CURRENT_SENSE_BASE_H
