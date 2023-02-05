#include "MCP_23017.h"
#include "../../SuplaDeviceGUI.h"
#include <FunctionalInterrupt.h>

static const uint8_t MCP23017_BASEADDRESS = 0x20;
static const uint8_t MCP23017_IODIRA = 0x00;
static const uint8_t MCP23017_IODIRB = 0x01;
static const uint8_t MCP23017_IPOLA = 0x02;
static const uint8_t MCP23017_IPOLB = 0x03;
static const uint8_t MCP23017_GPINTENA = 0x04;
static const uint8_t MCP23017_GPINTENB = 0x05;
static const uint8_t MCP23017_DEFVALA = 0x06;
static const uint8_t MCP23017_DEFVALB = 0x07;
static const uint8_t MCP23017_INTCONA = 0x08;
static const uint8_t MCP23017_INTCONB = 0x09;
static const uint8_t MCP23017_IOCONA = 0x0A;
static const uint8_t MCP23017_IOCONB = 0x0B;
static const uint8_t MCP23017_GPPUA = 0x0C;
static const uint8_t MCP23017_GPPUB = 0x0D;
static const uint8_t MCP23017_INTFA = 0x0E;
static const uint8_t MCP23017_INTFB = 0x0F;
static const uint8_t MCP23017_INTCAPA = 0x10;
static const uint8_t MCP23017_INTCAPB = 0x11;
static const uint8_t MCP23017_GPIOA = 0x12;
static const uint8_t MCP23017_GPIOB = 0x13;
static const uint8_t MCP23017_OLATA = 0x14;
static const uint8_t MCP23017_OLATB = 0x15;

MCP23017::MCP23017(TwoWire* wire) : _wire(wire), _callback(NULL) {
}

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
void MCP23017::init(uint8_t sda, uint8_t scl, bool fast) {
  _wire->begin(sda, scl);
  if (fast)
    _wire->setClock(400000);
}
#else
void MCP23017::init(bool fast) {
  _wire->begin();
  if (fast)
    _wire->setClock(400000);
}
#endif

bool MCP23017::begin(uint8_t address) {
  _address = MCP23017_BASEADDRESS | (address & 0x07);
  return (writeReg16(MCP23017_IOCONA, 0x4242) && writeReg16(MCP23017_IODIRA,
                                                            0xFFFF));  // INT MIRROR & INT POL HIGH, ALL INPUTS
}

void MCP23017::pinMode(uint8_t pin, uint8_t mode) {
  if (pin < 16) {
    if (mode == OUTPUT) {
      updateReg(pin < 8 ? MCP23017_IODIRA : MCP23017_IODIRB, ~(uint8_t)(1 << (pin % 8)), 0x00);
    }
    else if ((mode == INPUT) || (mode == INPUT_PULLUP)) {
      updateReg(pin < 8 ? MCP23017_IODIRA : MCP23017_IODIRB, 0xFF, 1 << (pin % 8));
      if (mode == INPUT_PULLUP) {
        updateReg(pin < 8 ? MCP23017_GPPUA : MCP23017_GPPUB, 0xFF, 1 << (pin % 8));
        // updateReg(
        //     pin < 8 ? MCP23017_IPOLA : MCP23017_IPOLB, 0xFF, 1 << (pin % 8));
      }
      else {
        updateReg(pin < 8 ? MCP23017_GPPUA : MCP23017_GPPUB, ~(uint8_t)(1 << (pin % 8)), 0x00);
        //  updateReg(pin < 8 ? MCP23017_IPOLA : MCP23017_IPOLB,
        //            ~(uint8_t)(1 << (pin % 8)),
        //            0x00);
      }
    }
  }
}

void MCP23017::setPullup(uint8_t pin, bool pullup, bool inverse) {
  if (pin < 16) {
    if (pullup)
      updateReg(pin < 8 ? MCP23017_GPPUA : MCP23017_GPPUB, 0xFF, 1 << (pin % 8));
    else
      updateReg(pin < 8 ? MCP23017_GPPUA : MCP23017_GPPUB, ~(uint8_t)(1 << (pin % 8)), 0x00);
    if (inverse)
      updateReg(pin < 8 ? MCP23017_IPOLA : MCP23017_IPOLB, 0xFF, 1 << (pin % 8));
    else
      updateReg(pin < 8 ? MCP23017_IPOLA : MCP23017_IPOLB, ~(uint8_t)(1 << (pin % 8)), 0x00);
  }
}

bool MCP23017::digitalRead(uint8_t pin) {
  if (millis() - get_ba > 30) {
    ba = readReg16(MCP23017_GPIOA);
    get_ba = millis();
  }
  // if (pin < 16) {
  return ((ba >> (pin % 16)) & 0x01);
  // }
}

void MCP23017::digitalWrite(uint8_t pin, bool value) {
  if (pin < 16) {
    if (value)
      updateReg(pin < 8 ? MCP23017_GPIOA : MCP23017_GPIOB, 0xFF, 1 << (pin % 8));
    else
      updateReg(pin < 8 ? MCP23017_GPIOA : MCP23017_GPIOB, ~(uint8_t)(1 << (pin % 8)), 0x00);
  }
}

uint16_t MCP23017::digitalReads() {
  return readReg16(MCP23017_GPIOA);
}

void MCP23017::digitalWrites(uint16_t values) {
  writeReg16(MCP23017_GPIOA, values);
}

void MCP23017::attachInterrupt(uint8_t pin, callback_t callback) {
  _callback = callback;
  ::pinMode(pin, INPUT);
  ::attachInterrupt(digitalPinToInterrupt(pin), std::bind(&MCP23017::_interrupt, this), RISING);
}

void MCP23017::detachInterrupt(uint8_t pin) {
  ::detachInterrupt(digitalPinToInterrupt(pin));
  _callback = NULL;
}

void MCP23017::setupInterrupt(uint8_t pin, bool enable) {
  if (pin < 16) {
    if (enable)
      updateReg(pin < 8 ? MCP23017_GPINTENA : MCP23017_GPINTENB, 0xFF, 1 << (pin % 8));
    else
      updateReg(pin < 8 ? MCP23017_GPINTENA : MCP23017_GPINTENB, ~(uint8_t)(1 << (pin % 8)), 0x00);
  }
}

void MCP23017::setupInterrupts(uint16_t pins, bool enable) {
  if (enable)
    updateReg16(MCP23017_GPINTENA, 0xFFFF, pins);
  else
    updateReg16(MCP23017_GPINTENA, ~pins, 0x00);
}

bool MCP23017::writeReg(uint8_t reg, uint8_t value) {
  _wire->beginTransmission(_address);
  _wire->write(reg);
  _wire->write(value);
  return (_wire->endTransmission() == 0);
}

bool MCP23017::writeReg16(uint8_t reg, uint16_t value) {
  _wire->beginTransmission(_address);
  _wire->write(reg);
  _wire->write(value & 0xFF);
  _wire->write(value >> 8);
  return (_wire->endTransmission() == 0);
}

uint8_t MCP23017::readReg(uint8_t reg) {
  _wire->beginTransmission(_address);
  _wire->write(reg);
  if (_wire->endTransmission() != 0)
    return 0;  // Error!
  _wire->requestFrom(_address, (uint8_t)1);
  return _wire->read();
}

uint16_t MCP23017::readReg16(uint8_t reg) {
  _wire->beginTransmission(_address);
  _wire->write(reg);
  if (_wire->endTransmission() != 0)
    return 0;  // Error!
  _wire->requestFrom(_address, (uint8_t)2);
  uint8_t a = _wire->read();
  return ((_wire->read() << 8) | a);
}

bool MCP23017::updateReg(uint8_t reg, uint8_t andMask, uint8_t orMask) {
  _wire->beginTransmission(_address);
  _wire->write(reg);
  if (_wire->endTransmission() != 0)
    return false;  // Error!
  _wire->requestFrom(_address, (uint8_t)1);
  uint8_t a = (_wire->read() & andMask) | orMask;
  _wire->beginTransmission(_address);
  _wire->write(reg);
  _wire->write(a);
  return (_wire->endTransmission() == 0);
}

bool MCP23017::updateReg16(uint8_t reg, uint16_t andMask, uint16_t orMask) {
  _wire->beginTransmission(_address);
  _wire->write(reg);
  if (_wire->endTransmission() != 0)
    return false;  // Error!
  _wire->requestFrom(_address, (uint8_t)2);
  uint16_t ab = _wire->read();
  ab |= (_wire->read() << 8);
  ab &= andMask;
  ab |= orMask;
  _wire->beginTransmission(_address);
  _wire->write(reg);
  _wire->write(ab & 0xFF);
  _wire->write(ab >> 8);
  return (_wire->endTransmission() == 0);
}

void IRAM_ATTR MCP23017::_interrupt() {
  uint16_t pins, values;

  pins = readReg16(MCP23017_INTFA);
  values = readReg16(MCP23017_INTCAPA);
  if (_callback)
    _callback(pins, values);
}

namespace Supla {
namespace Control {
MCP_23017::MCP_23017() {
  mcp1 = new MCP23017(&getTwoWire(0));
  mcp2 = new MCP23017(&getTwoWire(1));
  mcp3 = new MCP23017(&getTwoWire(2));
  mcp4 = new MCP23017(&getTwoWire(3));

  if (!mcp1->begin(0)) {
    Serial.println(F("MCP23017 1 not found!"));  // begin(uint8_t address)  "Pin 100 - 115"
  }
  if (!mcp2->begin(1)) {
    Serial.println(F("MCP23017 2 not found!"));  // begin(uint8_t address)  "Pin 116 - 131"
  }
  if (!mcp3->begin(2)) {
    Serial.println(F("MCP23017 3 not found!"));  // begin(uint8_t address)  "Pin 132 - 147"
  }
  if (!mcp4->begin(3)) {
    Serial.println(F("MCP23017 4 not found!"));  // begin(uint8_t address)  "Pin 148 - 163"
  }
}

TwoWire& MCP_23017::getTwoWire(uint8_t address) {
#ifdef ARDUINO_ARCH_ESP32
  if (ConfigESP->getAdressMCP23017(0, FUNCTION_RELAY) == address &&
      ConfigManager->get(KEY_ACTIVE_EXPENDER)->getElement(FUNCTION_RELAY).toInt() == EXPENDER_MCP23017_I2C2) {
    Serial.print(F("Add Wire1 for address: "));
    Serial.println(address);
    return Wire1;
  }
  if (ConfigESP->getAdressMCP23017(0, FUNCTION_BUTTON) == address &&
      ConfigManager->get(KEY_ACTIVE_EXPENDER)->getElement(FUNCTION_BUTTON).toInt() == EXPENDER_MCP23017_I2C2) {
    Serial.print(F("Add Wire1 for address: "));
    Serial.println(address);
    return Wire1;
  }
  if (ConfigESP->getAdressMCP23017(0, FUNCTION_LIMIT_SWITCH) == address &&
      ConfigManager->get(KEY_ACTIVE_EXPENDER)->getElement(FUNCTION_LIMIT_SWITCH).toInt() == EXPENDER_MCP23017_I2C2) {
    Serial.print(F("Add Wire1 for address: "));
    Serial.println(address);
    return Wire1;
  }
#endif

  return Wire;
}

void MCP_23017::customDigitalWrite(int channelNumber, uint8_t pin, uint8_t val) {
  if ((pin >= EXPENDER_SHIFT_MCP23017) && (pin < EXPENDER_SHIFT_PIN_MCP23017 + EXPENDER_SHIFT_MCP23017)) {
    mcp1->digitalWrite(pin - EXPENDER_SHIFT_MCP23017, val);
    return;
  }
  if ((pin >= EXPENDER_SHIFT_PIN_MCP23017 + EXPENDER_SHIFT_MCP23017) && (pin < (EXPENDER_SHIFT_PIN_MCP23017 * 2) + EXPENDER_SHIFT_MCP23017)) {
    mcp2->digitalWrite(pin - EXPENDER_SHIFT_MCP23017 - EXPENDER_SHIFT_PIN_MCP23017, val);
    return;
  }
  if ((pin >= (EXPENDER_SHIFT_PIN_MCP23017 * 2) + EXPENDER_SHIFT_MCP23017) && (pin < (EXPENDER_SHIFT_PIN_MCP23017 * 3) + EXPENDER_SHIFT_MCP23017)) {
    mcp3->digitalWrite(pin - EXPENDER_SHIFT_MCP23017 - (EXPENDER_SHIFT_PIN_MCP23017 * 2), val);
    return;
  }
  if ((pin >= (EXPENDER_SHIFT_PIN_MCP23017 * 3) + EXPENDER_SHIFT_MCP23017) && (pin < (EXPENDER_SHIFT_PIN_MCP23017 * 4) + EXPENDER_SHIFT_MCP23017)) {
    mcp4->digitalWrite(pin - EXPENDER_SHIFT_MCP23017 - (EXPENDER_SHIFT_PIN_MCP23017 * 3), val);
    return;
  }

  return ::digitalWrite(pin, val);
}

int MCP_23017::customDigitalRead(int channelNumber, uint8_t pin) {
  if ((pin >= EXPENDER_SHIFT_MCP23017) && (pin < EXPENDER_SHIFT_PIN_MCP23017 + EXPENDER_SHIFT_MCP23017)) {
    return mcp1->digitalRead(pin - EXPENDER_SHIFT_MCP23017);
  }
  if ((pin >= EXPENDER_SHIFT_PIN_MCP23017 + EXPENDER_SHIFT_MCP23017) && (pin < (EXPENDER_SHIFT_PIN_MCP23017 * 2) + EXPENDER_SHIFT_MCP23017)) {
    return mcp2->digitalRead(pin - EXPENDER_SHIFT_MCP23017 - EXPENDER_SHIFT_PIN_MCP23017);
  }
  if ((pin >= (EXPENDER_SHIFT_PIN_MCP23017 * 2) + EXPENDER_SHIFT_MCP23017) && (pin < (EXPENDER_SHIFT_PIN_MCP23017 * 3) + EXPENDER_SHIFT_MCP23017)) {
    return mcp3->digitalRead(pin - EXPENDER_SHIFT_MCP23017 - (EXPENDER_SHIFT_PIN_MCP23017 * 2));
  }
  if ((pin >= (EXPENDER_SHIFT_PIN_MCP23017 * 3) + EXPENDER_SHIFT_MCP23017) && (pin < (EXPENDER_SHIFT_PIN_MCP23017 * 4) + EXPENDER_SHIFT_MCP23017)) {
    return mcp4->digitalRead(pin - EXPENDER_SHIFT_MCP23017 - (EXPENDER_SHIFT_PIN_MCP23017 * 3));
  }

  return ::digitalRead(pin);
}
void MCP_23017::customPinMode(int channelNumber, uint8_t pin, uint8_t mode) {
  (void)(channelNumber);

  if ((pin >= EXPENDER_SHIFT_MCP23017) && (pin < EXPENDER_SHIFT_PIN_MCP23017 + EXPENDER_SHIFT_MCP23017)) {
    mcp1->pinMode(pin - EXPENDER_SHIFT_MCP23017, mode);
  }
  if ((pin >= EXPENDER_SHIFT_PIN_MCP23017 + EXPENDER_SHIFT_MCP23017) && (pin < (EXPENDER_SHIFT_PIN_MCP23017 * 2) + EXPENDER_SHIFT_MCP23017)) {
    mcp2->pinMode(pin - EXPENDER_SHIFT_MCP23017 - EXPENDER_SHIFT_PIN_MCP23017, mode);
  }
  if ((pin >= (EXPENDER_SHIFT_PIN_MCP23017 * 2) + EXPENDER_SHIFT_MCP23017) && (pin < (EXPENDER_SHIFT_PIN_MCP23017 * 3) + EXPENDER_SHIFT_MCP23017)) {
    mcp3->pinMode(pin - EXPENDER_SHIFT_MCP23017 - (EXPENDER_SHIFT_PIN_MCP23017 * 2), mode);
  }
  if ((pin >= (EXPENDER_SHIFT_PIN_MCP23017 * 3) + EXPENDER_SHIFT_MCP23017) && (pin < (EXPENDER_SHIFT_PIN_MCP23017 * 4) + EXPENDER_SHIFT_MCP23017)) {
    mcp4->pinMode(pin - EXPENDER_SHIFT_MCP23017 - (EXPENDER_SHIFT_PIN_MCP23017 * 3), mode);
  }

  return ::pinMode(pin, mode);
}

void MCP_23017::setPullup(uint8_t pin, bool pullup, bool inverse) {
  if ((pin >= EXPENDER_SHIFT_MCP23017) && (pin < EXPENDER_SHIFT_PIN_MCP23017 + EXPENDER_SHIFT_MCP23017)) {
    mcp1->setPullup(pin - EXPENDER_SHIFT_MCP23017, pullup, inverse);
  }
  if ((pin >= EXPENDER_SHIFT_PIN_MCP23017 + EXPENDER_SHIFT_MCP23017) && (pin < (EXPENDER_SHIFT_PIN_MCP23017 * 2) + EXPENDER_SHIFT_MCP23017)) {
    mcp2->setPullup(pin - EXPENDER_SHIFT_MCP23017 - EXPENDER_SHIFT_PIN_MCP23017, pullup, inverse);
  }
  if ((pin >= (EXPENDER_SHIFT_PIN_MCP23017 * 2) + EXPENDER_SHIFT_MCP23017) && (pin < (EXPENDER_SHIFT_PIN_MCP23017 * 3) + EXPENDER_SHIFT_MCP23017)) {
    mcp3->setPullup(pin - EXPENDER_SHIFT_MCP23017 - (EXPENDER_SHIFT_PIN_MCP23017 * 2), pullup, inverse);
  }
  if ((pin >= (EXPENDER_SHIFT_PIN_MCP23017 * 3) + EXPENDER_SHIFT_MCP23017) && (pin < (EXPENDER_SHIFT_PIN_MCP23017 * 4) + EXPENDER_SHIFT_MCP23017)) {
    mcp4->setPullup(pin - EXPENDER_SHIFT_MCP23017 - (EXPENDER_SHIFT_PIN_MCP23017 * 3), pullup, inverse);
  }
}
}  // namespace Control
}  // namespace Supla