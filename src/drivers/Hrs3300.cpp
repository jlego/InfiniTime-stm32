#include "drivers/Hrs3300.h"
#include <algorithm>
#include <iterator>
#include "stm32wbxx_hal.h"
#include <FreeRTOS.h>
#include <task.h>

using namespace Pinetime::Drivers;

namespace {
  static constexpr uint8_t ledDriveCurrentValue = 0x2f;
}

Hrs3300::Hrs3300(TwiMaster& twiMaster, uint8_t twiAddress) : twiMaster {twiMaster}, twiAddress {twiAddress} {
}

void Hrs3300::Init() {
  Disable();
  vTaskDelay(100);

  WriteRegister(static_cast<uint8_t>(Registers::Enable), 0x50);
  WriteRegister(static_cast<uint8_t>(Registers::PDriver), ledDriveCurrentValue);
  WriteRegister(static_cast<uint8_t>(Registers::Res), 0x77);
  WriteRegister(static_cast<uint8_t>(Registers::Hgain), 0x00);
}

void Hrs3300::Enable() {
  auto value = ReadRegister(static_cast<uint8_t>(Registers::Enable));
  value |= 0x80;
  WriteRegister(static_cast<uint8_t>(Registers::Enable), value);
  WriteRegister(static_cast<uint8_t>(Registers::PDriver), ledDriveCurrentValue);
}

void Hrs3300::Disable() {
  auto value = ReadRegister(static_cast<uint8_t>(Registers::Enable));
  value &= ~0x80;
  WriteRegister(static_cast<uint8_t>(Registers::Enable), value);
  WriteRegister(static_cast<uint8_t>(Registers::PDriver), 0);
}

Hrs3300::PackedHrsAls Hrs3300::ReadHrsAls() {
  constexpr Registers dataRegisters[] =
    {Registers::C1dataM, Registers::C0DataM, Registers::C0DataH, Registers::C1dataH, Registers::C1dataL, Registers::C0dataL};
  constexpr uint8_t baseOffset = static_cast<uint8_t>(*std::min_element(std::begin(dataRegisters), std::end(dataRegisters)));
  constexpr uint8_t length = static_cast<uint8_t>(*std::max_element(std::begin(dataRegisters), std::end(dataRegisters))) - baseOffset + 1;

  Hrs3300::PackedHrsAls res;
  uint8_t buf[length];
  twiMaster.Read(twiAddress, baseOffset, buf, length);

  uint8_t m = static_cast<uint8_t>(Registers::C0DataM) - baseOffset;
  uint8_t h = static_cast<uint8_t>(Registers::C0DataH) - baseOffset;
  uint8_t l = static_cast<uint8_t>(Registers::C0dataL) - baseOffset;
  res.hrs = (buf[m] << 8) | ((buf[h] & 0x0f) << 4) | (buf[l] & 0x0f);

  m = static_cast<uint8_t>(Registers::C1dataM) - baseOffset;
  h = static_cast<uint8_t>(Registers::C1dataH) - baseOffset;
  l = static_cast<uint8_t>(Registers::C1dataL) - baseOffset;
  res.als = ((buf[h] & 0x3f) << 11) | (buf[m] << 3) | (buf[l] & 0x07);

  return res;
}

void Hrs3300::WriteRegister(uint8_t reg, uint8_t data) {
  twiMaster.Write(twiAddress, reg, &data, 1);
}

uint8_t Hrs3300::ReadRegister(uint8_t reg) {
  uint8_t value;
  twiMaster.Read(twiAddress, reg, &value, 1);
  return value;
}