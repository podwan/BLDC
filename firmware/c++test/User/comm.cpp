
#include "comm.h"
#include "main.h"
#include "usart.h"
// using namespace std;
void printLog()
{

    uint8_t hello1[] = "Hello, blocking\n";
    HAL_UART_Transmit(&huart3, hello1, sizeof(hello1), 500);
}