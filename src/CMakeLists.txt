cmake_minimum_required(VERSION 3.10)

project(pinetime-app C CXX ASM)

set(NRF_PROJECT_NAME pinetime-app)

if (NOT STM32WB55_HAL_PATH)
  message(FATAL_ERROR "The path to the STM32WB55 HAL (STM32WB55_HAL_PATH) must be set.")
endif ()
if (DEFINED ARM_NONE_EABI_TOOLCHAIN_PATH)
  set(ARM_NONE_EABI_TOOLCHAIN_BIN_PATH ${ARM_NONE_EABI_TOOLCHAIN_PATH}/bin)
endif ()

include(${CMAKE_SOURCE_DIR}/cmake-nRF5x/arm-gcc-toolchain.cmake)

if (NOT DEFINED ARM_GCC_TOOLCHAIN)
  message(FATAL_ERROR "The toolchain must be set up before calling this macro")
endif ()
set(CMAKE_OSX_SYSROOT "/")
set(CMAKE_OSX_DEPLOYMENT_TARGET "")

set(STM32_HAL_SRC
        "${STM32WB55_HAL_PATH}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal.c"
        "${STM32WB55_HAL_PATH}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_cortex.c"
        "${STM32WB55_HAL_PATH}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_dma.c"
        "${STM32WB55_HAL_PATH}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_dma_ex.c"
        "${STM32WB55_HAL_PATH}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_exti.c"
        "${STM32WB55_HAL_PATH}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_flash.c"
        "${STM32WB55_HAL_PATH}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_flash_ex.c"
        "${STM32WB55_HAL_PATH}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_gpio.c"
        "${STM32WB55_HAL_PATH}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_hsem.c"
        "${STM32WB55_HAL_PATH}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_pwr.c"
        "${STM32WB55_HAL_PATH}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_pwr_ex.c"
        "${STM32WB55_HAL_PATH}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_rcc.c"
        "${STM32WB55_HAL_PATH}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_rcc_ex.c"
        "${STM32WB55_HAL_PATH}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_spi.c"
        "${STM32WB55_HAL_PATH}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_spi_ex.c"
        "${STM32WB55_HAL_PATH}/Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_rcc.c"

        "${STM32WB55_HAL_PATH}/Core/Src/system_stm32wbxx.c"
        "${STM32WB55_HAL_PATH}/Core/Startup/startup_stm32wb55cgux.s"
        "${STM32WB55_HAL_PATH}/Core/Src/stm32wbxx_hal_msp.c"
        )

set(LITTLEFS_SRC
        libs/littlefs/lfs_util.h
        libs/littlefs/lfs.h
        libs/littlefs/lfs_util.c
        libs/littlefs/lfs.c
        )

set(LVGL_SRC
        libs/lv_conf.h
        libs/lvgl/lvgl.h

        libs/lvgl/src/lvgl.h
        libs/lvgl/src/lv_api_map.h
        libs/lvgl/src/lv_conf_internal.h
        libs/lvgl/src/lv_core/lv_disp.h
        libs/lvgl/src/lv_core/lv_group.h
        libs/lvgl/src/lv_core/lv_indev.h
        libs/lvgl/src/lv_core/lv_obj.h
        libs/lvgl/src/lv_core/lv_obj_style_dec.h
        libs/lvgl/src/lv_core/lv_refr.h
        libs/lvgl/src/lv_core/lv_style.h
        libs/lvgl/src/lv_draw/lv_draw.h
        libs/lvgl/src/lv_draw/lv_draw_arc.h
        libs/lvgl/src/lv_draw/lv_draw_blend.h
        libs/lvgl/src/lv_draw/lv_draw_img.h
        libs/lvgl/src/lv_draw/lv_draw_label.h
        libs/lvgl/src/lv_draw/lv_draw_line.h
        libs/lvgl/src/lv_draw/lv_draw_mask.h
        libs/lvgl/src/lv_draw/lv_draw_rect.h
        libs/lvgl/src/lv_draw/lv_draw_triangle.h
        libs/lvgl/src/lv_draw/lv_img_buf.h
        libs/lvgl/src/lv_draw/lv_img_cache.h
        libs/lvgl/src/lv_draw/lv_img_decoder.h
        libs/lvgl/src/lv_font/lv_font.h
        libs/lvgl/src/lv_font/lv_font_fmt_txt.h
        libs/lvgl/src/lv_font/lv_font_loader.h
        libs/lvgl/src/lv_font/lv_symbol_def.h
        libs/lvgl/src/lv_hal/lv_hal.h
        libs/lvgl/src/lv_hal/lv_hal_disp.h
        libs/lvgl/src/lv_hal/lv_hal_indev.h
        libs/lvgl/src/lv_hal/lv_hal_tick.h
        libs/lvgl/src/lv_misc/lv_anim.h
        libs/lvgl/src/lv_misc/lv_area.h
        libs/lvgl/src/lv_misc/lv_async.h
        libs/lvgl/src/lv_misc/lv_bidi.h
        libs/lvgl/src/lv_misc/lv_color.h
        libs/lvgl/src/lv_misc/lv_debug.h
        libs/lvgl/src/lv_misc/lv_fs.h
        libs/lvgl/src/lv_misc/lv_gc.h
        libs/lvgl/src/lv_misc/lv_ll.h
        libs/lvgl/src/lv_misc/lv_log.h
        libs/lvgl/src/lv_misc/lv_math.h
        libs/lvgl/src/lv_misc/lv_mem.h
        libs/lvgl/src/lv_misc/lv_printf.h
        libs/lvgl/src/lv_misc/lv_task.h
        libs/lvgl/src/lv_misc/lv_templ.h
        libs/lvgl/src/lv_misc/lv_txt.h
        libs/lvgl/src/lv_misc/lv_txt_ap.h
        libs/lvgl/src/lv_misc/lv_types.h
        libs/lvgl/src/lv_misc/lv_utils.h
        libs/lvgl/src/lv_themes/lv_theme.h
        libs/lvgl/src/lv_themes/lv_theme_empty.h
        libs/lvgl/src/lv_themes/lv_theme_material.h
        libs/lvgl/src/lv_widgets/lv_arc.h
        libs/lvgl/src/lv_widgets/lv_bar.h
        libs/lvgl/src/lv_widgets/lv_btn.h
        libs/lvgl/src/lv_widgets/lv_btnmatrix.h
        libs/lvgl/src/lv_widgets/lv_calendar.h
        libs/lvgl/src/lv_widgets/lv_canvas.h
        libs/lvgl/src/lv_widgets/lv_chart.h
        libs/lvgl/src/lv_widgets/lv_checkbox.h
        libs/lvgl/src/lv_widgets/lv_cont.h
        libs/lvgl/src/lv_widgets/lv_cpicker.h
        libs/lvgl/src/lv_widgets/lv_dropdown.h
        libs/lvgl/src/lv_widgets/lv_gauge.h
        libs/lvgl/src/lv_widgets/lv_img.h
        libs/lvgl/src/lv_widgets/lv_imgbtn.h
        libs/lvgl/src/lv_widgets/lv_keyboard.h
        libs/lvgl/src/lv_widgets/lv_label.h
        libs/lvgl/src/lv_widgets/lv_led.h
        libs/lvgl/src/lv_widgets/lv_line.h
        libs/lvgl/src/lv_widgets/lv_linemeter.h
        libs/lvgl/src/lv_widgets/lv_list.h
        libs/lvgl/src/lv_widgets/lv_msgbox.h
        libs/lvgl/src/lv_widgets/lv_objmask.h
        libs/lvgl/src/lv_widgets/lv_objx_templ.h
        libs/lvgl/src/lv_widgets/lv_page.h
        libs/lvgl/src/lv_widgets/lv_roller.h
        libs/lvgl/src/lv_widgets/lv_slider.h
        libs/lvgl/src/lv_widgets/lv_spinbox.h
        libs/lvgl/src/lv_widgets/lv_spinner.h
        libs/lvgl/src/lv_widgets/lv_switch.h
        libs/lvgl/src/lv_widgets/lv_table.h
        libs/lvgl/src/lv_widgets/lv_tabview.h
        libs/lvgl/src/lv_widgets/lv_textarea.h
        libs/lvgl/src/lv_widgets/lv_tileview.h
        libs/lvgl/src/lv_widgets/lv_win.h
        libs/lvgl/src/lv_core/lv_disp.c
        libs/lvgl/src/lv_core/lv_group.c
        libs/lvgl/src/lv_core/lv_indev.c
        libs/lvgl/src/lv_core/lv_obj.c
        libs/lvgl/src/lv_core/lv_refr.c
        libs/lvgl/src/lv_core/lv_style.c
        libs/lvgl/src/lv_draw/lv_draw_arc.c
        libs/lvgl/src/lv_draw/lv_draw_blend.c
        libs/lvgl/src/lv_draw/lv_draw_img.c
        libs/lvgl/src/lv_draw/lv_draw_label.c
        libs/lvgl/src/lv_draw/lv_draw_line.c
        libs/lvgl/src/lv_draw/lv_draw_mask.c
        libs/lvgl/src/lv_draw/lv_draw_rect.c
        libs/lvgl/src/lv_draw/lv_draw_triangle.c
        libs/lvgl/src/lv_draw/lv_img_buf.c
        libs/lvgl/src/lv_draw/lv_img_cache.c
        libs/lvgl/src/lv_draw/lv_img_decoder.c
        libs/lvgl/src/lv_font/lv_font.c
        libs/lvgl/src/lv_font/lv_font_fmt_txt.c
        libs/lvgl/src/lv_font/lv_font_loader.c
        libs/lvgl/src/lv_font/lv_font_montserrat_14.c
        libs/lvgl/src/lv_font/lv_font_montserrat_18.c
        libs/lvgl/src/lv_font/lv_font_montserrat_22.c
        libs/lvgl/src/lv_font/lv_font_montserrat_28.c
        libs/lvgl/src/lv_hal/lv_hal_disp.c
        libs/lvgl/src/lv_hal/lv_hal_indev.c
        libs/lvgl/src/lv_hal/lv_hal_tick.c
        libs/lvgl/src/lv_misc/lv_anim.c
        libs/lvgl/src/lv_misc/lv_area.c
        libs/lvgl/src/lv_misc/lv_async.c
        libs/lvgl/src/lv_misc/lv_bidi.c
        libs/lvgl/src/lv_misc/lv_color.c
        libs/lvgl/src/lv_misc/lv_debug.c
        libs/lvgl/src/lv_misc/lv_fs.c
        libs/lvgl/src/lv_misc/lv_gc.c
        libs/lvgl/src/lv_misc/lv_ll.c
        libs/lvgl/src/lv_misc/lv_log.c
        libs/lvgl/src/lv_misc/lv_math.c
        libs/lvgl/src/lv_misc/lv_mem.c
        libs/lvgl/src/lv_misc/lv_printf.c
        libs/lvgl/src/lv_misc/lv_task.c
        libs/lvgl/src/lv_misc/lv_templ.c
        libs/lvgl/src/lv_misc/lv_txt.c
        libs/lvgl/src/lv_misc/lv_txt_ap.c
        libs/lvgl/src/lv_misc/lv_utils.c
        libs/lvgl/src/lv_themes/lv_theme.c
        libs/lvgl/src/lv_themes/lv_theme_empty.c
        libs/lvgl/src/lv_themes/lv_theme_material.c
        libs/lvgl/src/lv_widgets/lv_arc.c
        libs/lvgl/src/lv_widgets/lv_bar.c
        libs/lvgl/src/lv_widgets/lv_btn.c
        libs/lvgl/src/lv_widgets/lv_btnmatrix.c
        libs/lvgl/src/lv_widgets/lv_calendar.c
        libs/lvgl/src/lv_widgets/lv_canvas.c
        libs/lvgl/src/lv_widgets/lv_chart.c
        libs/lvgl/src/lv_widgets/lv_checkbox.c
        libs/lvgl/src/lv_widgets/lv_cont.c
        libs/lvgl/src/lv_widgets/lv_cpicker.c
        libs/lvgl/src/lv_widgets/lv_dropdown.c
        libs/lvgl/src/lv_widgets/lv_gauge.c
        libs/lvgl/src/lv_widgets/lv_img.c
        libs/lvgl/src/lv_widgets/lv_imgbtn.c
        libs/lvgl/src/lv_widgets/lv_keyboard.c
        libs/lvgl/src/lv_widgets/lv_label.c
        libs/lvgl/src/lv_widgets/lv_led.c
        libs/lvgl/src/lv_widgets/lv_line.c
        libs/lvgl/src/lv_widgets/lv_linemeter.c
        libs/lvgl/src/lv_widgets/lv_list.c
        libs/lvgl/src/lv_widgets/lv_msgbox.c
        libs/lvgl/src/lv_widgets/lv_objmask.c
        libs/lvgl/src/lv_widgets/lv_objx_templ.c
        libs/lvgl/src/lv_widgets/lv_page.c
        libs/lvgl/src/lv_widgets/lv_roller.c
        libs/lvgl/src/lv_widgets/lv_slider.c
        libs/lvgl/src/lv_widgets/lv_spinbox.c
        libs/lvgl/src/lv_widgets/lv_spinner.c
        libs/lvgl/src/lv_widgets/lv_switch.c
        libs/lvgl/src/lv_widgets/lv_table.c
        libs/lvgl/src/lv_widgets/lv_tabview.c
        libs/lvgl/src/lv_widgets/lv_textarea.c
        libs/lvgl/src/lv_widgets/lv_tileview.c
        libs/lvgl/src/lv_widgets/lv_win.c
        )

list(APPEND IMAGE_FILES
        displayapp/icons/battery/batteryicon.c
        )
list(APPEND SOURCE_FILES
        stdlib.c
        FreeRTOS/heap_4_infinitime.c
        BootloaderVersion.cpp
        logging/Stm32Logger.cpp
        displayapp/DisplayApp.cpp
        displayapp/screens/Screen.cpp
        displayapp/screens/Tile.cpp
        displayapp/screens/InfiniPaint.cpp
        displayapp/screens/Paddle.cpp
        displayapp/screens/StopWatch.cpp
        displayapp/screens/BatteryIcon.cpp
        displayapp/screens/BleIcon.cpp
        displayapp/screens/NotificationIcon.cpp
        displayapp/screens/SystemInfo.cpp
        displayapp/screens/Label.cpp
        displayapp/screens/FirmwareUpdate.cpp
        displayapp/screens/Music.cpp
        displayapp/screens/Navigation.cpp
        displayapp/screens/Metronome.cpp
        displayapp/screens/Motion.cpp
        displayapp/screens/Weather.cpp
        displayapp/screens/Calculator.cpp
        displayapp/screens/FirmwareValidation.cpp
        displayapp/screens/ApplicationList.cpp
        displayapp/screens/Notifications.cpp
        displayapp/screens/Twos.cpp
        displayapp/screens/HeartRate.cpp
        displayapp/screens/FlashLight.cpp
        displayapp/screens/List.cpp
        displayapp/screens/CheckboxList.cpp
        displayapp/screens/BatteryInfo.cpp
        displayapp/screens/Steps.cpp
        displayapp/screens/Timer.cpp
        displayapp/screens/Dice.cpp
        displayapp/screens/PassKey.cpp
        displayapp/screens/Error.cpp
        displayapp/screens/Alarm.cpp
        displayapp/screens/Styles.cpp
        displayapp/screens/WeatherSymbols.cpp
        displayapp/Colors.cpp
        displayapp/widgets/Counter.cpp
        displayapp/widgets/PageIndicator.cpp
        displayapp/widgets/DotIndicator.cpp
        displayapp/widgets/StatusIcons.cpp

        displayapp/screens/settings/QuickSettings.cpp
        displayapp/screens/settings/Settings.cpp
        displayapp/screens/settings/SettingWatchFace.cpp
        displayapp/screens/settings/SettingTimeFormat.cpp
        displayapp/screens/settings/SettingWeatherFormat.cpp
        displayapp/screens/settings/SettingWakeUp.cpp
        displayapp/screens/settings/SettingDisplay.cpp
        displayapp/screens/settings/SettingHeartRate.cpp
        displayapp/screens/settings/SettingSteps.cpp
        displayapp/screens/settings/SettingSetDateTime.cpp
        displayapp/screens/settings/SettingSetDate.cpp
        displayapp/screens/settings/SettingSetTime.cpp
        displayapp/screens/settings/SettingChimes.cpp
        displayapp/screens/settings/SettingShakeThreshold.cpp
        displayapp/screens/settings/SettingBluetooth.cpp
        displayapp/screens/settings/SettingOTA.cpp

        displayapp/screens/WatchFaceAnalog.cpp
        displayapp/screens/WatchFaceDigital.cpp
        displayapp/screens/WatchFaceInfineat.cpp
        displayapp/screens/WatchFaceTerminal.cpp
        displayapp/screens/WatchFacePineTimeStyle.cpp
        displayapp/screens/WatchFaceCasioStyleG7710.cpp
        displayapp/screens/WatchFacePrideFlag.cpp

        main.cpp
        drivers/St7789.cpp
        drivers/SpiNorFlash.cpp
        drivers/SpiMaster.cpp
        drivers/Spi.cpp
        drivers/Watchdog.cpp
        drivers/InternalFlash.cpp
        drivers/Hrs3300.cpp
        drivers/Bma421.cpp
        drivers/Bma421_C/bma4.c
        drivers/Bma421_C/bma423.c
        components/battery/BatteryController.cpp
        components/ble/BleController.cpp
        components/ble/NotificationManager.cpp
        components/datetime/DateTimeController.cpp
        components/brightness/BrightnessController.cpp
        components/motion/MotionController.cpp
        components/firmwarevalidator/FirmwareValidator.cpp
        components/motor/MotorController.cpp
        components/settings/Settings.cpp
        components/timer/Timer.cpp
        components/stopwatch/StopWatchController.cpp
        components/alarm/AlarmController.cpp
        components/fs/FS.cpp
        drivers/Cst816s.cpp
        FreeRTOS/port.c
        FreeRTOS/port_cmsis_systick.c
        FreeRTOS/port_cmsis.c

        displayapp/LittleVgl.cpp
        displayapp/InfiniTimeTheme.cpp

        systemtask/SystemTask.cpp
        systemtask/SystemMonitor.cpp
        systemtask/WakeLock.cpp
        drivers/TwiMaster.cpp

        heartratetask/HeartRateTask.cpp
        components/heartrate/HeartRateController.cpp
        components/heartrate/Ppg.cpp

        buttonhandler/ButtonHandler.cpp
        touchhandler/TouchHandler.cpp

        utility/Math.cpp
        )

list(APPEND RECOVERY_SOURCE_FILES
        stdlib.c
        FreeRTOS/heap_4_infinitime.c

        BootloaderVersion.cpp
        logging/Stm32Logger.cpp
        displayapp/DisplayAppRecovery.cpp

        main.cpp
        drivers/St7789.cpp
        drivers/SpiNorFlash.cpp
        drivers/SpiMaster.cpp
        drivers/Spi.cpp
        drivers/Watchdog.cpp
        drivers/InternalFlash.cpp
        drivers/Hrs3300.cpp
        drivers/Bma421.cpp
        drivers/Bma421_C/bma4.c
        drivers/Bma421_C/bma423.c
        components/battery/BatteryController.cpp
        components/ble/BleController.cpp
        components/ble/NotificationManager.cpp
        components/datetime/DateTimeController.cpp
        components/brightness/BrightnessController.cpp
        components/motion/MotionController.cpp
        components/firmwarevalidator/FirmwareValidator.cpp
        components/settings/Settings.cpp
        components/timer/Timer.cpp
        components/stopwatch/StopWatchController.cpp
        components/alarm/AlarmController.cpp
        drivers/Cst816s.cpp
        FreeRTOS/port.c
        FreeRTOS/port_cmsis_systick.c
        FreeRTOS/port_cmsis.c

        systemtask/SystemTask.cpp
        systemtask/SystemMonitor.cpp
        systemtask/WakeLock.cpp
        drivers/TwiMaster.cpp
        components/rle/RleDecoder.cpp
        components/heartrate/HeartRateController.cpp
        heartratetask/HeartRateTask.cpp
        components/heartrate/Ppg.cpp

        components/motor/MotorController.cpp
        components/fs/FS.cpp
        buttonhandler/ButtonHandler.cpp
        touchhandler/TouchHandler.cpp

        utility/Math.cpp
        )

list(APPEND RECOVERYLOADER_SOURCE_FILES
        stdlib.c
        FreeRTOS/heap_4_infinitime.c

        FreeRTOS/port.c
        FreeRTOS/port_cmsis_systick.c
        FreeRTOS/port_cmsis.c

        drivers/SpiNorFlash.cpp
        drivers/SpiMaster.cpp
        drivers/Spi.cpp
        logging/Stm32Logger.cpp

        components/rle/RleDecoder.cpp

        drivers/St7789.cpp
        components/brightness/BrightnessController.cpp

        recoveryLoader.cpp
        )


set(INCLUDE_FILES

        ${CMAKE_CURRENT_BINARY_DIR}/src/Version.h
        BootloaderVersion.h
        logging/Logger.h
        logging/Stm32Logger.h
        displayapp/DisplayApp.h
        displayapp/Messages.h
        displayapp/TouchEvents.h
        displayapp/screens/Screen.h
        displayapp/screens/Tile.h
        displayapp/screens/InfiniPaint.h
        displayapp/screens/StopWatch.h
        displayapp/screens/Paddle.h
        displayapp/screens/BatteryIcon.h
        displayapp/screens/BleIcon.h
        displayapp/screens/NotificationIcon.h
        displayapp/screens/SystemInfo.h
        displayapp/screens/ScreenList.h
        displayapp/screens/Label.h
        displayapp/screens/FirmwareUpdate.h
        displayapp/screens/FirmwareValidation.h
        displayapp/screens/ApplicationList.h
        displayapp/screens/CheckboxList.h
        displayapp/Apps.h
        displayapp/screens/Notifications.h
        displayapp/screens/HeartRate.h
        displayapp/screens/Metronome.h
        displayapp/screens/Motion.h
        displayapp/screens/Timer.h
        displayapp/screens/Dice.h
        displayapp/screens/Alarm.h
        displayapp/Colors.h
        displayapp/widgets/Counter.h
        displayapp/widgets/PageIndicator.h
        displayapp/widgets/DotIndicator.h
        displayapp/widgets/StatusIcons.h
        drivers/St7789.h
        drivers/SpiNorFlash.h
        drivers/SpiMaster.h
        drivers/Spi.h
        drivers/Watchdog.h
        drivers/InternalFlash.h
        drivers/Hrs3300.h
        drivers/PinMap.h
        drivers/Bma421.h
        drivers/Bma421_C/bma4.c
        drivers/Bma421_C/bma423.c
        components/battery/BatteryController.h
        components/ble/BleController.h
        components/ble/NotificationManager.h
        components/datetime/DateTimeController.h
        components/brightness/BrightnessController.h
        components/motion/MotionController.h
        components/firmwarevalidator/FirmwareValidator.h
        components/settings/Settings.h
        components/timer/Timer.h
        components/stopwatch/StopWatchController.h
        components/alarm/AlarmController.h
        drivers/Cst816s.h
        FreeRTOS/portmacro.h
        FreeRTOS/portmacro_cmsis.h
        displayapp/LittleVgl.h
        displayapp/InfiniTimeTheme.h
        systemtask/SystemTask.h
        systemtask/SystemMonitor.h
        systemtask/WakeLock.h
        displayapp/screens/Symbols.h
        drivers/TwiMaster.h
        heartratetask/HeartRateTask.h
        components/heartrate/Ppg.h
        components/heartrate/HeartRateController.h
        libs/arduinoFFT/src/arduinoFFT.h
        libs/arduinoFFT/src/defs.h
        libs/arduinoFFT/src/types.h
        components/motor/MotorController.h
        buttonhandler/ButtonHandler.h
        touchhandler/TouchHandler.h
        utility/Math.h
        )

include_directories(
        ${CMAKE_BINARY_DIR}/src
        .
        ../
)
include_directories(SYSTEM
        libs/
        FreeRTOS/

        "${STM32WB55_HAL_PATH}/Core/Inc"
        "${STM32WB55_HAL_PATH}/Drivers/STM32WBxx_HAL_Driver/Inc"
        "${STM32WB55_HAL_PATH}/Drivers/STM32WBxx_HAL_Driver/Inc/Legacy"
        "${STM32WB55_HAL_PATH}/Drivers/CMSIS/Device/ST/STM32WBxx/Include"
        "${STM32WB55_HAL_PATH}/Drivers/CMSIS/Include"
        )

link_directories(
        ../
)


set(COMMON_FLAGS -MP -MD -mthumb -mabi=aapcs -ftree-vrp -ffunction-sections -fdata-sections -fno-strict-aliasing -fno-builtin -fshort-enums -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fstack-usage -fno-exceptions -fno-non-call-exceptions --specs=nano.specs)
set(WARNING_FLAGS -Wall -Wextra -Warray-bounds=2 -Wformat=2 -Wformat-overflow=2 -Wformat-truncation=2 -Wformat-nonliteral -Wno-missing-field-initializers -Wno-unknown-pragmas -Wno-expansion-to-defined -Wreturn-type -Werror=return-type -Werror)
set(DEBUG_FLAGS -Og -g3)
set(RELEASE_FLAGS -Os)
set(CXX_FLAGS -fno-rtti)
set(ASM_FLAGS -x assembler-with-cpp)

add_definitions(-DSTM32WB55xx)
add_definitions(-DUSE_HAL_DRIVER)
add_definitions(-DHSE_VALUE=8000000)
add_definitions(-DFREERTOS)
add_definitions(-D__STACK_SIZE=1024)
add_definitions(-D__HEAP_SIZE=0)
add_definitions(-DLFS_CONFIG=libs/lfs_config.h)

add_link_options(-Wl,-wrap=malloc -Wl,-wrap=free -Wl,-wrap=calloc -Wl,-wrap=realloc -Wl,-wrap=_malloc_r -Wl,-wrap=_sbrk)

add_definitions(-DTARGET_DEVICE_PINETIME)
add_definitions(-DTARGET_DEVICE_NAME="PINETIME")

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  add_definitions(-DDEBUG)
endif()

add_subdirectory(displayapp/fonts)
target_compile_options(infinitime_fonts PUBLIC
        ${COMMON_FLAGS}
        $<$<CONFIG:DEBUG>: ${DEBUG_FLAGS}>
        $<$<CONFIG:RELEASE>: ${RELEASE_FLAGS}>
        $<$<COMPILE_LANGUAGE:CXX>: ${CXX_FLAGS}>
        $<$<COMPILE_LANGUAGE:ASM>: ${ASM_FLAGS}>
        )

add_subdirectory(displayapp/apps)

add_library(stm32-hal STATIC ${STM32_HAL_SRC})
target_include_directories(stm32-hal SYSTEM PUBLIC . ../)
target_include_directories(stm32-hal SYSTEM PUBLIC ${INCLUDES_FROM_LIBS})
target_compile_options(stm32-hal PRIVATE
        ${COMMON_FLAGS}
        $<$<CONFIG:DEBUG>: ${DEBUG_FLAGS}>
        $<$<CONFIG:RELEASE>: ${RELEASE_FLAGS}>
        $<$<COMPILE_LANGUAGE:CXX>: ${CXX_FLAGS}>
        $<$<COMPILE_LANGUAGE:ASM>: ${ASM_FLAGS}>
        )

add_library(lvgl STATIC ${LVGL_SRC})
target_include_directories(lvgl SYSTEM PUBLIC . ../)
target_include_directories(lvgl SYSTEM PUBLIC ${INCLUDES_FROM_LIBS})
target_compile_options(lvgl PRIVATE
        ${COMMON_FLAGS}
        $<$<CONFIG:DEBUG>: ${DEBUG_FLAGS}>
        $<$<CONFIG:RELEASE>: ${RELEASE_FLAGS}>
        $<$<COMPILE_LANGUAGE:CXX>: ${CXX_FLAGS}>
        $<$<COMPILE_LANGUAGE:ASM>: ${ASM_FLAGS}>
        )

add_library(littlefs STATIC ${LITTLEFS_SRC})
target_include_directories(littlefs SYSTEM PUBLIC . ../)
target_include_directories(littlefs SYSTEM PUBLIC ${INCLUDES_FROM_LIBS})
target_compile_options(littlefs PRIVATE
        ${COMMON_FLAGS}
        $<$<CONFIG:DEBUG>: ${DEBUG_FLAGS}>
        $<$<CONFIG:RELEASE>: ${RELEASE_FLAGS}>
        $<$<COMPILE_LANGUAGE:CXX>: ${CXX_FLAGS}>
        $<$<COMPILE_LANGUAGE:ASM>: ${ASM_FLAGS}>
        )

set(EXECUTABLE_NAME "pinetime-app")
set(EXECUTABLE_FILE_NAME ${EXECUTABLE_NAME}-${pinetime_VERSION_MAJOR}.${pinetime_VERSION_MINOR}.${pinetime_VERSION_PATCH})
set(STM32_LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/gcc_stm32wb55.ld")
add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})
set_target_properties(${EXECUTABLE_NAME} PROPERTIES OUTPUT_NAME ${EXECUTABLE_FILE_NAME})
target_link_libraries(${EXECUTABLE_NAME} stm32-hal lvgl littlefs infinitime_fonts infinitime_apps)
target_compile_options(${EXECUTABLE_NAME} PUBLIC
        ${COMMON_FLAGS}
        ${WARNING_FLAGS}
        $<$<CONFIG:DEBUG>: ${DEBUG_FLAGS}>
        $<$<CONFIG:RELEASE>: ${RELEASE_FLAGS}>
        $<$<COMPILE_LANGUAGE:CXX>: ${CXX_FLAGS}>
        $<$<COMPILE_LANGUAGE:ASM>: ${ASM_FLAGS}>
        )

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        SUFFIX ".out"
        LINK_FLAGS "-mthumb -mabi=aapcs -T${STM32_LINKER_SCRIPT} -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Wl,--gc-sections -Wl,--print-memory-usage --specs=nano.specs -Wl,-Map=${EXECUTABLE_FILE_NAME}.map"
        )

add_custom_command(TARGET ${EXECUTABLE_NAME}
        POST_BUILD
        COMMAND ${CMAKE_SIZE_UTIL} ${EXECUTABLE_FILE_NAME}.out
        COMMAND ${CMAKE_OBJCOPY} -O binary ${EXECUTABLE_FILE_NAME}.out "${EXECUTABLE_FILE_NAME}.bin"
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${EXECUTABLE_FILE_NAME}.out "${EXECUTABLE_FILE_NAME}.hex"
        COMMENT "post build steps for ${EXECUTABLE_FILE_NAME}")

if(BUILD_RESOURCES)
  add_dependencies(${EXECUTABLE_NAME} GenerateResources)
endif()

set(EXECUTABLE_MCUBOOT_NAME "pinetime-mcuboot-app")
set(EXECUTABLE_MCUBOOT_FILE_NAME ${EXECUTABLE_MCUBOOT_NAME}-${pinetime_VERSION_MAJOR}.${pinetime_VERSION_MINOR}.${pinetime_VERSION_PATCH})
set(IMAGE_MCUBOOT_FILE_NAME_HEX ${EXECUTABLE_MCUBOOT_NAME}-image-${pinetime_VERSION_MAJOR}.${pinetime_VERSION_MINOR}.${pinetime_VERSION_PATCH}.hex)
set(IMAGE_MCUBOOT_FILE_NAME_BIN ${EXECUTABLE_MCUBOOT_NAME}-image-${pinetime_VERSION_MAJOR}.${pinetime_VERSION_MINOR}.${pinetime_VERSION_PATCH}.bin)
set(STM32_LINKER_SCRIPT_MCUBOOT "${CMAKE_SOURCE_DIR}/gcc_stm32wb55-mcuboot.ld")
add_executable(${EXECUTABLE_MCUBOOT_NAME} ${SOURCE_FILES})
target_link_libraries(${EXECUTABLE_MCUBOOT_NAME} stm32-hal lvgl littlefs infinitime_fonts infinitime_apps)
set_target_properties(${EXECUTABLE_MCUBOOT_NAME} PROPERTIES OUTPUT_NAME ${EXECUTABLE_MCUBOOT_FILE_NAME})
target_compile_options(${EXECUTABLE_MCUBOOT_NAME} PUBLIC
        ${COMMON_FLAGS}
        ${WARNING_FLAGS}
        $<$<CONFIG:DEBUG>: ${DEBUG_FLAGS}>
        $<$<CONFIG:RELEASE>: ${RELEASE_FLAGS}>
        $<$<COMPILE_LANGUAGE:CXX>: ${CXX_FLAGS}>
        $<$<COMPILE_LANGUAGE:ASM>: ${ASM_FLAGS}>
        )

set_target_properties(${EXECUTABLE_MCUBOOT_NAME} PROPERTIES
        SUFFIX ".out"
        LINK_FLAGS "-mthumb -mabi=aapcs -T${STM32_LINKER_SCRIPT_MCUBOOT} -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Wl,--gc-sections -Wl,--print-memory-usage --specs=nano.specs -Wl,-Map=${EXECUTABLE_MCUBOOT_FILE_NAME}.map"
        )

add_custom_command(TARGET ${EXECUTABLE_MCUBOOT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_SIZE_UTIL} ${EXECUTABLE_MCUBOOT_FILE_NAME}.out
        COMMAND ${CMAKE_OBJCOPY} -O binary ${EXECUTABLE_MCUBOOT_FILE_NAME}.out "${EXECUTABLE_MCUBOOT_FILE_NAME}.bin"
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${EXECUTABLE_MCUBOOT_FILE_NAME}.out "${EXECUTABLE_MCUBOOT_FILE_NAME}.hex"
        COMMENT "post build steps for ${EXECUTABLE_MCUBOOT_FILE_NAME}"
        )

if(BUILD_RESOURCES)
  add_dependencies(${EXECUTABLE_MCUBOOT_NAME} GenerateResources)
endif()

set(EXECUTABLE_RECOVERY_NAME "pinetime-recovery")
set(EXECUTABLE_RECOVERY_FILE_NAME ${EXECUTABLE_RECOVERY_NAME}-${pinetime_VERSION_MAJOR}.${pinetime_VERSION_MINOR}.${pinetime_VERSION_PATCH})
add_executable(${EXECUTABLE_RECOVERY_NAME} ${RECOVERY_SOURCE_FILES})
target_link_libraries(${EXECUTABLE_RECOVERY_NAME} stm32-hal littlefs infinitime_fonts infinitime_apps)
set_target_properties(${EXECUTABLE_RECOVERY_NAME} PROPERTIES OUTPUT_NAME ${EXECUTABLE_RECOVERY_FILE_NAME})
target_compile_definitions(${EXECUTABLE_RECOVERY_NAME} PUBLIC "PINETIME_IS_RECOVERY")
target_compile_options(${EXECUTABLE_RECOVERY_NAME} PUBLIC
        ${COMMON_FLAGS}
        ${WARNING_FLAGS}
        $<$<CONFIG:DEBUG>: ${DEBUG_FLAGS}>
        $<$<CONFIG:RELEASE>: ${RELEASE_FLAGS}>
        $<$<COMPILE_LANGUAGE:CXX>: ${CXX_FLAGS}>
        $<$<COMPILE_LANGUAGE:ASM>: ${ASM_FLAGS}>
        )

set_target_properties(${EXECUTABLE_RECOVERY_NAME} PROPERTIES
        SUFFIX ".out"
        LINK_FLAGS "-mthumb -mabi=aapcs -T${STM32_LINKER_SCRIPT} -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Wl,--gc-sections -Wl,--print-memory-usage --specs=nano.specs -Wl,-Map=${EXECUTABLE_RECOVERY_FILE_NAME}.map"
        )

add_custom_command(TARGET ${EXECUTABLE_RECOVERY_NAME}
        POST_BUILD
        COMMAND ${CMAKE_SIZE_UTIL} ${EXECUTABLE_RECOVERY_FILE_NAME}.out
        COMMAND ${CMAKE_OBJCOPY} -O binary ${EXECUTABLE_RECOVERY_FILE_NAME}.out "${EXECUTABLE_RECOVERY_FILE_NAME}.bin"
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${EXECUTABLE_RECOVERY_FILE_NAME}.out "${EXECUTABLE_RECOVERY_FILE_NAME}.hex"
        COMMENT "post build steps for ${EXECUTABLE_RECOVERY_FILE_NAME}"
        )

set(EXECUTABLE_RECOVERY_MCUBOOT_NAME "pinetime-mcuboot-recovery")
set(EXECUTABLE_RECOVERY_MCUBOOT_FILE_NAME ${EXECUTABLE_RECOVERY_MCUBOOT_NAME}-${pinetime_VERSION_MAJOR}.${pinetime_VERSION_MINOR}.${pinetime_VERSION_PATCH})
set(IMAGE_RECOVERY_MCUBOOT_FILE_NAME ${EXECUTABLE_RECOVERY_MCUBOOT_NAME}-image-${pinetime_VERSION_MAJOR}.${pinetime_VERSION_MINOR}.${pinetime_VERSION_PATCH})
set(IMAGE_RECOVERY_MCUBOOT_FILE_NAME_HEX ${IMAGE_RECOVERY_MCUBOOT_FILE_NAME}.hex)
add_executable(${EXECUTABLE_RECOVERY_MCUBOOT_NAME} ${RECOVERY_SOURCE_FILES})
target_link_libraries(${EXECUTABLE_RECOVERY_MCUBOOT_NAME} stm32-hal littlefs infinitime_fonts infinitime_apps)
set_target_properties(${EXECUTABLE_RECOVERY_MCUBOOT_NAME} PROPERTIES OUTPUT_NAME ${EXECUTABLE_RECOVERY_MCUBOOT_FILE_NAME})
target_compile_definitions(${EXECUTABLE_RECOVERY_MCUBOOT_NAME} PUBLIC "PINETIME_IS_RECOVERY")
target_compile_options(${EXECUTABLE_RECOVERY_MCUBOOT_NAME} PUBLIC
        ${COMMON_FLAGS}
        ${WARNING_FLAGS}
        $<$<CONFIG:DEBUG>: ${DEBUG_FLAGS}>
        $<$<CONFIG:RELEASE>: ${RELEASE_FLAGS}>
        $<$<COMPILE_LANGUAGE:CXX>: ${CXX_FLAGS}>
        $<$<COMPILE_LANGUAGE:ASM>: ${ASM_FLAGS}>
        )

set_target_properties(${EXECUTABLE_RECOVERY_MCUBOOT_NAME} PROPERTIES
        SUFFIX ".out"
        LINK_FLAGS "-mthumb -mabi=aapcs -T${STM32_LINKER_SCRIPT_MCUBOOT} -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Wl,--gc-sections -Wl,--print-memory-usage --specs=nano.specs -Wl,-Map=${EXECUTABLE_RECOVERY_MCUBOOT_FILE_NAME}.map"
        )

add_custom_command(TARGET ${EXECUTABLE_RECOVERY_MCUBOOT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_SIZE_UTIL} ${EXECUTABLE_RECOVERY_MCUBOOT_FILE_NAME}.out
        COMMAND ${CMAKE_OBJCOPY} -O binary ${EXECUTABLE_RECOVERY_MCUBOOT_FILE_NAME}.out "${EXECUTABLE_RECOVERY_MCUBOOT_FILE_NAME}.bin"
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${EXECUTABLE_RECOVERY_MCUBOOT_FILE_NAME}.out "${EXECUTABLE_RECOVERY_MCUBOOT_FILE_NAME}.hex"
        COMMAND ${CMAKE_OBJCOPY} -I ihex -O binary ${IMAGE_RECOVERY_MCUBOOT_FILE_NAME_HEX} "${IMAGE_RECOVERY_MCUBOOT_FILE_NAME}.bin"
        COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/bin2c.py ${IMAGE_RECOVERY_MCUBOOT_FILE_NAME}.bin recoveryImage > recoveryImage.h
        COMMENT "post build steps for ${EXECUTABLE_RECOVERY_MCUBOOT_FILE_NAME}"
        )

set(EXECUTABLE_RECOVERYLOADER_NAME "pinetime-recovery-loader")
set(EXECUTABLE_RECOVERYLOADER_FILE_NAME ${EXECUTABLE_RECOVERYLOADER_NAME}-${pinetime_VERSION_MAJOR}.${pinetime_VERSION_MINOR}.${pinetime_VERSION_PATCH})
add_executable(${EXECUTABLE_RECOVERYLOADER_NAME} ${RECOVERYLOADER_SOURCE_FILES})
target_link_libraries(${EXECUTABLE_RECOVERYLOADER_NAME} stm32-hal infinitime_fonts infinitime_apps)
set_target_properties(${EXECUTABLE_RECOVERYLOADER_NAME} PROPERTIES OUTPUT_NAME ${EXECUTABLE_RECOVERYLOADER_FILE_NAME})
target_compile_options(${EXECUTABLE_RECOVERYLOADER_NAME} PUBLIC
        ${COMMON_FLAGS}
        ${WARNING_FLAGS}
        $<$<CONFIG:DEBUG>: ${DEBUG_FLAGS}>
        $<$<CONFIG:RELEASE>: ${RELEASE_FLAGS}>
        $<$<COMPILE_LANGUAGE:CXX>: ${CXX_FLAGS}>
        $<$<COMPILE_LANGUAGE:ASM>: ${ASM_FLAGS}>
        )
target_include_directories(${EXECUTABLE_RECOVERYLOADER_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src>
        )
add_dependencies(${EXECUTABLE_RECOVERYLOADER_NAME} ${EXECUTABLE_RECOVERY_MCUBOOT_NAME})

set_target_properties(${EXECUTABLE_RECOVERYLOADER_NAME} PROPERTIES
        SUFFIX ".out"
        LINK_FLAGS "-mthumb -mabi=aapcs -T${STM32_LINKER_SCRIPT} -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Wl,--gc-sections -Wl,--print-memory-usage --specs=nano.specs -Wl,-Map=${EXECUTABLE_RECOVERYLOADER_FILE_NAME}.map"
        )

add_custom_command(TARGET ${EXECUTABLE_RECOVERYLOADER_NAME}
        POST_BUILD
        COMMAND ${CMAKE_SIZE_UTIL} ${EXECUTABLE_RECOVERYLOADER_FILE_NAME}.out
        COMMAND ${CMAKE_OBJCOPY} -O binary ${EXECUTABLE_RECOVERYLOADER_FILE_NAME}.out "${EXECUTABLE_RECOVERYLOADER_FILE_NAME}.bin"
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${EXECUTABLE_RECOVERYLOADER_FILE_NAME}.out "${EXECUTABLE_RECOVERYLOADER_FILE_NAME}.hex"
        COMMENT "post build steps for ${EXECUTABLE_RECOVERYLOADER_FILE_NAME}"
        )