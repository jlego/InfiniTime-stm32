#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>

#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include "stm32wbxx_hal.h"

namespace Pinetime {
  namespace Drivers {
    class Spi;

    class SpiMaster {
    public:
      explicit SpiMaster(SPI_HandleTypeDef* hspi);
      SpiMaster(const SpiMaster&) = delete;
      SpiMaster& operator=(const SpiMaster&) = delete;
      SpiMaster(SpiMaster&&) = delete;
      SpiMaster& operator=(SpiMaster&&) = delete;

      bool Init();
      bool Write(const uint8_t* data, size_t size, const std::function<void()>& preTransactionHook);
      bool Read(uint8_t* cmd, size_t cmdSize, uint8_t* data, size_t dataSize);

      void Sleep();
      void Wakeup();

      SPI_HandleTypeDef* GetHandle() { return hspi; }

    private:
      void ConfigureSpi();
      void SetupCsnPin();

      SPI_HandleTypeDef* hspi;
      SemaphoreHandle_t mutex = nullptr;
      bool initialized = false;
    };
  }
}