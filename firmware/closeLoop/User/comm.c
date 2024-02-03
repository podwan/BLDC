
#include "comm.h"
#include "usart.h"
Uart rxUart;
char sendStuff[MAX_LEN];
uint8_t aRxBuffer;
char sndBuff[USART_BUFFER_SIZE];

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
  printf("%s", sendStuff);
  memset(sendStuff, '\0', sizeof sendStuff);
}

// 若使用C库printf需要实现
// char putchar(char c)
// {
//   UART_SEND_BYTE(c);
//   return c;
// }

/*接收中断调用*/
void uartRcv(const char buf)
{
  if (rxUart.toProcessData == 0)
  {
    if (rxUart.index < MAX_LEN)
    {
      if (buf == '\n') // || buf == '\r')
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

void commander_run(void)
{
  if (rxUart.toProcessData == 1)
  {

    rxUart.toProcessData = 0;
    switch (rxUart.buf[0])
    {
    case 'H':
      // sprintf(sndBuff, "Hello World!\r\n");
      // HAL_UART_Transmit_DMA(&huart3, (uint8_t *)sndBuff, sizeof(sndBuff));
      sprintf(sndBuff, "Hello World!\r\n", target);
      printf("%s", sndBuff);
      break;
    case 'T': // T6.28

      target = atof((const char *)(rxUart.buf + 1));
      sprintf(sndBuff, "Target=%.2f\r\n", target);
      printf("%s", sndBuff);
      // HAL_UART_Transmit_DMA(&huart3, (uint8_t *)sndBuff, sizeof(sndBuff));
      break;
    case 'P': // P0.5  čŽžç˝ŽéĺşŚçŻçPĺć°
      velocityPID.P = atof((const char *)(rxUart.buf + 1));
      sprintf(sndBuff, "P=%.2f\r\n", velocityPID.P);
      printf("%s", sndBuff);
      break;
    case 'I': // I0.2  čŽžç˝ŽéĺşŚçŻçIĺć°
      velocityPID.I = atof((const char *)(rxUart.buf + 1));
      sprintf(sndBuff, "I=%.2f\r\n", velocityPID.I);
      printf("%s", sndBuff);
      break;
    case 'V': // V  čŻťĺŽćśé?ĺşŚ
      sprintf(sndBuff, "Vel=%.2f\r\n", shaft_velocity);
      printf("%s", sndBuff);
      break;
    case 'A': // A  čŻťçťĺŻšč§ĺş?
      sprintf(sndBuff, "Ang=%.2f\r\n", shaft_angle);
      printf("%s", sndBuff);
      break;
    }

    memset(rxUart.buf, '\0', sizeof(rxUart.buf));
    // memset(rxBuffer, '\0', sizeof(rxBuffer));
  }
}
