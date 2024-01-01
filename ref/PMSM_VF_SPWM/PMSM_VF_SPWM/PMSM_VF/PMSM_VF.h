/*
 * File: PMSM_VF.h
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

#ifndef RTW_HEADER_PMSM_VF_h_
#define RTW_HEADER_PMSM_VF_h_
#ifndef PMSM_VF_COMMON_INCLUDES_
#define PMSM_VF_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* PMSM_VF_COMMON_INCLUDES_ */

#include "PMSM_VF_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* Block states (default storage) for system '<Root>' */
typedef struct {
  real32_T Integrator_DSTATE;          /* '<S6>/Integrator' */
  real32_T PrevY;                      /* '<S2>/Rate Limiter1' */
  boolean_T Initial_FirstOutputTime;   /* '<S6>/Initial' */
} DW_PMSM_VF_T;

/* Real-time Model Data Structure */
struct tag_RTM_PMSM_VF_T {
  const char_T * volatile errorStatus;
};

/* Block states (default storage) */
extern DW_PMSM_VF_T PMSM_VF_DW;

/* Model entry point functions */
extern void PMSM_VF_initialize(void);
extern void PMSM_VF_terminate(void);

/* Customized model step function */
extern void PMSM_VF_step(real32_T arg_Speed_Ref, real32_T arg_PWM[3]);

/* Real-time Model object */
extern RT_MODEL_PMSM_VF_T *const PMSM_VF_M;

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<S3>/Data Type Duplicate' : Unused code path elimination
 * Block '<Root>/Scope' : Unused code path elimination
 * Block '<Root>/Scope1' : Unused code path elimination
 * Block '<Root>/Scope10' : Unused code path elimination
 * Block '<Root>/Scope11' : Unused code path elimination
 * Block '<Root>/Scope12' : Unused code path elimination
 * Block '<Root>/Scope13' : Unused code path elimination
 * Block '<Root>/Scope14' : Unused code path elimination
 * Block '<Root>/Scope15' : Unused code path elimination
 * Block '<Root>/Scope16' : Unused code path elimination
 * Block '<Root>/Scope17' : Unused code path elimination
 * Block '<Root>/Scope2' : Unused code path elimination
 * Block '<Root>/Scope3' : Unused code path elimination
 * Block '<Root>/Scope4' : Unused code path elimination
 * Block '<Root>/Scope5' : Unused code path elimination
 * Block '<Root>/Scope6' : Unused code path elimination
 * Block '<Root>/Scope7' : Unused code path elimination
 * Block '<Root>/Scope8' : Unused code path elimination
 * Block '<Root>/Scope9' : Unused code path elimination
 * Block '<S3>/Conversion' : Eliminate redundant data type conversion
 * Block '<S5>/K1' : Eliminated nontunable gain of 1
 * Block '<S5>/K2' : Eliminated nontunable gain of 1
 */

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'PMSM_VF'
 * '<S1>'   : 'PMSM_VF/Output Handle'
 * '<S2>'   : 'PMSM_VF/V//F Control'
 * '<S3>'   : 'PMSM_VF/V//F Control/Data Type Conversion Inherited'
 * '<S4>'   : 'PMSM_VF/V//F Control/Integrator with Wrapped State (Discrete or Continuous)'
 * '<S5>'   : 'PMSM_VF/V//F Control/Inverse Clarke Transform'
 * '<S6>'   : 'PMSM_VF/V//F Control/Integrator with Wrapped State (Discrete or Continuous)/Discrete'
 * '<S7>'   : 'PMSM_VF/V//F Control/Integrator with Wrapped State (Discrete or Continuous)/Discrete/Compare To Constant'
 * '<S8>'   : 'PMSM_VF/V//F Control/Integrator with Wrapped State (Discrete or Continuous)/Discrete/Compare To Constant1'
 * '<S9>'   : 'PMSM_VF/V//F Control/Integrator with Wrapped State (Discrete or Continuous)/Discrete/Reinitialization'
 */
#endif                                 /* RTW_HEADER_PMSM_VF_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
