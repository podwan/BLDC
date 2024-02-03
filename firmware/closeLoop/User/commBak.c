#include "comm.h"
#include "usart.h"
uint8_t rxCompleted = RESET;
bool uartRecvDone;
char rxBuffer[USART_BUFFER_SIZE];

float temp[5];
uint8_t tempData[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x80, 0x7F};

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3)
    {
        // rxCompleted = SET;
        // __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
    }
}

// void on_UART_IDLE(UART_HandleTypeDef *huart)
// {
//     if (__HAL_UART_GET_IT_SOURCE(huart, UART_FLAG_IDLE) == RESET)
//     {
//         return;
//     }
//     __HAL_UART_CLEAR_IDLEFLAG(huart);
//     __HAL_UART_DISABLE_IT(huart, UART_IT_IDLE);
//     if (rxCompleted)
//     {

//         switch (rxBuffer[0])
//         {
//         case 'H':
//             sprintf(sndBuff, "Hello World!\r\n");
//             HAL_UART_Transmit_DMA(&huart3, (uint8_t *)sndBuff, sizeof(sndBuff));
//             break;
//         case 'T': // T6.28
//             // printf("%s", rxBuffer);
//             // printf("target = %.2f\n", atof(rxBuffer + 1));
//             target = atof((const char *)(rxBuffer + 1));
//             sprintf(sndBuff, "Target=%.2f\r\n", target);
//             HAL_UART_Transmit_DMA(&huart3, (uint8_t *)sndBuff, sizeof(sndBuff));
//             break;
//         case 'P': // P0.5  čŽžç˝ŽéĺşŚçŻçPĺć°
//             velocityPID.P = atof((const char *)(rxBuffer + 1));
//             sprintf(sndBuff, "P=%.2f\r\n", velocityPID.P);
//             break;
//         case 'I': // I0.2  čŽžç˝ŽéĺşŚçŻçIĺć°
//             velocityPID.I = atof((const char *)(rxBuffer + 1));
//             sprintf(sndBuff, "I=%.2f\r\n", velocityPID.I);
//             break;
//         case 'V': // V  čŻťĺŽćśé?ĺşŚ
//             sprintf(sndBuff, "Vel=%.2f\r\n", shaft_velocity);
//             break;
//         case 'A': // A  čŻťçťĺŻšč§ĺş?
//             sprintf(sndBuff, "Ang=%.2f\r\n", shaft_angle);
//             break;
//         }

//         rxCompleted = RESET;
//         HAL_UART_Receive_IT(huart, rxBuffer, RX_CMD_LEN);
//     }
// }
void setUartRecvDone()
{
    uartRecvDone = 1;
}
