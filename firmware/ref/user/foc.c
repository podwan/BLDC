#include "foc.h"
#include "sensor.h"
#include "math_utils.h"
#include <math.h>
#include "app.h"

static CurrentSensor cs;

void clark(float *Ia, float *Ib)
{
    //  getPhaseCurrents(&cs, getSector());
    *Ia = cs.currentU - cs.currentV / 2 - cs.currentW / 2;
    *Ib = _SQRT3_2 * cs.currentV - _SQRT3_2 * cs.currentW;
}

void park(float electricAngle, float Ia, float Ib, float *Iq, float *Id)
{
    *Id = Ia * CosApprox(electricAngle) + Ib * SinApprox(electricAngle);
    *Iq = -Ia * SinApprox(electricAngle) + Ib * CosApprox(electricAngle);
}

// sample(Id Iq), goal (Id, Iq) --> PID ---> Vq, Vd

void revPark(float electricAngle, float *Va, float *Vb, float Vq, float Vd)
{
    *Va = Vd * CosApprox(electricAngle) - Vq * SinApprox(electricAngle);
    *Vb = Vd * SinApprox(electricAngle) + Vq * CosApprox(electricAngle);
}

void revClark(float Va, float Vb, ThreePhaseVoltage *tpv)
{
    tpv->U = Va + voltagePowerSupply / 2;
    tpv->V = (_SQRT3 * Vb - Va) / 2 + voltagePowerSupply / 2;
    tpv->W = (-Va - _SQRT3 * Vb) / 2 + voltagePowerSupply / 2;
}
int Ut, Vt, Wt;
float v1, v2, t1, t2, t3;
float voltageU, voltageV, voltageW;
float Va, Vb;

void SVPWM(float electricAngle, float Vq, float Vd)
{
    // reverse park
    Va = Vd * CosApprox(electricAngle) - Vq * SinApprox(electricAngle);
    Vb = Vd * SinApprox(electricAngle) + Vq * CosApprox(electricAngle);

    // map to space vector
    v1 = fabs(Va - _1_SQRT3 * Vb);
    v2 = fabs(_2_SQRT3 * Vb);

    // compute duty accoring to sector
    t1 = PWM_PERIOD * v1 / voltagePowerSupply;
    t2 = PWM_PERIOD * v2 / voltagePowerSupply;
    t3 = PWM_PERIOD - t1 - t2;

    switch (getSector(electricAngle))
    {
    case 1:
        Ut = t1 + t2 + t3 / 2;
        Vt = t2 + t3 / 2;
        Wt = t3 / 2;
        break;

    case 2:
        Ut = t1 + t3 / 2;
        Vt = t1 + t2 + t3 / 2;
        Wt = t3 / 2;
        break;
    case 3:
        Ut = t3 / 2;
        Vt = t1 + t2 + t3 / 2;
        Wt = t2 + t3 / 2;
        break;
    case 4:
        Ut = t3 / 2;
        Vt = t1 + t3 / 2;
        Wt = t1 + t2 + t3 / 2;
        break;
    case 5:
        Ut = t2 + t3 / 2;
        Vt = t3 / 2;
        Wt = t1 + t2 + t3 / 2;
        break;
    case 6:
        Ut = t1 + t2 + t3 / 2;
        Vt = t3 / 2;
        Wt = t1 + t3 / 2;
        break;
    }
    // voltageU = Ut * voltagePowerSupply;
    // voltageV = Vt * voltagePowerSupply;
    // voltageW = Wt * voltagePowerSupply;
    // tpv->U = CONSTRAINT(voltageU, 0, voltagePowerSupply);
    // tpv->V = CONSTRAINT(voltageV, 0, voltagePowerSupply);
    // tpv->W = CONSTRAINT(voltageW, 0, voltagePowerSupply);
    // Ut *= vLimit / voltagePowerSupply;
    // Vt *= vLimit / voltagePowerSupply;
    // Wt *= vLimit / voltagePowerSupply;
    // Ut = CONSTRAINT(Ut, 0, PWM_PERIOD * vLimit / voltagePowerSupply);
    // Vt = CONSTRAINT(Vt, 0, PWM_PERIOD * vLimit / voltagePowerSupply);
    // Wt = CONSTRAINT(Wt, 0, PWM_PERIOD * vLimit / voltagePowerSupply);

    SET_PWM(Ut, Vt, Wt);
}

float Constraint(float _val)
{
    if (_val > 1)
        _val = 1;
    else if (_val < 0)
        _val = 0;

    return _val;
}

void SetVoltage(float _voltageA, float _voltageB, float _voltageC)
{
    float dutyA;
    float dutyB;
    float dutyC;
    _voltageA = CONSTRAINT(_voltageA, 0, voltagePowerSupply);
    _voltageB = CONSTRAINT(_voltageB, 0, voltagePowerSupply);
    _voltageC = CONSTRAINT(_voltageC, 0, voltagePowerSupply);

    dutyA = _voltageA / voltagePowerSupply * PWM_PERIOD;
    dutyB = _voltageB / voltagePowerSupply * PWM_PERIOD;
    dutyC = _voltageC / voltagePowerSupply * PWM_PERIOD;

    SET_PWM(dutyA, dutyB, dutyC);
}

float voltageA, voltageB, voltageC;
float uOut, t0, t1, t2;
uint8_t sec;
float tA, tB, tC;
void SetPhaseVoltage(float _voltageQ, float _voltageD, float _angleElectrical)
{
    // uOut = _angleElectrical;
    //    if (_voltageD != 0)
    //    {
    //        uOut = SQRT(_voltageD * _voltageD + _voltageQ * _voltageQ) / voltagePowerSupply;
    //        _angleElectrical = normalizeAngle(_angleElectrical + atan2(_voltageQ, _voltageD));
    //    }
    //    else
    // {
    //     uOut = _voltageQ / voltagePowerSupply;
    // _angleElectrical = normalizeAngle(_angleElectrical + _PI_2);
    // }

    t1 = _SQRT3 * SinApprox((float)(sec)*_PI_3 - _angleElectrical);
    t2 = _SQRT3 * SinApprox(_angleElectrical - ((float)(sec)-1.0f) * _PI_3);
    t0 = 1 - t1 - t2;

    switch (getSector(_angleElectrical))
    {
    case 1:
        tA = t1 + t2 + t0 / 2;
        tB = t2 + t0 / 2;
        tC = t0 / 2;
        break;
    case 2:
        tA = t1 + t0 / 2;
        tB = t1 + t2 + t0 / 2;
        tC = t0 / 2;
        break;
    case 3:
        tA = t0 / 2;
        tB = t1 + t2 + t0 / 2;
        tC = t2 + t0 / 2;
        break;
    case 4:
        tA = t0 / 2;
        tB = t1 + t0 / 2;
        tC = t1 + t2 + t0 / 2;
        break;
    case 5:
        tA = t2 + t0 / 2;
        tB = t0 / 2;
        tC = t1 + t2 + t0 / 2;
        break;
    case 6:
        tA = t1 + t2 + t0 / 2;
        tB = t0 / 2;
        tC = t1 + t0 / 2;
        break;
    default:
        tA = 0;
        tB = 0;
        tC = 0;
    }

    // calculate the phase voltages and center
    voltageA = tA * voltagePowerSupply;
    voltageB = tB * voltagePowerSupply;
    voltageC = tC * voltagePowerSupply;

    SetVoltage(voltageA, voltageB, voltageC);
}