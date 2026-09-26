#pragma once

namespace Pinetime {
  namespace Logging {
    class Logger {
    public:
      virtual void Init() = 0;
      virtual void Resume() = 0;
      virtual void Debug(const char* format, ...) = 0;
      virtual void Info(const char* format, ...) = 0;
      virtual void Warn(const char* format, ...) = 0;
      virtual void Error(const char* format, ...) = 0;
    };
  }
}