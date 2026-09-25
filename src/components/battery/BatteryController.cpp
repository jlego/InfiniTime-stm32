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

  ADC_HandleTypeDef hadc1;
  ADC_ChannelConfTypeDef sConfig = {0};

  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  HAL_ADC_Init(&hadc1);

  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_247CYCLES_5;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);

  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
  uint32_t rawValue = HAL_ADC_GetValue(&hadc1);
  HAL_ADC_Stop(&hadc1);

  static const Utility::LinearApproximation<uint16_t, uint8_t, 6> approx {
    {{{3500, 0}, {3616, 3}, {3723, 22}, {3776, 48}, {3979, 79}, {4180, 100}}}};

  voltage = static_cast<uint16_t>(rawValue * 3300 / 4096 * 8);

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