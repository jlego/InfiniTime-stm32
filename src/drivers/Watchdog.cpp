#include "drivers/Watchdog.h"
#include "stm32wbxx_hal.h"

using namespace Pinetime::Drivers;

static uint8_t watchdogTimeout = 0;
static uint32_t watchdogReload = 0;

void Watchdog::Setup(uint8_t timeoutSeconds, SleepBehaviour sleepBehaviour, HaltBehaviour haltBehaviour) {
  (void)sleepBehaviour;
  (void)haltBehaviour;
  watchdogTimeout = timeoutSeconds;
  watchdogReload = (timeoutSeconds * 32000) / 256;
  if (watchdogReload > 0xFFF)
    watchdogReload = 0xFFF;
}

void Watchdog::Start() {
  IWDG->KR = 0x5555;
  IWDG->PR = 0x06;
  IWDG->RLR = watchdogReload;
  while (IWDG->SR != 0) {}
  IWDG->KR = 0xCCCC;
}

void Watchdog::Kick() {
  IWDG->KR = 0xAAAA;
}

Watchdog::ResetReason Watchdog::GetResetReason() const {
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
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST)) {
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