#ifndef __USERMAIN_H
#define __USERMAIN_H

#ifdef __cplusplus
extern "C"
{
#endif
// below are common headers which are not created by the user
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cmath>

    void userMain(void);
#ifdef __cplusplus
}
#endif

#endif
