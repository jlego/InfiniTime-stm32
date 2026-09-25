#include "drivers/Cst816s.h"
#include <FreeRTOS.h>
#include <array>
#include <task.h>
#include "drivers/PinMap.h"
#include "stm32wbxx_hal.h"

using namespace Pinetime::Drivers;

Cst816S::Cst816S(TwiMaster& twiMaster, uint8_t twiAddress) : twiMaster {twiMaster}, twiAddress {twiAddress} {
}

bool Cst816S::Init() {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = PinMap::TouchResetPin.pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(PinMap::TouchResetPin.port, &GPIO_InitStruct);

  HAL_GPIO_WritePin(PinMap::TouchResetPin.port, PinMap::TouchResetPin.pin, GPIO_PIN_RESET);
  vTaskDelay(5);
  HAL_GPIO_WritePin(PinMap::TouchResetPin.port, PinMap::TouchResetPin.pin, GPIO_PIN_SET);
  vTaskDelay(50);

  uint8_t dummy;
  twiMaster.Read(twiAddress, 0x15, &dummy, 1);
  vTaskDelay(5);
  twiMaster.Read(twiAddress, 0xa7, &dummy, 1);
  vTaskDelay(5);

  CheckDeviceIds();

  static constexpr uint8_t motionMask = 0b00000101;
  twiMaster.Write(twiAddress, 0xEC, &motionMask, 1);

  static constexpr uint8_t irqCtl = 0b01110000;
  twiMaster.Write(twiAddress, 0xFA, &irqCtl, 1);

  twiMaster.Write(twiAddress, 0xFB, 0, 1);

  return true;
}

Cst816S::TouchInfos Cst816S::GetTouchInfo() {
  Cst816S::TouchInfos info;
  std::array<uint8_t, 6> touchData {};

  constexpr uint8_t addressOffset = 1;
  auto ret = twiMaster.Read(twiAddress, addressOffset, touchData.data(), sizeof(touchData));
  if (ret != TwiMaster::ErrorCodes::NoError) {
    info.isValid = false;
    return info;
  }

  uint8_t nbTouchPoints = touchData[touchPointNumIndex - addressOffset] & 0x0f;
  uint8_t xHigh = touchData[touchXHighIndex - addressOffset] & 0x0f;
  uint8_t xLow = touchData[touchXLowIndex - addressOffset];
  uint16_t x = (xHigh << 8) | xLow;
  uint8_t yHigh = touchData[touchYHighIndex - addressOffset] & 0x0f;
  uint8_t yLow = touchData[touchYLowIndex - addressOffset];
  uint16_t y = (yHigh << 8) | yLow;
  Gestures gesture = static_cast<Gestures>(touchData[gestureIndex - addressOffset]);

  if (x >= maxX || y >= maxY ||
      (gesture != Gestures::None && gesture != Gestures::SlideDown && gesture != Gestures::SlideUp && gesture != Gestures::SlideLeft &&
       gesture != Gestures::SlideRight && gesture != Gestures::SingleTap && gesture != Gestures::DoubleTap &&
       gesture != Gestures::LongPress)) {
    info.isValid = false;
    return info;
  }

  info.x = x;
  info.y = y;
  info.touching = (nbTouchPoints > 0);
  info.gesture = gesture;
  info.isValid = true;
  return info;
}

void Cst816S::Sleep() {
  HAL_GPIO_WritePin(PinMap::TouchResetPin.port, PinMap::TouchResetPin.pin, GPIO_PIN_RESET);
  vTaskDelay(5);
  HAL_GPIO_WritePin(PinMap::TouchResetPin.port, PinMap::TouchResetPin.pin, GPIO_PIN_SET);
  vTaskDelay(50);
  static constexpr uint8_t sleepValue = 0x03;
  twiMaster.Write(twiAddress, 0xA5, &sleepValue, 1);
}

void Cst816S::Wakeup() {
  Init();
}

bool Cst816S::CheckDeviceIds() {
  if (twiMaster.Read(twiAddress, 0xA7, &chipId, 1) == TwiMaster::ErrorCodes::TransactionFailed) {
    chipId = 0xFF;
    return false;
  }
  if (twiMaster.Read(twiAddress, 0xA8, &vendorId, 1) == TwiMaster::ErrorCodes::TransactionFailed) {
    vendorId = 0xFF;
    return false;
  }
  return true;
}