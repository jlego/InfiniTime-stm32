#include "FreeRTOS.h"
#include "task.h"
#include "stm32wbxx_hal.h"

volatile uint32_t ulTimerCountsForOneTick = 0;
volatile uint32_t xPendingTicks = 0;
volatile BaseType_t xYieldPending = pdFALSE;

void SysTick_Handler(void) {
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
    xPortSysTickHandler();
  }
}

void vPortSetupTimerInterrupt(void) {
  ulTimerCountsForOneTick = (SystemCoreClock / configTICK_RATE_HZ);
  HAL_SYSTICK_Config(ulTimerCountsForOneTick);
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
  HAL_NVIC_SetPriority(SysTick_IRQn, configKERNEL_INTERRUPT_PRIORITY, 0);
}

void xPortSysTickHandler(void) {
  vPortIncrementTick();
  portYIELD();
}

BaseType_t xPortSysTickHandler(void);