#ifndef _S_MCP23017_H
#define _S_MCP23017_H

#include <Arduino.h>
#include <functional>

#include <supla/io.h>

#define EXPENDER_SHIFT_MCP23017     80
#define EXPENDER_SHIFT_PIN_MCP23017 16

class MCP23017 {
 public:
  typedef std::function<void(uint16_t pins, uint16_t values)> callback_t;

  MCP23017() : _callback(NULL) {
  }

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  static void init(uint8_t sda, uint8_t scl, bool fast = true);
  static void init(bool fast = true) {
    init(SDA, SCL, fast);
  }
#else
  static void init(bool fast = true);
#endif

  bool begin(uint8_t address = 0);

  void pinMode(uint8_t pin, uint8_t mode);
  void setPullup(uint8_t pin, bool pullup, bool inverse = false);
  bool digitalRead(uint8_t pin);
  void digitalWrite(uint8_t pin, bool value);
  uint16_t digitalReads();
  void digitalWrites(uint16_t values);

  void attachInterrupt(uint8_t pin, callback_t callback);
  void detachInterrupt(uint8_t pin);
  void setupInterrupt(uint8_t pin, bool enable = true);
  void setupInterrupts(uint16_t pins, bool enable = true);

 protected:
  bool writeReg(uint8_t reg, uint8_t value);
  bool writeReg16(uint8_t reg, uint16_t value);
  uint8_t readReg(uint8_t reg);
  uint16_t readReg16(uint8_t reg);
  bool updateReg(uint8_t reg, uint8_t andMask, uint8_t orMask);
  bool updateReg16(uint8_t reg, uint16_t andMask, uint16_t orMask);

  void _interrupt();

  uint8_t _address;
  uint16_t ba = 0;
  unsigned long get_ba = 0;
  callback_t _callback;
};

namespace Supla {
namespace Control {
class MCP_23017 : public Supla::Io {
 public:
  MCP_23017();
  void customDigitalWrite(int channelNumber, uint8_t pin, uint8_t val);
  int customDigitalRead(int channelNumber, uint8_t pin);
  void customPinMode(int channelNumber, uint8_t pin, uint8_t mode);
  void setPullup(uint8_t pin, bool pullup, bool inverse);

  MCP23017 *mcp1;
  MCP23017 *mcp2;
  MCP23017 *mcp3;
  MCP23017 *mcp4;
};

}  // namespace Control
}  // namespace Supla
#endif
