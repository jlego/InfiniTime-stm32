#include "components/motor/MotorController.h"
#include "stm32wbxx_hal.h"
#include "systemtask/SystemTask.h"
#include "drivers/PinMap.h"

using namespace Pinetime::Controllers;

void MotorController::Init() {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitStruct.Pin = PinMap::MotorPin.pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(PinMap::MotorPin.port, &GPIO_InitStruct);

  HAL_GPIO_WritePin(PinMap::MotorPin.port, PinMap::MotorPin.pin, GPIO_PIN_SET);

  shortVib = xTimerCreate("shortVib", 1, pdFALSE, nullptr, StopMotor);
  longVib = xTimerCreate("longVib", pdMS_TO_TICKS(1000), pdTRUE, this, Ring);
}

void MotorController::Ring(TimerHandle_t xTimer) {
  auto* motorController = static_cast<MotorController*>(pvTimerGetTimerID(xTimer));
  motorController->RunForDuration(50);
}

void MotorController::RunForDuration(uint8_t motorDuration) {
  if (motorDuration > 0 && xTimerChangePeriod(shortVib, pdMS_TO_TICKS(motorDuration), 0) == pdPASS && xTimerStart(shortVib, 0) == pdPASS) {
    HAL_GPIO_WritePin(PinMap::MotorPin.port, PinMap::MotorPin.pin, GPIO_PIN_RESET);
  }
}

void MotorController::StartRinging() {
  RunForDuration(50);
  xTimerStart(longVib, 0);
}

void MotorController::StopRinging() {
  xTimerStop(longVib, 0);
  StopMotor(shortVib);
}

void MotorController::StopMotor(TimerHandle_t xTimer) {
  HAL_GPIO_WritePin(PinMap::MotorPin.port, PinMap::MotorPin.pin, GPIO_PIN_SET);
}

bool MotorController::IsRinging() {
  return (xTimerIsTimerActive(longVib) == pdTRUE);
}