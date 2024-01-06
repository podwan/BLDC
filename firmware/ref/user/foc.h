#ifndef __FOC_H
#define __FOC_H
#include "userMain.h"


#define PWM_PERIOD 4000

typedef struct
{
    float U;
    float V;
    float W;
} ThreePhaseVoltage;

void clark(float *Ia, float *Ib);
void park(float electricAngle, float Ia, float Ib, float *Iq, float *Id);

void revPark(float electricAngle, float *Va, float *Vb, float Vq, float Vd);
void revClark(float Va, float Vb, ThreePhaseVoltage *tpv);
void SVPWM(float electricAngle, float Vq, float Vd);

#endif
