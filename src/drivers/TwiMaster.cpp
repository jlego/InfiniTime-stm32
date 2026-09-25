#include "drivers/TwiMaster.h"
#include "drivers/PinMap.h"
#include <cstring>

using namespace Pinetime::Drivers;

TwiMaster::TwiMaster(I2C_HandleTypeDef* hi2c, uint8_t pinSda, uint8_t pinScl)
  : hi2c {hi2c}, pinSda {pinSda}, pinScl {pinScl} {
}

void TwiMaster::Init() {
  if (mutex == nullptr) {
    mutex = xSemaphoreCreateBinary();
  }

  hi2c->Instance = I2C1;
  hi2c->Init.Timing = 0x00000E14;
  hi2c->Init.OwnAddress1 = 0;
  hi2c->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c->Init.OwnAddress2 = 0;
  hi2c->Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

  if (HAL_I2C_Init(hi2c) != HAL_OK) {
    return;
  }

  HAL_I2CEx_AnalogFilter_Config(hi2c, I2C_ANALOGFILTER_ENABLE);

  initialized = true;
  xSemaphoreGive(mutex);
}

TwiMaster::ErrorCodes TwiMaster::Read(uint8_t deviceAddress, uint8_t registerAddress, uint8_t* data, size_t size) {
  xSemaphoreTake(mutex, portMAX_DELAY);

  HAL_StatusTypeDef status = HAL_I2C_Mem_Read(hi2c, deviceAddress << 1, registerAddress, I2C_MEMADD_SIZE_8BIT, data, size, HAL_MAX_DELAY);

  xSemaphoreGive(mutex);
  return (status == HAL_OK) ? ErrorCodes::NoError : ErrorCodes::TransactionFailed;
}

TwiMaster::ErrorCodes TwiMaster::Write(uint8_t deviceAddress, uint8_t registerAddress, const uint8_t* data, size_t size) {
  xSemaphoreTake(mutex, portMAX_DELAY);

  HAL_StatusTypeDef status = HAL_I2C_Mem_Write(hi2c, deviceAddress << 1, registerAddress, I2C_MEMADD_SIZE_8BIT, const_cast<uint8_t*>(data), size, HAL_MAX_DELAY);

  xSemaphoreGive(mutex);
  return (status == HAL_OK) ? ErrorCodes::NoError : ErrorCodes::TransactionFailed;
}

void TwiMaster::Sleep() {
  if (initialized) {
    HAL_I2C_DeInit(hi2c);
    __HAL_RCC_I2C1_CLK_DISABLE();
  }
}

void TwiMaster::Wakeup() {
  if (initialized) {
    __HAL_RCC_I2C1_CLK_ENABLE();
    HAL_I2C_Init(hi2c);
  }
}