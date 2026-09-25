#include <cstring>
#include "drivers/St7789.h"
#include "drivers/PinMap.h"
#include "drivers/Spi.h"
#include "stm32wbxx_hal.h"
#include "task.h"

using namespace Pinetime::Drivers;

St7789::St7789(Spi& spi, uint8_t pinDataCommand, uint8_t pinReset) : spi {spi}, pinDataCommand {pinDataCommand}, pinReset {pinReset} {
}

void St7789::Init() {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitStruct.Pin = PinMap::LcdDataCommandPin.pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(PinMap::LcdDataCommandPin.port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = PinMap::LcdResetPin.pin;
  HAL_GPIO_Init(PinMap::LcdResetPin.port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = PinMap::LcdChipSelectPin.pin;
  HAL_GPIO_Init(PinMap::LcdChipSelectPin.port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = PinMap::LcdBacklightPin.pin;
  HAL_GPIO_Init(PinMap::LcdBacklightPin.port, &GPIO_InitStruct);

  HAL_GPIO_WritePin(PinMap::LcdResetPin.port, PinMap::LcdResetPin.pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(PinMap::LcdChipSelectPin.port, PinMap::LcdChipSelectPin.pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(PinMap::LcdBacklightPin.port, PinMap::LcdBacklightPin.pin, GPIO_PIN_SET);

  HardwareReset();
  SoftwareReset();
  Command2Enable();
  PixelFormat();
  MemoryDataAccessControl();
  SetAddrWindow(0, 0, Width, Height);
  DisplayInversionOn();
  PorchSet();
  FrameRateNormalSet();
  IdleFrameRateOff();
  NormalModeOn();
  SetVdv();
  PowerControl();
  GateControl();
  SleepOut();
  DisplayOn();
}

void St7789::WriteData(uint8_t data) {
  WriteData(&data, 1);
}

void St7789::WriteData(const uint8_t* data, size_t size) {
  WriteSpi(data, size, []() {
    HAL_GPIO_WritePin(PinMap::LcdDataCommandPin.port, PinMap::LcdDataCommandPin.pin, GPIO_PIN_SET);
  });
}

void St7789::WriteCommand(uint8_t data) {
  WriteCommand(&data, 1);
}

void St7789::WriteCommand(const uint8_t* data, size_t size) {
  WriteSpi(data, size, []() {
    HAL_GPIO_WritePin(PinMap::LcdDataCommandPin.port, PinMap::LcdDataCommandPin.pin, GPIO_PIN_RESET);
  });
}

void St7789::WriteSpi(const uint8_t* data, size_t size, const std::function<void()>& preTransactionHook) {
  spi.Write(data, size, preTransactionHook);
}

void St7789::SoftwareReset() {
  EnsureSleepOutPostDelay();
  WriteCommand(static_cast<uint8_t>(Commands::SoftwareReset));
  sleepIn = true;
  lastSleepExit = xTaskGetTickCount();
  vTaskDelay(pdMS_TO_TICKS(125));
}

void St7789::Command2Enable() {
  WriteCommand(static_cast<uint8_t>(Commands::Command2Enable));
  constexpr uint8_t args[] = {
    0x5a, 0x69, 0x02, 0x01,
  };
  WriteData(args, sizeof(args));
}

void St7789::SleepOut() {
  if (!sleepIn) {
    return;
  }
  WriteCommand(static_cast<uint8_t>(Commands::SleepOut));
  vTaskDelay(pdMS_TO_TICKS(6));
  lastSleepExit = xTaskGetTickCount();
  sleepIn = false;
}

void St7789::EnsureSleepOutPostDelay() {
  TickType_t delta = xTaskGetTickCount() - lastSleepExit;
  if (delta < pdMS_TO_TICKS(125)) {
    vTaskDelay(pdMS_TO_TICKS(125) - delta);
  }
}

void St7789::SleepIn() {
  if (sleepIn) {
    return;
  }
  EnsureSleepOutPostDelay();
  WriteCommand(static_cast<uint8_t>(Commands::SleepIn));
  vTaskDelay(pdMS_TO_TICKS(6));
  sleepIn = true;
}

void St7789::PixelFormat() {
  WriteCommand(static_cast<uint8_t>(Commands::ColumnFormat));
  WriteData(0x55);
}

void St7789::MemoryDataAccessControl() {
  WriteCommand(static_cast<uint8_t>(Commands::MemoryDataAccessControl));
  WriteData(0x00);
}

void St7789::DisplayInversionOn() {
  WriteCommand(static_cast<uint8_t>(Commands::DisplayInversionOn));
}

void St7789::NormalModeOn() {
  WriteCommand(static_cast<uint8_t>(Commands::NormalModeOn));
}

void St7789::IdleModeOn() {
  WriteCommand(static_cast<uint8_t>(Commands::IdleModeOn));
}

void St7789::IdleModeOff() {
  WriteCommand(static_cast<uint8_t>(Commands::IdleModeOff));
}

void St7789::PorchSet() {
  WriteCommand(static_cast<uint8_t>(Commands::PorchControl));
  constexpr uint8_t args[] = {
    0x02, 0x03, 0x01, 0xed, 0xed,
  };
  WriteData(args, sizeof(args));
}

void St7789::FrameRateNormalSet() {
  WriteCommand(static_cast<uint8_t>(Commands::FrameRateNormal));
  WriteData(0x0a);
}

void St7789::IdleFrameRateOn() {
  WriteCommand(static_cast<uint8_t>(Commands::FrameRateIdle));
  constexpr uint8_t args[] = {
    0x12, 0x1e, 0x1e,
  };
  WriteData(args, sizeof(args));
}

void St7789::IdleFrameRateOff() {
  WriteCommand(static_cast<uint8_t>(Commands::FrameRateIdle));
  constexpr uint8_t args[] = {
    0x00, 0x0a, 0x0a,
  };
  WriteData(args, sizeof(args));
}

void St7789::DisplayOn() {
  WriteCommand(static_cast<uint8_t>(Commands::DisplayOn));
}

void St7789::PowerControl() {
  WriteCommand(static_cast<uint8_t>(Commands::PowerControl1));
  constexpr uint8_t args[] = {
    0xa4, 0x00,
  };
  WriteData(args, sizeof(args));

  WriteCommand(static_cast<uint8_t>(Commands::PowerControl2));
  WriteData(0xb3);
}

void St7789::GateControl() {
  WriteCommand(static_cast<uint8_t>(Commands::GateControl));
  WriteData(0x00);
}

void St7789::SetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  WriteCommand(static_cast<uint8_t>(Commands::ColumnAddressSet));
  uint8_t colArgs[] = {
    static_cast<uint8_t>(x0 >> 8),
    static_cast<uint8_t>(x0),
    static_cast<uint8_t>(x1 >> 8),
    static_cast<uint8_t>(x1)
  };
  WriteData(colArgs, sizeof(colArgs));

  WriteCommand(static_cast<uint8_t>(Commands::RowAddressSet));
  uint8_t rowArgs[] = {
    static_cast<uint8_t>(y0 >> 8),
    static_cast<uint8_t>(y0),
    static_cast<uint8_t>(y1 >> 8),
    static_cast<uint8_t>(y1)
  };
  WriteData(rowArgs, sizeof(rowArgs));
}

void St7789::WriteToRam(const uint8_t* data, size_t size) {
  WriteCommand(static_cast<uint8_t>(Commands::WriteToRam));
  WriteData(data, size);
}

void St7789::SetVdv() {
  WriteCommand(static_cast<uint8_t>(Commands::VdvSetting));
  WriteData(0x10);
}

void St7789::DisplayOff() {
  WriteCommand(static_cast<uint8_t>(Commands::DisplayOff));
}

void St7789::VerticalScrollStartAddress(uint16_t line) {
  verticalScrollingStartAddress = line;
  WriteCommand(static_cast<uint8_t>(Commands::VerticalScrollStartAddress));
  uint8_t args[] = {
    static_cast<uint8_t>(line >> 8),
    static_cast<uint8_t>(line)
  };
  WriteData(args, sizeof(args));
}

void St7789::Uninit() {
}

void St7789::DrawBuffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t* data, size_t size) {
  SetAddrWindow(x, y, x + width - 1, y + height - 1);
  WriteToRam(data, size);
}

void St7789::HardwareReset() {
  HAL_GPIO_WritePin(PinMap::LcdResetPin.port, PinMap::LcdResetPin.pin, GPIO_PIN_RESET);
  vTaskDelay(pdMS_TO_TICKS(1));
  HAL_GPIO_WritePin(PinMap::LcdResetPin.port, PinMap::LcdResetPin.pin, GPIO_PIN_SET);
  sleepIn = true;
  lastSleepExit = xTaskGetTickCount();
  vTaskDelay(pdMS_TO_TICKS(125));
}

void St7789::LowPowerOn() {
  IdleModeOn();
  IdleFrameRateOn();
}

void St7789::LowPowerOff() {
  IdleModeOff();
  IdleFrameRateOff();
}

void St7789::Sleep() {
  SleepIn();
  HAL_GPIO_DeInit(PinMap::LcdDataCommandPin.port, PinMap::LcdDataCommandPin.pin);
}

void St7789::Wakeup() {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = PinMap::LcdDataCommandPin.pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(PinMap::LcdDataCommandPin.port, &GPIO_InitStruct);

  SleepOut();
  DisplayOn();
}