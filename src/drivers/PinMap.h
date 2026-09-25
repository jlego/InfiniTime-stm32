#pragma once
#include <cstdint>
#include "stm32wbxx_hal.h"

namespace Pinetime {
  namespace PinMap {

    static constexpr uint8_t Charging = 0;
    static constexpr uint8_t Cst816sReset = 14;
    static constexpr uint8_t Button = 2;
    static constexpr uint8_t ButtonEnable = 3;
    static constexpr uint8_t Cst816sIrq = 2;
    static constexpr uint8_t PowerPresent = 0;
    static constexpr uint8_t Bma421Irq = 3;

    static constexpr uint8_t Motor = 1;

    static constexpr uint8_t LcdBacklightLow = 0;
    static constexpr uint8_t LcdBacklightMedium = 0;
    static constexpr uint8_t LcdBacklightHigh = 0;

    static constexpr uint8_t SpiSck = 5;
    static constexpr uint8_t SpiMosi = 5;
    static constexpr uint8_t SpiMiso = 6;

    static constexpr uint8_t SpiFlashCsn = 15;
    static constexpr uint8_t SpiLcdCsn = 15;
    static constexpr uint8_t LcdDataCommand = 0;
    static constexpr uint8_t LcdReset = 1;

    static constexpr uint8_t TwiScl = 6;
    static constexpr uint8_t TwiSda = 7;

    struct GpioPin {
      GPIO_TypeDef* port;
      uint16_t pin;
    };

    static constexpr GpioPin LcdResetPin = {GPIOB, GPIO_PIN_1};
    static constexpr GpioPin LcdDataCommandPin = {GPIOB, GPIO_PIN_0};
    static constexpr GpioPin LcdChipSelectPin = {GPIOA, GPIO_PIN_15};
    static constexpr GpioPin LcdBacklightPin = {GPIOA, GPIO_PIN_0};

    static constexpr GpioPin Spi1SckPin = {GPIOA, GPIO_PIN_5};
    static constexpr GpioPin Spi1MosiPin = {GPIOB, GPIO_PIN_5};
    static constexpr GpioPin Spi1MisoPin = {GPIOA, GPIO_PIN_6};

    static constexpr GpioPin SpiFlashCsnPin = {GPIOA, GPIO_PIN_4};
    static constexpr GpioPin SpiLcdCsnPin = {GPIOA, GPIO_PIN_15};

    static constexpr GpioPin Twi1SclPin = {GPIOB, GPIO_PIN_6};
    static constexpr GpioPin Twi1SdaPin = {GPIOB, GPIO_PIN_7};

    static constexpr GpioPin TouchIntPin = {GPIOB, GPIO_PIN_2};
    static constexpr GpioPin TouchResetPin = {GPIOA, GPIO_PIN_14};

    static constexpr GpioPin MotorPin = {GPIOB, GPIO_PIN_1};

    static constexpr GpioPin ButtonPin = {GPIOB, GPIO_PIN_2};
    static constexpr GpioPin ButtonEnablePin = {GPIOB, GPIO_PIN_3};
    static constexpr GpioPin ChargingPin = {GPIOA, GPIO_PIN_0};
  }
}