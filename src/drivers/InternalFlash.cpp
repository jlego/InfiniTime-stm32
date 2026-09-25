#include "drivers/InternalFlash.h"
#include "stm32wbxx_hal.h"

using namespace Pinetime::Drivers;

void InternalFlash::ErasePage(uint32_t address) {
  HAL_FLASH_Unlock();

  FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t PageError = 0;

  EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.Page = (address - FLASH_BASE) / FLASH_PAGE_SIZE;
  EraseInitStruct.NbPages = 1;

  HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);

  HAL_FLASH_Lock();
}

void InternalFlash::WriteWord(uint32_t address, uint32_t value) {
  HAL_FLASH_Unlock();
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, value);
  HAL_FLASH_Lock();
}

void InternalFlash::Wait() {
}