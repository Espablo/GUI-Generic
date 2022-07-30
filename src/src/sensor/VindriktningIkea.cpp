/*
  Copyright (C) krycha88

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

//https://github.com/Hypfer/esp8266-vindriktning-particle-sensor

#include "VindriktningIkea.h"

namespace Supla {
namespace Sensor {
VindriktningIkea::VindriktningIkea(int8_t _pin_rx) {
#ifdef ARDUINO_ARCH_ESP8266
  _serial = new SoftwareSerial(_pin_rx);
  _serial->enableIntTx(false);
  _serial->begin(VINDRIKTNING_IKEA_BAUDRATE);
#elif ESP32
  _serial = new HardwareSerial(_pin_rx);
  _serial->begin(VINDRIKTNING_IKEA_BAUDRATE);
#endif
}

double VindriktningIkea::getValue() {
  double value = TEMPERATURE_NOT_AVAILABLE;

  if (state.valid) {
    value = state.avgPM25;
  }

  return value;
}

void VindriktningIkea::iterateAlways() {
  if (millis() - lastReadTime > 10000) {
    lastReadTime = millis();
    handleUart(state);
    channel.setNewValue(getValue());
  }
}

void VindriktningIkea::onInit() {
  handleUart(state);
  channel.setNewValue(getValue());
}

void VindriktningIkea::clearRxBuf() {
  // Clear everything for the next message
  memset(serialRxBuf, 0, sizeof(serialRxBuf));
  rxBufIdx = 0;
}

void VindriktningIkea::parseState(particleSensorState_t& state) {
  /**
   *         MSB  DF 3     DF 4  LSB
   * uint16_t = xxxxxxxx xxxxxxxx
   */
  const uint16_t pm25 = (serialRxBuf[5] << 8) | serialRxBuf[6];

  Serial.printf("Received PM 2.5 reading: %d\n", pm25);

  state.measurements[state.measurementIdx] = pm25;
  state.measurementIdx = (state.measurementIdx + 1) % 5;

  if (state.measurementIdx == 0) {
    float avgPM25 = 0.0f;

    for (uint8_t i = 0; i < 5; ++i) {
      avgPM25 += state.measurements[i] / 5.0f;
    }

    state.avgPM25 = avgPM25;
    state.valid = true;

    Serial.printf("New Avg PM25: %d\n", state.avgPM25);
  }

  clearRxBuf();
}

bool VindriktningIkea::isValidHeader() {
  bool headerValid = serialRxBuf[0] == 0x16 && serialRxBuf[1] == 0x11 && serialRxBuf[2] == 0x0B;

  if (!headerValid) {
    Serial.println(F("Received message with invalid header."));
  }

  return headerValid;
}

bool VindriktningIkea::isValidChecksum() {
  uint8_t checksum = 0;

  for (uint8_t i = 0; i < 20; i++) {
    checksum += serialRxBuf[i];
  }

  if (checksum != 0) {
    Serial.printf("Received message with invalid checksum. Expected: 0. Actual: %d\n", checksum);
  }

  return checksum == 0;
}

void VindriktningIkea::handleUart(particleSensorState_t& state) {
  if (!_serial->available()) {
    return;
  }

  Serial.print(F("Receiving:"));
  long now = millis();
  long lastMsg = now;
  // wait for data with a timeout.
  while (millis() - lastMsg < 15) {
    if (_serial->available()) {
      lastMsg = millis();  // re-init the timeout timer since we received new data.

      serialRxBuf[rxBufIdx++] = _serial->read();
      Serial.print(F("."));

      if (rxBufIdx >= 64) {
        clearRxBuf();
      }
    }
  }
  Serial.println(F("Done."));

  if (isValidHeader() && isValidChecksum()) {
    parseState(state);

    Serial.printf("Current measurements: %d, %d, %d, %d, %d\n", state.measurements[0], state.measurements[1], state.measurements[2],
                  state.measurements[3], state.measurements[4]);
  }
  else {
    clearRxBuf();
  }
}

}  // namespace Sensor
}  // namespace Supla