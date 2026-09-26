#include "logging/Stm32Logger.h"
#include <cstdarg>
#include <cstdio>

using namespace Pinetime::Logging;

void Stm32Logger::Init() {
}

void Stm32Logger::Resume() {
}

void Stm32Logger::Debug(const char* format, ...) {
#ifdef DEBUG
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("\n");
#endif
}

void Stm32Logger::Info(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("\n");
}

void Stm32Logger::Warn(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("\n");
}

void Stm32Logger::Error(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("\n");
}