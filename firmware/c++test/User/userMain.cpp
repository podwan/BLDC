#include "userMain.h"
#include "comm.h"

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
//   const char                   *name;   ///< name of the thread
//   uint32_t                 attr_bits;   ///< attribute bits
//   void                      *cb_mem;    ///< memory for control block
//   uint32_t                   cb_size;   ///< size of provided memory for control block
//   void                   *stack_mem;    ///< memory for stack
//   uint32_t                stack_size;   ///< size of stack
//   osPriority_t              priority;   ///< initial thread priority (default: osPriorityNormal)
//   TZ_ModuleId_t            tz_module;   ///< TrustZone module identifier
//   uint32_t                  reserved;   ///< reserved (must be 0)

//.name = "ControlLoopTask",
//    .stack_size = 4096,
//    .priority = (osPriority_t)osPriorityRealtime, // robot control thread is critical, should be the highest
//}
;
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


