#include "FreeRTOS.h"
#include "task.h"
#include "stm32wbxx_hal.h"

volatile uint32_t ulTimerCountsForOneTick = 0;

void vPortSetupTimerInterrupt(void) {
  ulTimerCountsForOneTick = (SystemCoreClock / configTICK_RATE_HZ);
  HAL_SYSTICK_Config(ulTimerCountsForOneTick);
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
  HAL_NVIC_SetPriority(SysTick_IRQn, configKERNEL_INTERRUPT_PRIORITY, 0);
}

void SysTick_Handler(void) {
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
    xTaskIncrementTick();
    portYIELD_WITHIN_API();
  }
}