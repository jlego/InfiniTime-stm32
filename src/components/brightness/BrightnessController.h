#pragma once

#include <cstdint>
#include "stm32wbxx_hal.h"

namespace Pinetime {
  namespace Controllers {
    class BrightnessController {
    public:
      enum class Levels { Off, AlwaysOn, Low, Medium, High };
      void Init();

      void Set(Levels level);
      Levels Level() const;
      void Lower();
      void Higher();
      void Step();

      const char* GetIcon();
      const char* ToString();

    private:
      Levels level = Levels::High;
      void ApplyBrightness(uint8_t percent);
    };
  }
}