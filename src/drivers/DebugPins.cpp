#include "stm32wbxx_hal.h"

#ifdef USE_DEBUG_PINS

enum debugpins_pins {
  DebugPin0 = 0,
  DebugPin1 = 1,
  DebugPin2 = 2,
  DebugPin3 = 3,
  DebugPin4 = 4
};

void debugpins_init() {
}

void debugpins_set(debugpins_pins pin) {
  (void)pin;
}

void debugpins_clear(debugpins_pins pin) {
  (void)pin;
}

void debugpins_pulse(debugpins_pins pin) {
  (void)pin;
}
#else

enum debugpins_pins {
  DebugPin0 = 0,
  DebugPin1 = 1,
  DebugPin2 = 2,
  DebugPin3 = 3,
  DebugPin4 = 4
};

void debugpins_init() {
}

void debugpins_set(debugpins_pins pin) {
}

void debugpins_clear(debugpins_pins pin) {
}

void debugpins_pulse(debugpins_pins pin) {
}

#endif