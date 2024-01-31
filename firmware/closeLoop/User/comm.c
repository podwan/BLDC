#include "comm.h"
#include "usart.h"

bool uartRecvDone;
char rxBuffer[USART_BUFFER_SIZE];
char sndBuff[USART_BUFFER_SIZE];
float temp[5];
uint8_t tempData[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x80, 0x7F};

void setUartRecvDone()
{
    uartRecvDone = 1;
}

void commander_run(void)
{
    if (uartRecvDone == 1)
    {
        memset(sndBuff, '\0', sizeof(sndBuff));
        uartRecvDone = 0;
        switch (rxBuffer[0])
        {
        case 'H':
            sprintf(sndBuff, "Hello World!\r\n");
            HAL_UART_Transmit_DMA(&huart3, (uint8_t *)sndBuff, sizeof(sndBuff));
            break;
        case 'T': // T6.28
            // printf("%s", rxBuffer);
            // printf("target = %.2f\n", atof(rxBuffer + 1));
            target = atof((const char *)(rxBuffer + 1));
            sprintf(sndBuff, "RX=%.2f\r\n", target);
            HAL_UART_Transmit_DMA(&huart3, (uint8_t *)sndBuff, sizeof(sndBuff));
            break;
        case 'P': // P0.5  čŽžç˝ŽéĺşŚçŻçPĺć°
            velocityPID.P = atof((const char *)(rxBuffer + 1));
            sprintf(sndBuff, "P=%.2f\r\n", velocityPID.P);
            break;
        case 'I': // I0.2  čŽžç˝ŽéĺşŚçŻçIĺć°
            velocityPID.I = atof((const char *)(rxBuffer + 1));
            sprintf(sndBuff, "I=%.2f\r\n", velocityPID.I);
            break;
        case 'V': // V  čŻťĺŽćśé?ĺşŚ
            sprintf(sndBuff, "Vel=%.2f\r\n", shaft_velocity);
            break;
        case 'A': // A  čŻťçťĺŻšč§ĺş?
            sprintf(sndBuff, "Ang=%.2f\r\n", shaft_angle);
            break;
        }

        memset(rxBuffer, '\0', sizeof(rxBuffer));
    }
}

