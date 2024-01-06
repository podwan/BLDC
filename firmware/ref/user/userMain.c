
#include "userMain.h"
#include "app.h"

void ledFlash(void *argument)
{
    while (1)
    {
        //  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        osDelay(500);
    }
}
osThreadId_t appTaskHandle, ledFlashTaskHandle;
void userMain()
{
    // osThreadId_t ledFlashTaskHandle;
    // osThreadId_t appTaskHandle;

    const osThreadAttr_t ledFlashTask_attributes = {
        .name = "ledFlash",
        .stack_size = 512,
        .priority = (osPriority_t)osPriorityRealtime,
    };
    ledFlashTaskHandle = osThreadNew(ledFlash, NULL, &ledFlashTask_attributes);

    const osThreadAttr_t appTask_attributes = {
        .name = "appTask",
        .stack_size = 1024,
        .priority = (osPriority_t)osPriorityRealtime,
    };
   // appTaskHandle = osThreadNew(appRunning, NULL, &appTask_attributes);
}
/* Timer Callbacks -------------------------------------------------------*/
void OnTimerCallback()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Wake & invoke thread IMMEDIATELY.
    vTaskNotifyGiveFromISR(ledFlashTaskHandle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
