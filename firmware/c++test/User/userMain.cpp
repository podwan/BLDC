#include "userMain.h"
#include "comm.h"
// #include "User/Platform/encoder.h"
// #include "User/Platform/driver.h"
// #include "User/Motor/motor.h"
// #include "User/Motor/knob.h"

void communicationLoop(void *argument)
{
    for (;;)
    {

        commander_run();
        printLog();
        osDelay(500);
    }
}
osThreadId_t commTaskHandle;
// osThreadId_t UserLoopTaskHandle;
// void ThreadUserLoop(void* argument)
// {
//     motor.AttachDriver(&driver);
//     motor.AttachEncoder(&encoder);
//     knob.Init(&motor);
//     knob.SetEnable(true);
//     knob.SetMode(KnobSimulator::MODE_DAMPED);

//     for (;;)
//     {
//         // Suspended here until got Notification.
//         ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

//         knob.Tick();
//     }
// }
void userMain()
{
    const osThreadAttr_t commTask_attributes = {
        "commTask",
        0,
        NULL,
        0,
        NULL,
        256 * 2,
        (osPriority_t)osPriorityNormal, // robot control thread is critical, should be the highest
        0,
        0,
    };
    commTaskHandle = osThreadNew(communicationLoop, NULL, &commTask_attributes);
}


