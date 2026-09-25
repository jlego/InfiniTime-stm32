#pragma once
#include "logging/Logger.h"

namespace Pinetime {
  namespace Logging {
    class Stm32Logger : public Logger {
    public:
      void Init() override;
      void Debug(const char* format, ...) override;
      void Info(const char* format, ...) override;
      void Warn(const char* format, ...) override;
      void Error(const char* format, ...) override;
    };
  }
}