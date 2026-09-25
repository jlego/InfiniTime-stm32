#pragma once
#include <cstdint>
#include <systemtask/SystemTask.h>

namespace Pinetime {
  namespace Controllers {

    class Battery {
    public:
      Battery();

      void ReadPowerState();
      void MeasureVoltage();
      void Register(System::SystemTask* systemTask);

      uint8_t PercentRemaining() const {
        return percentRemaining;
      }

      uint16_t Voltage() const {
        return voltage;
      }

      bool IsCharging() const {
        return isCharging && !isFull;
      }

      bool IsPowerPresent() const {
        return isPowerPresent;
      }

    private:
      static Battery* instance;

      uint16_t voltage = 0;
      uint8_t percentRemaining = 0;

      bool isFull = false;
      bool isCharging = false;
      bool isPowerPresent = false;
      bool firstMeasurement = true;
      bool isReading = false;

      Pinetime::System::SystemTask* systemTask = nullptr;
    };
  }
}