#pragma once
#include <FreeRTOS.h>
#include <semphr.h>
#include <cstdint>
#include "stm32wbxx_hal.h"

namespace Pinetime {
  namespace Drivers {
    class TwiMaster {
    public:
      enum class ErrorCodes { NoError, TransactionFailed };

      TwiMaster(I2C_HandleTypeDef* hi2c, uint8_t pinSda, uint8_t pinScl);

      void Init();
      ErrorCodes Read(uint8_t deviceAddress, uint8_t registerAddress, uint8_t* buffer, size_t size);
      ErrorCodes Write(uint8_t deviceAddress, uint8_t registerAddress, const uint8_t* data, size_t size);

      void Sleep();
      void Wakeup();

    private:
      I2C_HandleTypeDef* hi2c;
      SemaphoreHandle_t mutex = nullptr;
      uint8_t pinSda;
      uint8_t pinScl;
      bool initialized = false;
    };
  }
}