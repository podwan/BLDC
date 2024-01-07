#ifndef __COMMON_H
#define __COMMON_H
#include "stm32g4xx_hal.h"
#include "math_utils.h"

#define TRUE 1

#define FALSE 0

typedef enum
{
    false,
    true
} bool;

#define TRUE (1 == 1)

#define FALSE (!TRUE)

#endif