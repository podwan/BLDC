#ifndef __COMM_H
#define __COMM_H
#include "userMain.h"
#define USART_BUFFER_SIZE 256
extern float temp[5];
extern uint8_t tempData[24];
extern char rxBuffer[USART_BUFFER_SIZE];
void setUartRecvDone(void);
void commander_run(void);
#endif

