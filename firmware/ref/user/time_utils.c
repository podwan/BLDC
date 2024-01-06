
#include "userMain.h"
#include "time_utils.h"
#include "stm32g4xx_ll_cortex.h"

__IO uint32_t m, tms, u;

uint64_t micros()
{
    /* Ensure COUNTFLAG is reset by reading SysTick control and status register */
    LL_SYSTICK_IsActiveCounterFlag();
    m = HAL_GetTick();
    tms = SysTick->LOAD + 1;
    u = tms - SysTick->VAL;
    if (LL_SYSTICK_IsActiveCounterFlag())
    {
        m = HAL_GetTick();
        u = tms - SysTick->VAL;
    }
    return (m * 1000 + (u * 1000) / tms);
}