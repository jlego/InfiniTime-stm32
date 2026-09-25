#include "drivers/Watchdog.h"
#include "stm32wbxx_hal.h"

using namespace Pinetime::Drivers;

static IWDG_HandleTypeDef hiwdg;
static uint8_t watchdogTimeout = 0;

void Watchdog::Setup(uint8_t timeoutSeconds, SleepBehaviour sleepBehaviour, HaltBehaviour haltBehaviour) {
  watchdogTimeout = timeoutSeconds;
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
  hiwdg.Init.Reload = (timeoutSeconds * 32000) / 256;
  if (hiwdg.Init.Reload > 0xFFF)
    hiwdg.Init.Reload = 0xFFF;
  hiwdg.Init.Window = 0xFFF;
}

void Watchdog::Start() {
  HAL_IWDG_Init(&hiwdg);
}

void Watchdog::Kick() {
  HAL_IWDG_Refresh(&hiwdg);
}

Watchdog::ResetReason Watchdog::ResetReason() {
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST)) {
    __HAL_RCC_CLEAR_RESET_FLAGS();
    return ResetReason::Watchdog;
  }
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)) {
    __HAL_RCC_CLEAR_RESET_FLAGS();
    return ResetReason::ResetPin;
  }
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)) {
    __HAL_RCC_CLEAR_RESET_FLAGS();
    return ResetReason::SoftReset;
  }
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST)) {
    __HAL_RCC_CLEAR_RESET_FLAGS();
    return ResetReason::HardReset;
  }
  return ResetReason::HardReset;
}

bool Watchdog::IsRunning() {
  return (IWDG->SR & IWDG_SR_WVU) != 0;
}

bool Watchdog::IsResetRecent() {
  return __HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET;
}