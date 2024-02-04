
#include "comm.h"
#include "usart.h"

uint8_t aRxBuffer;
float target;
Uart rxUart;
uint8_t sendBuffer[MAX_LEN];

/*接收中断调用*/
void uartRcv(const char buf)
{
    if (rxUart.toProcessData == 0)
    {
        if (rxUart.index < MAX_LEN)
        {
            if (buf == '\n')
            {
                rxUart.toProcessData = 1;
                rxUart.index = 0;
            }
            else
            {
                rxUart.buf[rxUart.index] = buf;
                rxUart.index++;
                rxUart.len++;
            }
        }
        else // 接收的数据过长，无效
        {
            memset(rxUart.buf, '\0', MAX_LEN);
            rxUart.index = 0;
            rxUart.len = 0;
        }
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3)
    {
        uartRcv(aRxBuffer);

        HAL_UART_Receive_IT(&huart3, (uint8_t *)&aRxBuffer, 1);
    }
}
void printLog()
{
    uint8_t hello1[] = "Hello\n";
    HAL_UART_Transmit(&huart3, hello1, sizeof(hello1), 500);
}

void commander_run(void)
{
    if (rxUart.toProcessData == 1)
    {

        rxUart.toProcessData = 0;
        switch (rxUart.buf[0])
        {
        case 'H':

            sprintf((char *)sendBuffer, "Hello World!\r\n");
            HAL_UART_Transmit(&huart3, sendBuffer, sizeof(sendBuffer), 500);
            break;
        case 'T': // T6.28
            target = atof((const char *)(rxUart.buf + 1));
            sprintf((char *)sendBuffer, "Target=%.2f\r\n", target);
            HAL_UART_Transmit(&huart3, sendBuffer, sizeof(sendBuffer), 500);
            //    HAL_UART_Transmit_DMA(&huart3, (uint8_t *)sendBuffer, sizeof(sendBuffer));
            break;
            // case 'P': // P0.5
            //     velocityPID.P = atof((const char *)(rxUart.buf + 1));
            //     sprintf(sendBuffer, "P=%.2f\r\n", velocityPID.P);
            //     printf("%s", sendBuffer);
            //     break;
            // case 'I': // I0.2
            //     velocityPID.I = atof((const char *)(rxUart.buf + 1));
            //     sprintf(sendBuffer, "I=%.2f\r\n", velocityPID.I);
            //     printf("%s", sendBuffer);
            //     break;
            // case 'V': // V
            //     sprintf(sendBuffer, "Vel=%.2f\r\n", estimateVelocity);
            //     printf("%s", sendBuffer);
            //     break;
            // case 'A': // A
            //     sprintf(sendBuffer, "Ang=%.2f\r\n", estimateAngle);
            //     printf("%s", sendBuffer);
            //     break;
        }

        memset(rxUart.buf, '\0', sizeof(rxUart.buf));
        // memset(rxBuffer, '\0', sizeof(rxBuffer));
    }
}
