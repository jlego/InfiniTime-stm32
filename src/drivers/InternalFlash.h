#pragma once
#include <cstdint>
#include "stm32wbxx_hal.h"

namespace Pinetime {
  namespace Drivers {
    class InternalFlash {
    public:
      static void ErasePage(uint32_t address);
      static void WriteWord(uint32_t address, uint32_t value);

    private:
      static inline void Wait();
    };
  }
}