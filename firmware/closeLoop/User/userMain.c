#include "userMain.h"

/* Thread Definitions -----------------------------------------------------*/
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
        // commander_run();
    }
}

void communicationLoop(void *argument)
{
    for (;;)
    {

        printf("hello\n");
        osDelay(1000);
    }
}
osThreadId_t ctrlLoopTaskHandle;
osThreadId_t commTaskHandle;
void userMain()
{
    const osThreadAttr_t controlLoopTask_attributes = {
        .name = "ControlLoopTask",
        .stack_size = 256 * 4,
        .priority = (osPriority_t)osPriorityRealtime, // robot control thread is critical, should be the highest
    };
    ctrlLoopTaskHandle = osThreadNew(ThreadCtrlLoop, NULL, &controlLoopTask_attributes);

    const osThreadAttr_t commTask_attributes = {
        .name = "commTask",
        .stack_size = 256 * 2,
        .priority = (osPriority_t)osPriorityNormal, // robot control thread is critical, should be the highest
    };
    commTaskHandle = osThreadNew(communicationLoop, NULL, &commTask_attributes);

    HAL_TIM_Base_Start_IT(&htim6);
}

// void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
// {
//     if (htim->Instance == TIM6)
//     {
//         BaseType_t xHigherPriorityTaskWoken = pdFALSE;

//         // Wake & invoke thread IMMEDIATELY.
//         vTaskNotifyGiveFromISR(ctrlLoopTaskHandle, &xHigherPriorityTaskWoken);
//         portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//     }
// }
