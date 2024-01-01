/*
 * File: PMSM_VF.c
 *
 * Code generated for Simulink model 'PMSM_VF'.
 *
 * Model version                  : 1.19
 * Simulink Coder version         : 9.7 (R2022a) 13-Nov-2021
 * C/C++ source code generated on : Fri Dec  2 21:15:21 2022
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "PMSM_VF.h"
#include "rtwtypes.h"
#include <math.h>
#include "PMSM_VF_private.h"

/* Block states (default storage) */
DW_PMSM_VF_T PMSM_VF_DW;

/* Real-time model */
static RT_MODEL_PMSM_VF_T PMSM_VF_M_;
RT_MODEL_PMSM_VF_T *const PMSM_VF_M = &PMSM_VF_M_;

/* Model step function */
void PMSM_VF_step(real32_T arg_Speed_Ref, real32_T arg_PWM[3])
{
  real32_T Initial;
  real32_T rtb_Saturation;
  real32_T rtb_Vbeta;

  /* RateLimiter: '<S2>/Rate Limiter1' incorporates:
   *  Inport: '<Root>/Speed_Ref'
   */
  rtb_Saturation = arg_Speed_Ref - PMSM_VF_DW.PrevY;
  if (rtb_Saturation > 0.025F) {
    rtb_Saturation = PMSM_VF_DW.PrevY + 0.025F;
  } else if (rtb_Saturation < -0.025F) {
    rtb_Saturation = PMSM_VF_DW.PrevY + -0.025F;
  } else {
    rtb_Saturation = arg_Speed_Ref;
  }

  PMSM_VF_DW.PrevY = rtb_Saturation;

  /* End of RateLimiter: '<S2>/Rate Limiter1' */

  /* Gain: '<S2>/Speed_to_Frequency' */
  rtb_Saturation *= 0.116666667F;

  /* Saturate: '<S2>/Saturation' */
  if (rtb_Saturation > 1463.0F) {
    rtb_Saturation = 1463.0F;
  } else if (rtb_Saturation < 0.0F) {
    rtb_Saturation = 0.0F;
  }

  /* End of Saturate: '<S2>/Saturation' */

  /* Gain: '<S2>/Gain1' incorporates:
   *  Bias: '<S2>/Bias'
   *  Gain: '<S2>/V//F'
   */
  rtb_Vbeta = ((12.0F - Vmin) / 1463.0F * rtb_Saturation + Vmin) * 0.0833333358F;

  /* Saturate: '<S2>/Saturation1' */
  if (rtb_Vbeta > 1.0F) {
    rtb_Vbeta = 1.0F;
  } else if (rtb_Vbeta < 0.0F) {
    rtb_Vbeta = 0.0F;
  }

  /* End of Saturate: '<S2>/Saturation1' */

  /* InitialCondition: '<S6>/Initial' */
  if (PMSM_VF_DW.Initial_FirstOutputTime) {
    PMSM_VF_DW.Initial_FirstOutputTime = false;

    /* InitialCondition: '<S6>/Initial' */
    Initial = 0.0F;
  } else {
    /* InitialCondition: '<S6>/Initial' incorporates:
     *  Bias: '<S9>/Bias'
     *  Gain: '<S9>/Gain'
     *  Gain: '<S9>/Gain1'
     *  Rounding: '<S9>/Rounding Function'
     *  Sum: '<S9>/Sum1'
     */
    Initial = PMSM_VF_DW.Integrator_DSTATE - floorf(0.159154937F *
      PMSM_VF_DW.Integrator_DSTATE) * 6.28318548F;
  }

  /* End of InitialCondition: '<S6>/Initial' */

  /* DiscreteIntegrator: '<S6>/Integrator' incorporates:
   *  Constant: '<S7>/Constant'
   *  Constant: '<S8>/Constant'
   *  Logic: '<S6>/Logical Operator'
   *  RelationalOperator: '<S7>/Compare'
   *  RelationalOperator: '<S8>/Compare'
   */
  if ((PMSM_VF_DW.Integrator_DSTATE < 0.0F) || (PMSM_VF_DW.Integrator_DSTATE >=
       6.28318548F)) {
    PMSM_VF_DW.Integrator_DSTATE = Initial;
  }

  /* Fcn: '<S2>/Valpha' incorporates:
   *  DiscreteIntegrator: '<S6>/Integrator'
   */
  Initial = rtb_Vbeta * cosf(PMSM_VF_DW.Integrator_DSTATE);

  /* Fcn: '<S2>/Vbeta' incorporates:
   *  DiscreteIntegrator: '<S6>/Integrator'
   */
  rtb_Vbeta *= sinf(PMSM_VF_DW.Integrator_DSTATE);

  /* Outport: '<Root>/PWM' incorporates:
   *  Constant: '<S1>/Constant'
   *  Fcn: '<S5>/a'
   *  Fcn: '<S5>/b'
   *  Fcn: '<S5>/c'
   *  Gain: '<S1>/Gain'
   *  Sum: '<S1>/Sum'
   */
  arg_PWM[0] = (Initial + 1.0F) * 0.5F;
  arg_PWM[1] = ((-0.5F * Initial + 0.866025388F * rtb_Vbeta) + 1.0F) * 0.5F;
  arg_PWM[2] = ((-0.5F * Initial - 0.866025388F * rtb_Vbeta) + 1.0F) * 0.5F;

  /* Update for DiscreteIntegrator: '<S6>/Integrator' incorporates:
   *  Gain: '<S2>/Gain'
   */
  PMSM_VF_DW.Integrator_DSTATE += 6.28318548F * rtb_Saturation * 5.0E-5F;
}

/* Model initialize function */
void PMSM_VF_initialize(void)
{
  /* Start for InitialCondition: '<S6>/Initial' */
  PMSM_VF_DW.Initial_FirstOutputTime = true;
}

/* Model terminate function */
void PMSM_VF_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
