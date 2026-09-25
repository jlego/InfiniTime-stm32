#include "stm32wbxx_hal.h"
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <drivers/Hrs3300.h>
#include <drivers/Bma421.h>

#include "BootloaderVersion.h"
#include "components/battery/BatteryController.h"
#include "components/ble/BleController.h"
#include "components/ble/NotificationManager.h"
#include "components/brightness/BrightnessController.h"
#include "components/motor/MotorController.h"
#include "components/datetime/DateTimeController.h"
#include "components/heartrate/HeartRateController.h"
#include "components/stopwatch/StopWatchController.h"
#include "components/fs/FS.h"
#include "drivers/Spi.h"
#include "drivers/SpiMaster.h"
#include "drivers/SpiNorFlash.h"
#include "drivers/St7789.h"
#include "drivers/TwiMaster.h"
#include "drivers/Cst816s.h"
#include "drivers/PinMap.h"
#include "systemtask/SystemTask.h"
#include "touchhandler/TouchHandler.h"
#include "buttonhandler/ButtonHandler.h"

#include "logging/Stm32Logger.h"
Pinetime::Logging::Stm32Logger logger;

static constexpr uint8_t touchPanelTwiAddress = 0x15;
static constexpr uint8_t motionSensorTwiAddress = 0x18;
static constexpr uint8_t heartRateSensorTwiAddress = 0x44;

static SPI_HandleTypeDef hspi1;
static I2C_HandleTypeDef hi2c1;

Pinetime::Drivers::SpiMaster spi {&hspi1};
Pinetime::Drivers::Spi lcdSpi {spi, Pinetime::PinMap::SpiLcdCsnPin};
Pinetime::Drivers::St7789 lcd {lcdSpi, 0, 0};

Pinetime::Drivers::Spi flashSpi {spi, Pinetime::PinMap::SpiFlashCsnPin};
Pinetime::Drivers::SpiNorFlash spiNorFlash {flashSpi};

Pinetime::Drivers::TwiMaster twiMaster {&hi2c1, 0, 0};
Pinetime::Drivers::Cst816S touchPanel {twiMaster, touchPanelTwiAddress};

#ifdef PINETIME_IS_RECOVERY
  #include "displayapp/DisplayAppRecovery.h"
#else
  #include "displayapp/DisplayApp.h"
#endif

Pinetime::Drivers::Bma421 motionSensor {twiMaster, motionSensorTwiAddress};
Pinetime::Drivers::Hrs3300 heartRateSensor {twiMaster, heartRateSensorTwiAddress};

TimerHandle_t debounceTimer;
TimerHandle_t debounceChargeTimer;
Pinetime::Controllers::Battery batteryController;
Pinetime::Controllers::Ble bleController;

Pinetime::Controllers::FS fs {spiNorFlash};
Pinetime::Controllers::Settings settingsController {fs};
Pinetime::Controllers::MotorController motorController {};
Pinetime::Controllers::HeartRateController heartRateController;
Pinetime::Applications::HeartRateTask heartRateApp(heartRateSensor, heartRateController, settingsController);

Pinetime::Controllers::DateTime dateTimeController {settingsController};
Pinetime::Drivers::Watchdog watchdog;
Pinetime::Controllers::NotificationManager notificationManager;
Pinetime::Controllers::MotionController motionController;
Pinetime::Controllers::StopWatchController stopWatchController;
Pinetime::Controllers::AlarmController alarmController {dateTimeController, fs};
Pinetime::Controllers::TouchHandler touchHandler;
Pinetime::Controllers::ButtonHandler buttonHandler;
Pinetime::Controllers::BrightnessController brightnessController {};

Pinetime::Applications::DisplayApp displayApp(lcd,
                                              touchPanel,
                                              batteryController,
                                              bleController,
                                              dateTimeController,
                                              watchdog,
                                              notificationManager,
                                              heartRateController,
                                              settingsController,
                                              motorController,
                                              motionController,
                                              stopWatchController,
                                              alarmController,
                                              brightnessController,
                                              touchHandler,
                                              fs,
                                              spiNorFlash);

Pinetime::System::SystemTask systemTask(spi,
                                        spiNorFlash,
                                        twiMaster,
                                        touchPanel,
                                        batteryController,
                                        bleController,
                                        dateTimeController,
                                        stopWatchController,
                                        alarmController,
                                        watchdog,
                                        notificationManager,
                                        heartRateSensor,
                                        motionController,
                                        motionSensor,
                                        settingsController,
                                        heartRateController,
                                        displayApp,
                                        heartRateApp,
                                        fs,
                                        touchHandler,
                                        buttonHandler);

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

extern uint32_t __start_noinit_data;
extern uint32_t __stop_noinit_data;
static constexpr uint32_t NoInit_MagicValue = 0xDEAD0000;
uint32_t NoInit_MagicWord __attribute__((section(".noinit")));
std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> NoInit_BackUpTime __attribute__((section(".noinit")));

void DebounceTimerChargeCallback(TimerHandle_t xTimer) {
  xTimerStop(xTimer, 0);
  systemTask.PushMessage(Pinetime::System::Messages::OnChargingEvent);
}

void DebounceTimerCallback(TimerHandle_t /*unused*/) {
  systemTask.PushMessage(Pinetime::System::Messages::HandleButtonEvent);
}

void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
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

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (hspi->Instance == SPI1) {
    __HAL_RCC_SPI1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_5;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi) {
  if (hspi->Instance == SPI1) {
    __HAL_RCC_SPI1_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5 | GPIO_PIN_6);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_5);
  }
}

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (hi2c->Instance == I2C1) {
    __HAL_RCC_I2C1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c) {
  if (hi2c->Instance == I2C1) {
    __HAL_RCC_I2C1_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6 | GPIO_PIN_7);
  }
}

int main() {
  HAL_Init();
  SystemClock_Config();
  logger.Init();

  debounceTimer = xTimerCreate("debounceTimer", 10, pdFALSE, nullptr, DebounceTimerCallback);
  debounceChargeTimer = xTimerCreate("debounceTimerCharge", 200, pdFALSE, nullptr, DebounceTimerChargeCallback);

  if (NoInit_MagicWord == NoInit_MagicValue) {
    dateTimeController.SetCurrentTime(NoInit_BackUpTime);
  } else {
    memset(&__start_noinit_data, 0, (uintptr_t) &__stop_noinit_data - (uintptr_t) &__start_noinit_data);
    NoInit_MagicWord = NoInit_MagicValue;
  }

  systemTask.Start();

  vTaskStartScheduler();

  for (;;) {
  }
}