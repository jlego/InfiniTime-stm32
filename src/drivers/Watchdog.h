#pragma once
#include <cstdint>
#include "stm32wbxx_hal.h"

namespace Pinetime {
  namespace Drivers {
    class Watchdog {
    public:
      enum class ResetReason { ResetPin, Watchdog, SoftReset, CpuLockup, SystemOff, LpComp, DebugInterface, NFC, HardReset };

      enum class SleepBehaviour : uint8_t {
        Pause = 0,
        Run = 1
      };

      enum class HaltBehaviour : uint8_t {
        Pause = 0,
        Run = 1
      };

      void Setup(uint8_t timeoutSeconds, SleepBehaviour sleepBehaviour, HaltBehaviour haltBehaviour);
      void Start();
      void Kick();

      ResetReason ResetReason();
      bool IsRunning();
      bool IsResetRecent();
    };
  }
}