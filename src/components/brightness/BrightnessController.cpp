#include "components/brightness/BrightnessController.h"
#include "displayapp/screens/Symbols.h"
#include "drivers/PinMap.h"

using namespace Pinetime::Controllers;
using namespace Pinetime::Applications::Screens;

void BrightnessController::Init() {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = PinMap::LcdBacklightPin.pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
  HAL_GPIO_Init(PinMap::LcdBacklightPin.port, &GPIO_InitStruct);

  Set(level);
}

void BrightnessController::ApplyBrightness(uint8_t percent) {
  (void)percent;
}

void BrightnessController::Set(BrightnessController::Levels level) {
  this->level = level;
  switch (level) {
    default:
    case Levels::High:
      HAL_GPIO_WritePin(PinMap::LcdBacklightPin.port, PinMap::LcdBacklightPin.pin, GPIO_PIN_SET);
      break;
    case Levels::Medium:
      HAL_GPIO_WritePin(PinMap::LcdBacklightPin.port, PinMap::LcdBacklightPin.pin, GPIO_PIN_SET);
      break;
    case Levels::Low:
      HAL_GPIO_WritePin(PinMap::LcdBacklightPin.port, PinMap::LcdBacklightPin.pin, GPIO_PIN_SET);
      break;
    case Levels::AlwaysOn:
      HAL_GPIO_WritePin(PinMap::LcdBacklightPin.port, PinMap::LcdBacklightPin.pin, GPIO_PIN_SET);
      break;
    case Levels::Off:
      HAL_GPIO_WritePin(PinMap::LcdBacklightPin.port, PinMap::LcdBacklightPin.pin, GPIO_PIN_RESET);
      break;
  }
}

void BrightnessController::Lower() {
  switch (level) {
    case Levels::High:
      Set(Levels::Medium);
      break;
    case Levels::Medium:
      Set(Levels::Low);
      break;
    case Levels::Low:
      Set(Levels::Off);
      break;
    default:
      break;
  }
}

void BrightnessController::Higher() {
  switch (level) {
    case Levels::Off:
      Set(Levels::Low);
      break;
    case Levels::Low:
      Set(Levels::Medium);
      break;
    case Levels::Medium:
      Set(Levels::High);
      break;
    default:
      break;
  }
}

void BrightnessController::Step() {
  switch (level) {
    case Levels::Off:
      Set(Levels::Low);
      break;
    case Levels::Low:
      Set(Levels::Medium);
      break;
    case Levels::Medium:
      Set(Levels::High);
      break;
    case Levels::High:
      Set(Levels::Off);
      break;
    default:
      break;
  }
}

const char* BrightnessController::GetIcon() {
  switch (level) {
    case Levels::Off:
      return Symbols::brightnessLow;
    case Levels::Low:
      return Symbols::brightnessLow;
    case Levels::Medium:
      return Symbols::brightnessMedium;
    case Levels::High:
      return Symbols::brightnessHigh;
    default:
      return Symbols::brightnessHigh;
  }
}

const char* BrightnessController::ToString() {
  switch (level) {
    case Levels::Off:
      return "Off";
    case Levels::Low:
      return "Low";
    case Levels::Medium:
      return "Medium";
    case Levels::High:
      return "High";
    case Levels::AlwaysOn:
      return "AlwaysOn";
    default:
      return "???";
  }
}

BrightnessController::Levels BrightnessController::Level() const {
  return level;
}