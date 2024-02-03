#ifndef __COMM_H
#define __COMM_H

#include "userMain.h"

#define UART_SEND_BYTE(STUFF) UART0_Send_Byte(STUFF)

#define MAX_LEN 10
#define USART_BUFFER_SIZE 10
typedef struct
{
    unsigned char index;
    unsigned char buf[MAX_LEN];
    unsigned char len;
    unsigned char toProcessData;
} Uart;

void printLog();
void uartRcv(const char buf);
void commander_run(void);

extern Uart rxUart;
extern char sendStuff[MAX_LEN];
extern uint8_t aRxBuffer;

#endif
