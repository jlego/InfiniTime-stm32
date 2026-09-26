#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "stm32wbxx_hal.h"

#define FREERTOS_USE_SYSTICK 1

#define configUSE_PREEMPTION                    1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#define configUSE_TICKLESS_IDLE                 0
#define configCPU_CLOCK_HZ                      (SystemCoreClock)
#define configTICK_RATE_HZ                      1024
#define configMAX_PRIORITIES                    (3)
#define configMINIMAL_STACK_SIZE                (120)
#define configMAX_TASK_NAME_LEN                 (4)
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 1
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_COUNTING_SEMAPHORES           1
#define configUSE_QUEUE_SETS                    0
#define configUSE_TIME_SLICING                  0
#define configUSE_NEWLIB_REENTRANT              0
#define configENABLE_BACKWARD_COMPATIBILITY     0
#define configUSE_TASK_NOTIFICATIONS            1

#define configUSE_IDLE_HOOK            0
#define configUSE_TICK_HOOK            0
#define configCHECK_FOR_STACK_OVERFLOW 1
#define configUSE_MALLOC_FAILED_HOOK   1

#define configGENERATE_RUN_TIME_STATS        0
#define configUSE_TRACE_FACILITY             1
#define configUSE_STATS_FORMATTING_FUNCTIONS 0

#define configUSE_CO_ROUTINES           0
#define configMAX_CO_ROUTINE_PRIORITIES (2)

#define configUSE_TIMERS             1
#define configTIMER_TASK_PRIORITY    (1)
#define configTIMER_QUEUE_LENGTH     32
#define configTIMER_TASK_STACK_DEPTH (300)

#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP 2

#if defined(DEBUG)
  #define configASSERT(x) if((x) == 0) { for(;;); }
#endif

#define configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS 1

#define INCLUDE_vTaskPrioritySet               1
#define INCLUDE_uxTaskPriorityGet              1
#define INCLUDE_vTaskDelete                    1
#define INCLUDE_vTaskSuspend                   1
#define INCLUDE_xResumeFromISR                 1
#define INCLUDE_vTaskDelayUntil                1
#define INCLUDE_vTaskDelay                     1
#define INCLUDE_xTaskGetSchedulerState         1
#define INCLUDE_xTaskGetCurrentTaskHandle      1
#define INCLUDE_uxTaskGetStackHighWaterMark    1
#define INCLUDE_xTaskGetIdleTaskHandle         1
#define INCLUDE_xTimerGetTimerDaemonTaskHandle 1
#define INCLUDE_pcTaskGetTaskName              1
#define INCLUDE_eTaskGetState                  1
#define INCLUDE_xEventGroupSetBitFromISR       1
#define INCLUDE_xTimerPendFunctionCall         1

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY 0xf
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5

#define configKERNEL_INTERRUPT_PRIORITY configLIBRARY_LOWEST_INTERRUPT_PRIORITY
#define configMAX_SYSCALL_INTERRUPT_PRIORITY configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY

#define vPortSVCHandler    SVC_Handler
#define xPortPendSVHandler PendSV_Handler

#define xPortSysTickHandler SysTick_Handler

#if !(defined(__ASSEMBLY__) || defined(__ASSEMBLER__))
  #ifdef __NVIC_PRIO_BITS
    #define configPRIO_BITS __NVIC_PRIO_BITS
  #else
    #define configPRIO_BITS 4
  #endif

  #if (FREERTOS_USE_SYSTICK)
    #include <stdint.h>
    extern uint32_t SystemCoreClock;
  #endif
#endif

#define configUSE_DISABLE_TICK_AUTO_CORRECTION_DEBUG 0

#endif /* FREERTOS_CONFIG_H */