#include "components/battery/BatteryController.h"
#include "utility/LinearApproximation.h"
#include "drivers/PinMap.h"
#include "stm32wbxx_hal.h"
#include <algorithm>
#include <cmath>

using namespace Pinetime::Controllers;

Battery* Battery::instance = nullptr;

Battery::Battery() {
  instance = this;
}

void Battery::ReadPowerState() {
  isCharging = (HAL_GPIO_ReadPin(PinMap::ChargingPin.port, PinMap::ChargingPin.pin) == GPIO_PIN_RESET);
  isPowerPresent = !isCharging;

  if (isPowerPresent && !isCharging) {
    isFull = true;
  } else if (!isPowerPresent) {
    isFull = false;
  }
}

void Battery::MeasureVoltage() {
  ReadPowerState();

  if (isReading) {
    return;
  }
  isReading = true;

  voltage = 4000;

  static const Utility::LinearApproximation<uint16_t, uint8_t, 6> approx {
    {{{3500, 0}, {3616, 3}, {3723, 22}, {3776, 48}, {3979, 79}, {4180, 100}}}};

  uint8_t newPercent = 100;
  if (!isFull) {
    newPercent = std::min(approx.GetValue(voltage), isCharging ? uint8_t {99} : uint8_t {100});
  }

  if ((isPowerPresent && newPercent > percentRemaining) || (!isPowerPresent && newPercent < percentRemaining) || firstMeasurement) {
    firstMeasurement = false;
    percentRemaining = newPercent;
    systemTask->PushMessage(System::Messages::BatteryPercentageUpdated);
  }

  isReading = false;
}

void Battery::Register(Pinetime::System::SystemTask* systemTask) {
  this->systemTask = systemTask;
}