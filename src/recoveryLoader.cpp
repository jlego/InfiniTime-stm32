#include "stm32wbxx_hal.h"
#include <drivers/SpiMaster.h>
#include <drivers/Spi.h>
#include <drivers/SpiNorFlash.h>
#include <FreeRTOS.h>
#include <task.h>
#include <cstring>
#include <drivers/St7789.h>
#include <components/brightness/BrightnessController.h>
#include <algorithm>
#include "recoveryImage.h"
#include "drivers/PinMap.h"

#include "displayapp/icons/infinitime/infinitime-nb.c"
#include "components/rle/RleDecoder.h"
#include "logging/Stm32Logger.h"

Pinetime::Logging::Stm32Logger logger;

static constexpr uint8_t displayWidth = 240;
static constexpr uint8_t displayHeight = 240;
static constexpr uint8_t bytesPerPixel = 2;

static constexpr uint16_t colorWhite = 0xFFFF;
static constexpr uint16_t colorGreen = 0xE007;

static SPI_HandleTypeDef hspi1;

Pinetime::Drivers::SpiMaster spi {&hspi1};
Pinetime::Drivers::Spi flashSpi {spi, Pinetime::PinMap::SpiFlashCsnPin};
Pinetime::Drivers::SpiNorFlash spiNorFlash {flashSpi};

Pinetime::Drivers::Spi lcdSpi {spi, Pinetime::PinMap::SpiLcdCsnPin};
Pinetime::Drivers::St7789 lcd {lcdSpi, 0, 0};

Pinetime::Controllers::BrightnessController brightnessController;

void DisplayProgressBar(uint8_t percent, uint16_t color);
void DisplayLogo();

extern "C" {
void vApplicationIdleHook(void) {
}
}

void RefreshWatchdog() {
}

uint8_t displayBuffer[displayWidth * bytesPerPixel];

void Process(void* /*instance*/) {
  RefreshWatchdog();

  spi.Init();
  spiNorFlash.Init();
  spiNorFlash.Wakeup();
  brightnessController.Init();
  lcd.Init();

  DisplayLogo();

  for (uint32_t erased = 0; erased < sizeof(recoveryImage); erased += 0x1000) {
    spiNorFlash.SectorErase(erased);
    RefreshWatchdog();
  }

  static constexpr uint32_t memoryChunkSize = 200;
  uint8_t writeBuffer[memoryChunkSize];
  for (size_t offset = 0; offset < sizeof(recoveryImage); offset += memoryChunkSize) {
    std::memcpy(writeBuffer, &recoveryImage[offset], memoryChunkSize);
    spiNorFlash.Write(offset, writeBuffer, memoryChunkSize);
    DisplayProgressBar((static_cast<float>(offset) / static_cast<float>(sizeof(recoveryImage))) * 100.0f, colorWhite);
    RefreshWatchdog();
  }
  DisplayProgressBar(100.0f, colorGreen);

  while (1) {
    asm("nop");
  }
}

void DisplayLogo() {
  Pinetime::Tools::RleDecoder rleDecoder(infinitime_nb, sizeof(infinitime_nb));
  for (int i = 0; i < displayWidth; i++) {
    rleDecoder.DecodeNext(displayBuffer, displayWidth * bytesPerPixel);
    lcd.DrawBuffer(0, i, displayWidth, 1, reinterpret_cast<const uint8_t*>(displayBuffer), displayWidth * bytesPerPixel);
  }
}

void DisplayProgressBar(uint8_t percent, uint16_t color) {
  static constexpr uint8_t barHeight = 20;
  std::fill(displayBuffer, displayBuffer + (displayWidth * bytesPerPixel), color);
  for (int i = 0; i < barHeight; i++) {
    uint16_t barWidth = std::min(static_cast<float>(percent) * 2.4f, static_cast<float>(displayWidth));
    lcd.DrawBuffer(0, displayWidth - barHeight + i, barWidth, 1, reinterpret_cast<const uint8_t*>(displayBuffer), barWidth * bytesPerPixel);
  }
}

int mallocFailedCount = 0;
int stackOverflowCount = 0;
extern "C" {
void vApplicationMallocFailedHook() {
  mallocFailedCount++;
}

void vApplicationStackOverflowHook(TaskHandle_t /*xTask*/, char* /*pcTaskName*/) {
  stackOverflowCount++;
}
}

void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 32;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK4 | RCC_CLOCKTYPE_HCLK2
                              | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK2Divider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.AHBCLK4Divider = RCC_SYSCLK_DIV1;

  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3);
  HAL_RCCEx_EnableMSIPLLMode();
}

int main(void) {
  HAL_Init();
  SystemClock_Config();
  logger.Init();

  TaskHandle_t taskHandle;
  RefreshWatchdog();

  if (pdPASS != xTaskCreate(Process, "MAIN", 512, nullptr, 0, &taskHandle)) {
  }

  vTaskStartScheduler();

  for (;;) {
  }
}