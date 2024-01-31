#include "userMain.h"

/* Thread Definitions -----------------------------------------------------*/
osThreadId_t ctrlLoopTaskHandle;
void ThreadCtrlLoop(void *argument)
{
    motorInit();

    printf("Motor ready.\r\n");

    for (;;)
    {
        // Suspended here until got Notification.
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        move(target);
        loopFOC();
        commander_run();
    }
}

void userMain()
{
    const osThreadAttr_t controlLoopTask_attributes = {
        .name = "ControlLoopTask",
        .stack_size = 4096,
        .priority = (osPriority_t)osPriorityRealtime, // robot control thread is critical, should be the highest
    };
    ctrlLoopTaskHandle = osThreadNew(ThreadCtrlLoop, NULL, &controlLoopTask_attributes);
    HAL_TIM_Base_Start_IT(&htim6);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        // Wake & invoke thread IMMEDIATELY.
        // vTaskNotifyGiveFromISR(TaskHandle_t(ctrlLoopTaskHandle), &xHigherPriorityTaskWoken);
       // vTaskNotifyGiveFromISR(ctrlLoopTaskHandle, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
