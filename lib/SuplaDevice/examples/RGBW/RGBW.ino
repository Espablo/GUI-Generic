/*
Copyright (C) AC SOFTWARE SP. Z O.O.

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

#include <SPI.h>
#include <SuplaDevice.h>
#include <supla/control/rgbw_base.h>

// Choose proper network interface for your card:
// Arduino Mega with EthernetShield W5100:
#include <supla/network/ethernet_shield.h>
// Ethernet MAC address
uint8_t mac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
Supla::EthernetShield ethernet(mac);
//
// Arduino Mega with ENC28J60:
// #include <supla/network/ENC28J60.h>
// Supla::ENC28J60 ethernet(mac);
//
// ESP8266 based board:
// #include <supla/network/esp_wifi.h>
// Supla::ESPWifi wifi("your_wifi_ssid", "your_wifi_password");
//
// ESP32 based board:
// #include <supla/network/esp32_wifi.h>
// Supla::ESP32Wifi wifi("your_wifi_ssid", "your_wifi_password");

/*
 * Youtube: https://youtu.be/FE9tqzTjmA4
 * Youtube example was done on older version of SuplaDevice library
 */

#define RED_PIN              44
#define GREEN_PIN            45
#define BLUE_PIN             46
#define BRIGHTNESS_PIN       7
#define COLOR_BRIGHTNESS_PIN 8

class RgbwLeds : public Supla::Control::RGBWBase {
 public:
  RgbwLeds(int redPin,
           int greenPin,
           int bluePin,
           int colorBrightnessPin,
           int brightnessPin)
      : redPin(redPin),
        greenPin(greenPin),
        bluePin(bluePin),
        colorBrightnessPin(colorBrightnessPin),
        brightnessPin(brightnessPin) {
  }

  void setRGBWValueOnDevice(uint8_t red,
                            uint8_t green,
                            uint8_t blue,
                            uint8_t colorBrightness,
                            uint8_t brightness) {
    analogWrite(brightnessPin, (brightness * 255) / 100);
    analogWrite(colorBrightnessPin, (colorBrightness * 255) / 100);
    analogWrite(redPin, red);
    analogWrite(greenPin, green);
    analogWrite(bluePin, blue);
  }

 protected:
  int redPin;
  int greenPin;
  int bluePin;
  int brightnessPin;
  int colorBrightnessPin;
};

void setup() {
  Serial.begin(9600);

  // Replace the falowing GUID with value that you can retrieve from https://www.supla.org/arduino/get-guid
  char GUID[SUPLA_GUID_SIZE] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

  // Replace the following AUTHKEY with value that you can retrieve from: https://www.supla.org/arduino/get-authkey
  char AUTHKEY[SUPLA_AUTHKEY_SIZE] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

  /*
   * Having your device already registered at cloud.supla.org,
   * you want to change CHANNEL sequence or remove any of them,
   * then you must also remove the device itself from cloud.supla.org.
   * Otherwise you will get "Channel conflict!" error.
   */

  // CHANNEL0 - RGB controller and dimmer (RGBW)
  new RgbwLeds(
      RED_PIN, GREEN_PIN, BLUE_PIN, COLOR_BRIGHTNESS_PIN, BRIGHTNESS_PIN);

  /*
   * SuplaDevice Initialization.
   * Server address is available at https://cloud.supla.org 
   * If you do not have an account, you can create it at
   * https://cloud.supla.org/account/create SUPLA and SUPLA CLOUD are free of
   * charge
   *
   */

  SuplaDevice.begin(
      GUID,              // Global Unique Identifier
      "svr1.supla.org",  // SUPLA server address
      "email@address",   // Email address used to login to Supla Cloud
      AUTHKEY);          // Authorization key
}

void loop() {
  SuplaDevice.iterate();
}
