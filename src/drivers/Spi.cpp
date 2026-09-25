#include "drivers/Spi.h"
#include "stm32wbxx_hal.h"

using namespace Pinetime::Drivers;

Spi::Spi(SpiMaster& spiMaster, PinMap::GpioPin pinCsn) : spiMaster {spiMaster}, pinCsn {pinCsn} {
}

bool Spi::Init() {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = pinCsn.pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(pinCsn.port, &GPIO_InitStruct);
  HAL_GPIO_WritePin(pinCsn.port, pinCsn.pin, GPIO_PIN_SET);
  initialized = true;
  return true;
}

bool Spi::Write(const uint8_t* data, size_t size, const std::function<void()>& preTransactionHook) {
  auto hook = [this, &preTransactionHook]() {
    HAL_GPIO_WritePin(pinCsn.port, pinCsn.pin, GPIO_PIN_RESET);
    if (preTransactionHook) {
      preTransactionHook();
    }
  };

  auto ok = spiMaster.Write(data, size, hook);

  HAL_GPIO_WritePin(pinCsn.port, pinCsn.pin, GPIO_PIN_SET);
  return ok;
}

bool Spi::Read(uint8_t* cmd, size_t cmdSize, uint8_t* data, size_t dataSize) {
  HAL_GPIO_WritePin(pinCsn.port, pinCsn.pin, GPIO_PIN_RESET);
  bool ok = spiMaster.Read(cmd, cmdSize, data, dataSize);
  HAL_GPIO_WritePin(pinCsn.port, pinCsn.pin, GPIO_PIN_SET);
  return ok;
}

bool Spi::WriteCmdAndBuffer(const uint8_t* cmd, size_t cmdSize, const uint8_t* data, size_t dataSize) {
  HAL_GPIO_WritePin(pinCsn.port, pinCsn.pin, GPIO_PIN_RESET);
  bool ok = spiMaster.Write(cmd, cmdSize, nullptr);
  if (ok) {
    ok = spiMaster.Write(data, dataSize, nullptr);
  }
  HAL_GPIO_WritePin(pinCsn.port, pinCsn.pin, GPIO_PIN_SET);
  return ok;
}

void Spi::Sleep() {
  if (initialized) {
    HAL_GPIO_DeInit(pinCsn.port, pinCsn.pin);
  }
}

void Spi::Wakeup() {
  if (initialized) {
    Init();
  }
}