#pragma once

#include <FreeRTOS.h>
#include <timers.h>

void DebounceTimerCallback(TimerHandle_t xTimer);

extern int mallocFailedCount;
extern int stackOverflowCount;