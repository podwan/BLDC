
#include "main.h"
#include "usart.h"
#include <string>
using namespace std;

void userMain()
{

}

void startup()
{
    string str = "hello world!";
    while (1)
    {
        HAL_Delay(100);
        HAL_UART_Transmit(&huart3, (uint8_t *)str.data(), str.size(), 0xff);
    }
}
