
#ifndef __COMM_H_
#define __COMM_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "userMain.h"

#define MAX_LEN 15

    void printLog(void);

    extern uint8_t aRxBuffer;
    extern float target;

    typedef struct
    {
        unsigned char index;
        unsigned char buf[MAX_LEN];
        unsigned char len;
        unsigned char toProcessData;
    } Uart;
    void commander_run(void);
#ifdef __cplusplus
}
#endif

#endif
