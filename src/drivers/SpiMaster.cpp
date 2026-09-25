#include "drivers/SpiMaster.h"
#include "drivers/PinMap.h"
#include <algorithm>

using namespace Pinetime::Drivers;

SpiMaster::SpiMaster(SPI_HandleTypeDef* hspi) : hspi {hspi} {
}

void SpiMaster::ConfigureSpi() {
  hspi->Instance = SPI1;
  hspi->Init.Mode = SPI_MODE_MASTER;
  hspi->Init.Direction = SPI_DIRECTION_2LINES;
  hspi->Init.DataSize = SPI_DATASIZE_8BIT;
  hspi->Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi->Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi->Init.NSS = SPI_NSS_SOFT;
  hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi->Init.TIMode = SPI_TIMODE_DISABLE;
  hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi->Init.CRCPolynomial = 7;
  hspi->Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi->Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
}

bool SpiMaster::Init() {
  if (mutex == nullptr) {
    mutex = xSemaphoreCreateBinary();
    if (mutex == nullptr)
      return false;
  }

  ConfigureSpi();

  if (HAL_SPI_Init(hspi) != HAL_OK) {
    return false;
  }

  __HAL_SPI_ENABLE(hspi);

  initialized = true;
  xSemaphoreGive(mutex);
  return true;
}

bool SpiMaster::Write(const uint8_t* data, size_t size, const std::function<void()>& preTransactionHook) {
  if (data == nullptr)
    return false;

  auto ok = xSemaphoreTake(mutex, portMAX_DELAY);
  if (ok != pdTRUE)
    return false;

  if (preTransactionHook != nullptr) {
    preTransactionHook();
  }

  HAL_StatusTypeDef status = HAL_SPI_Transmit(hspi, const_cast<uint8_t*>(data), size, HAL_MAX_DELAY);

  xSemaphoreGive(mutex);
  return status == HAL_OK;
}

bool SpiMaster::Read(uint8_t* cmd, size_t cmdSize, uint8_t* data, size_t dataSize) {
  auto ok = xSemaphoreTake(mutex, portMAX_DELAY);
  if (ok != pdTRUE)
    return false;

  if (cmdSize > 0) {
    HAL_StatusTypeDef status = HAL_SPI_Transmit(hspi, cmd, cmdSize, HAL_MAX_DELAY);
    if (status != HAL_OK) {
      xSemaphoreGive(mutex);
      return false;
    }
  }

  if (dataSize > 0) {
    HAL_StatusTypeDef status = HAL_SPI_Receive(hspi, data, dataSize, HAL_MAX_DELAY);
    if (status != HAL_OK) {
      xSemaphoreGive(mutex);
      return false;
    }
  }

  xSemaphoreGive(mutex);
  return true;
}

void SpiMaster::Sleep() {
  if (initialized) {
    HAL_SPI_DeInit(hspi);
    __HAL_RCC_SPI1_CLK_DISABLE();
  }
}

void SpiMaster::Wakeup() {
  if (initialized) {
    __HAL_RCC_SPI1_CLK_ENABLE();
    ConfigureSpi();
    HAL_SPI_Init(hspi);
    __HAL_SPI_ENABLE(hspi);
  }
}