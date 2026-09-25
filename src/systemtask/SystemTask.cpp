#include "systemtask/SystemTask.h"
#include "stm32wbxx_hal.h"
#include "BootloaderVersion.h"
#include "components/battery/BatteryController.h"
#include "components/ble/BleController.h"
#include "displayapp/TouchEvents.h"
#include "drivers/Cst816s.h"
#include "drivers/St7789.h"
#include "drivers/InternalFlash.h"
#include "drivers/SpiMaster.h"
#include "drivers/SpiNorFlash.h"
#include "drivers/TwiMaster.h"
#include "drivers/Hrs3300.h"
#include "drivers/PinMap.h"
#include "BootErrors.h"

#include <memory>

using namespace Pinetime::System;

namespace {
  inline bool in_isr() {
    return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0;
  }
}

void MeasureBatteryTimerCallback(TimerHandle_t xTimer) {
  auto* sysTask = static_cast<SystemTask*>(pvTimerGetTimerID(xTimer));
  sysTask->PushMessage(Pinetime::System::Messages::MeasureBatteryTimerExpired);
}

SystemTask::SystemTask(Drivers::SpiMaster& spi,
                       Pinetime::Drivers::SpiNorFlash& spiNorFlash,
                       Drivers::TwiMaster& twiMaster,
                       Drivers::Cst816S& touchPanel,
                       Controllers::Battery& batteryController,
                       Controllers::Ble& bleController,
                       Controllers::DateTime& dateTimeController,
                       Controllers::StopWatchController& stopWatchController,
                       Controllers::AlarmController& alarmController,
                       Drivers::Watchdog& watchdog,
                       Pinetime::Controllers::NotificationManager& notificationManager,
                       Pinetime::Drivers::Hrs3300& heartRateSensor,
                       Pinetime::Controllers::MotionController& motionController,
                       Pinetime::Drivers::Bma421& motionSensor,
                       Controllers::Settings& settingsController,
                       Pinetime::Controllers::HeartRateController& heartRateController,
                       Pinetime::Applications::DisplayApp& displayApp,
                       Pinetime::Applications::HeartRateTask& heartRateApp,
                       Pinetime::Controllers::FS& fs,
                       Pinetime::Controllers::TouchHandler& touchHandler,
                       Pinetime::Controllers::ButtonHandler& buttonHandler)
  : spi {spi},
    spiNorFlash {spiNorFlash},
    twiMaster {twiMaster},
    touchPanel {touchPanel},
    batteryController {batteryController},
    bleController {bleController},
    dateTimeController {dateTimeController},
    stopWatchController {stopWatchController},
    alarmController {alarmController},
    watchdog {watchdog},
    notificationManager {notificationManager},
    heartRateSensor {heartRateSensor},
    motionSensor {motionSensor},
    settingsController {settingsController},
    heartRateController {heartRateController},
    motionController {motionController},
    displayApp {displayApp},
    heartRateApp(heartRateApp),
    fs {fs},
    touchHandler {touchHandler},
    buttonHandler {buttonHandler} {
}

void SystemTask::Start() {
  systemTasksMsgQueue = xQueueCreate(10, 1);
  if (pdPASS != xTaskCreate(SystemTask::Process, "MAIN", 350, this, 1, &taskHandle)) {
  }
}

void SystemTask::Process(void* instance) {
  auto* app = static_cast<SystemTask*>(instance);
  app->Work();
}

void SystemTask::Work() {
  BootErrors bootError = BootErrors::None;

  watchdog.Setup(7, Drivers::Watchdog::SleepBehaviour::Run, Drivers::Watchdog::HaltBehaviour::Pause);
  watchdog.Start();

  spi.Init();
  spiNorFlash.Init();
  spiNorFlash.Wakeup();

  fs.Init();

  twiMaster.Init();
  touchPanel.Init();
  dateTimeController.Register(this);
  batteryController.Register(this);
  motionSensor.SoftReset();
  alarmController.Init(this);

  twiMaster.Sleep();
  twiMaster.Init();

  motionSensor.Init();
  motionController.Init(motionSensor.DeviceType());
  settingsController.Init();

  displayApp.Register(this);
  displayApp.Start(bootError);

  heartRateSensor.Init();
  heartRateSensor.Disable();
  heartRateApp.Start();

  buttonHandler.Init(this);

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = PinMap::ButtonEnable.pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(PinMap::ButtonEnable.port, &GPIO_InitStruct);
  HAL_GPIO_WritePin(PinMap::ButtonEnable.port, PinMap::ButtonEnable.pin, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = PinMap::ButtonPin.pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(PinMap::ButtonPin.port, &GPIO_InitStruct);
  HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  GPIO_InitStruct.Pin = PinMap::TouchIntPin.pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(PinMap::TouchIntPin.port, &GPIO_InitStruct);
  HAL_NVIC_SetPriority(EXTI2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  GPIO_InitStruct.Pin = PinMap::ChargingPin.pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(PinMap::ChargingPin.port, &GPIO_InitStruct);
  HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  batteryController.MeasureVoltage();

  measureBatteryTimer = xTimerCreate("measureBattery", batteryMeasurementPeriod, pdTRUE, this, MeasureBatteryTimerCallback);
  xTimerStart(measureBatteryTimer, portMAX_DELAY);

  constexpr TickType_t stateUpdatePeriod = pdMS_TO_TICKS(100);
  TickType_t lastStateUpdate = xTaskGetTickCount() - stateUpdatePeriod;
  TickType_t elapsed;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
  while (true) {
    Messages msg;

    elapsed = xTaskGetTickCount() - lastStateUpdate;
    TickType_t waitTime;
    if (elapsed >= stateUpdatePeriod) {
      waitTime = 0;
    } else {
      waitTime = stateUpdatePeriod - elapsed;
    }
    if (xQueueReceive(systemTasksMsgQueue, &msg, waitTime) == pdTRUE) {
      switch (msg) {
        case Messages::EnableSleeping:
          wakeLocksHeld--;
          break;
        case Messages::DisableSleeping:
          GoToRunning();
          wakeLocksHeld++;
          break;
        case Messages::GoToRunning:
          GoToRunning();
          break;
        case Messages::GoToSleep:
          GoToSleep();
          break;
        case Messages::OnNewTime:
          if (alarmController.IsEnabled()) {
            alarmController.ScheduleAlarm();
          }
          break;
        case Messages::OnNewNotification:
          if (settingsController.GetNotificationStatus() == Pinetime::Controllers::Settings::Notification::On) {
            if (IsSleeping()) {
              GoToRunning();
            }
            displayApp.PushMessage(Pinetime::Applications::Display::Messages::NewNotification);
          }
          break;
        case Messages::SetOffAlarm:
          GoToRunning();
          displayApp.PushMessage(Pinetime::Applications::Display::Messages::AlarmTriggered);
          break;
        case Messages::BleConnected:
          displayApp.PushMessage(Pinetime::Applications::Display::Messages::NotifyDeviceActivity);
          isBleDiscoveryTimerRunning = true;
          bleDiscoveryTimer = 5;
          break;
        case Messages::BleFirmwareUpdateStarted:
          GoToRunning();
          wakeLocksHeld++;
          displayApp.PushMessage(Pinetime::Applications::Display::Messages::BleFirmwareUpdateStarted);
          break;
        case Messages::BleFirmwareUpdateFinished:
          if (bleController.State() == Pinetime::Controllers::Ble::FirmwareUpdateStates::Validated) {
            NVIC_SystemReset();
          }
          wakeLocksHeld--;
          break;
        case Messages::StartFileTransfer:
          GoToRunning();
          wakeLocksHeld++;
          break;
        case Messages::StopFileTransfer:
          wakeLocksHeld--;
          break;
        case Messages::OnTouchEvent:
          if (!touchHandler.ProcessTouchInfo(touchPanel.GetTouchInfo())) {
            break;
          }
          if (state == SystemTaskState::Running) {
            displayApp.PushMessage(Pinetime::Applications::Display::Messages::TouchEvent);
          } else {
            auto gesture = touchHandler.GestureGet();
            if (settingsController.GetNotificationStatus() != Controllers::Settings::Notification::Sleep &&
                gesture != Pinetime::Applications::TouchEvents::None &&
                ((gesture == Pinetime::Applications::TouchEvents::DoubleTap &&
                  settingsController.isWakeUpModeOn(Pinetime::Controllers::Settings::WakeUpMode::DoubleTap)) ||
                 (gesture == Pinetime::Applications::TouchEvents::Tap &&
                  settingsController.isWakeUpModeOn(Pinetime::Controllers::Settings::WakeUpMode::SingleTap)))) {
              GoToRunning();
            }
          }
          break;
        case Messages::HandleButtonEvent: {
          Controllers::ButtonActions action = Controllers::ButtonActions::None;
          if (HAL_GPIO_ReadPin(Pinetime::PinMap::ButtonPin.port, Pinetime::PinMap::ButtonPin.pin) == GPIO_PIN_RESET) {
            action = buttonHandler.HandleEvent(Controllers::ButtonHandler::Events::Release);
          } else {
            action = buttonHandler.HandleEvent(Controllers::ButtonHandler::Events::Press);
            if (IsSleeping()) {
              fastWakeUpDone = true;
              GoToRunning();
              break;
            }
          }
          HandleButtonAction(action);
        } break;
        case Messages::HandleButtonTimerEvent: {
          auto action = buttonHandler.HandleEvent(Controllers::ButtonHandler::Events::Timer);
          HandleButtonAction(action);
        } break;
        case Messages::OnDisplayTaskSleeping:
        case Messages::OnDisplayTaskAOD:
          if (state != SystemTaskState::GoingToSleep) {
            break;
          }

          if (msg == Messages::OnDisplayTaskSleeping) {
            if (BootloaderVersion::IsValid()) {
              spiNorFlash.Sleep();
            }
            spi.Sleep();
          }

          if (!settingsController.isWakeUpModeOn(Pinetime::Controllers::Settings::WakeUpMode::DoubleTap)) {
            touchPanel.Sleep();
          }

          if (msg == Messages::OnDisplayTaskSleeping) {
            state = SystemTaskState::Sleeping;
          } else {
            state = SystemTaskState::AODSleeping;
          }
          break;
        case Messages::OnNewDay:
          motionSensor.ResetStepCounter();
          motionController.AdvanceDay();
          break;
        case Messages::OnNewHour:
          using Pinetime::Controllers::AlarmController;
          if (settingsController.GetNotificationStatus() != Controllers::Settings::Notification::Sleep &&
              settingsController.GetChimeOption() == Controllers::Settings::ChimeOption::Hours && !alarmController.IsAlerting()) {
            GoToRunning();
            displayApp.PushMessage(Pinetime::Applications::Display::Messages::Chime);
          }
          break;
        case Messages::OnNewHalfHour:
          using Pinetime::Controllers::AlarmController;
          if (settingsController.GetNotificationStatus() != Controllers::Settings::Notification::Sleep &&
              settingsController.GetChimeOption() == Controllers::Settings::ChimeOption::HalfHours && !alarmController.IsAlerting()) {
            GoToRunning();
            displayApp.PushMessage(Pinetime::Applications::Display::Messages::Chime);
          }
          break;
        case Messages::OnChargingEvent:
          batteryController.ReadPowerState();
          GoToRunning();
          break;
        case Messages::MeasureBatteryTimerExpired:
          batteryController.MeasureVoltage();
          break;
        case Messages::BatteryPercentageUpdated:
          break;
        case Messages::OnPairing:
          GoToRunning();
          displayApp.PushMessage(Pinetime::Applications::Display::Messages::ShowPairingKey);
          break;
        case Messages::BleRadioEnableToggle:
          break;
        default:
          break;
      }
    }
    elapsed = xTaskGetTickCount() - lastStateUpdate;
    if (elapsed >= stateUpdatePeriod) {
      UpdateMotion();
      if (isBleDiscoveryTimerRunning) {
        if (bleDiscoveryTimer == 0) {
          isBleDiscoveryTimerRunning = false;

        } else {
          bleDiscoveryTimer--;
        }
      }
      monitor.Process();
      NoInit_BackUpTime = dateTimeController.CurrentDateTime();
      if (HAL_GPIO_ReadPin(PinMap::ButtonPin.port, PinMap::ButtonPin.pin) == GPIO_PIN_RESET) {
        watchdog.Reload();
      }
      lastStateUpdate = xTaskGetTickCount();
    }
  }
#pragma clang diagnostic pop
}

void SystemTask::GoToRunning() {
  if (state == SystemTaskState::Running) {
    return;
  }
  if (state == SystemTaskState::Sleeping || state == SystemTaskState::AODSleeping) {
    if (state == SystemTaskState::Sleeping) {
      spi.Wakeup();
      spiNorFlash.Wakeup();
    }

    if (!settingsController.isWakeUpModeOn(Pinetime::Controllers::Settings::WakeUpMode::DoubleTap)) {
      touchPanel.Wakeup();
    }
  }

  displayApp.PushMessage(Pinetime::Applications::Display::Messages::GoToRunning);
  heartRateApp.PushMessage(Pinetime::Applications::HeartRateTask::Messages::WakeUp);



  state = SystemTaskState::Running;
};

void SystemTask::GoToSleep() {
  if (IsSleeping()) {
    return;
  }
  if (IsSleepDisabled()) {
    return;
  }
  if (settingsController.GetAlwaysOnDisplay()) {
    displayApp.PushMessage(Pinetime::Applications::Display::Messages::GoToAOD);
  } else {
    displayApp.PushMessage(Pinetime::Applications::Display::Messages::GoToSleep);
  }
  heartRateApp.PushMessage(Pinetime::Applications::HeartRateTask::Messages::GoToSleep);

  state = SystemTaskState::GoingToSleep;
};

void SystemTask::UpdateMotion() {
  auto motionValues = motionSensor.Process();
  motionController.Update(motionValues.x, motionValues.y, motionValues.z, motionValues.steps);

  if (settingsController.GetNotificationStatus() != Controllers::Settings::Notification::Sleep) {
    if ((settingsController.isWakeUpModeOn(Pinetime::Controllers::Settings::WakeUpMode::RaiseWrist) &&
         motionController.ShouldRaiseWake()) ||
        (settingsController.isWakeUpModeOn(Pinetime::Controllers::Settings::WakeUpMode::Shake) &&
         motionController.CurrentShakeSpeed() > settingsController.GetShakeThreshold())) {
      GoToRunning();
    } else if (settingsController.isWakeUpModeOn(Pinetime::Controllers::Settings::WakeUpMode::LowerWrist) &&
               state == SystemTaskState::Running && motionController.ShouldLowerSleep()) {
      GoToSleep();
    }
  }
}

void SystemTask::HandleButtonAction(Controllers::ButtonActions action) {
  if (IsSleeping()) {
    return;
  }

  displayApp.PushMessage(Pinetime::Applications::Display::Messages::NotifyDeviceActivity);

  using Actions = Controllers::ButtonActions;

  switch (action) {
    case Actions::Click:
      if (!fastWakeUpDone) {
        displayApp.PushMessage(Applications::Display::Messages::ButtonPushed);
      }
      break;
    case Actions::DoubleClick:
      displayApp.PushMessage(Applications::Display::Messages::ButtonDoubleClicked);
      break;
    case Actions::LongPress:
      displayApp.PushMessage(Applications::Display::Messages::ButtonLongPressed);
      break;
    case Actions::LongerPress:
      displayApp.PushMessage(Applications::Display::Messages::ButtonLongerPressed);
      break;
    default:
      return;
  }

  fastWakeUpDone = false;
}

void SystemTask::PushMessage(System::Messages msg) {
  if (in_isr()) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(systemTasksMsgQueue, &msg, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  } else {
    xQueueSend(systemTasksMsgQueue, &msg, portMAX_DELAY);
  }
}