# CHANGELOG.md

## 2.4.2 (2022-06-20)

  - Change: (Arduino ESPx) Wi-Fi class handling change to support config mode
  - Change: StatusLed - change led sequence on error (300/100 ms)
  - Change: SuplaDevice removed entering to "No connection to network" state during first startup procedure for cleaner last status messages
  - Change: SuplaDevice renamed "Iterate fail" to "Communication fail" last state name.
  - Change: default device names adjustment
  - Change: Moved onInit methods for thermometers and therm-hygro meters to base classes.
  - Add: SuplaDevice: add handling of actions: enter config mode, toggle config mode, reset to factory default settings
  - Add: SuplaDevice: automatic GUID and AuthKey generation when it is missing and storage config is available. This functionality requires Storage Config class implementation of GUID/AuthKey generation on specific platform. By default it won't generate anything.
  - Add: RGBW, dimmer: add option to limit min/max values for brightness and colorBrightness
  - Add: SuplaDevice: ability to set user defined activity timeout
  - Add: VirtualThermometer
  - Add: VirtualThermHygroMeter.
  - Add: Element::onLoadConfig() method in which Element's config can be loaded
  - Add: set, clear, toggle methods for VirtualBinary
  - Add: SuplaDeviceClass::generateHostname method to generate DHCP hostname and AP Wi-Fi SSID in config mode
  - Add: support for SUPLA_CALCFG_CMD_ENTER_CFG_MODE (requires Storage::Config instance)
  - Add: Supla::Device::LastStateLogger to keep track of previous statuses and provide data for www
  - Add: WebServer class
  - Add: HTML blocks: DeviceInfo, ProtocolParameters, SwUpdate, SwUpdateBeta, WiFiParameters, CustomSwUpdate, StatusLedParameters
  - Add: (Arduino ESPx) LittleFsConfig class for configuration storage
  - Add: (ESP8266 RTOS, ESP IDF, Arduino ESPx, Linux): add GUID and AuthKey generation
  - Add: (ESP8266 RTOS, ESP IDF, Arduino ESPx): add web server for config mode
  - Add: (ESP8266 RTOS, ESP IDF) NvsConfig class for configuration storage
  - Add: (ESP8266 RTOS, ESP IDF) added detailed log to Last State for Wi-Fi connection problems and Supla server connection problem.
  - Add: (ESP8266 RTOS, ESP IDF) Sha256 and RsaVerificator implementation
  - Add: (ESP8266 RTOS, ESP IDF) OTA SW update procedure
  - Add: (Linux) time methods, timers implementation,
  - Add: (Linux) add ThermometerParsed, Supla::Source::Cmd, Supla::Parser::Simple, Supla::Parser::Json, ImpulseCouterParsed, ElectricityMeterParsed, Source::File, BinaryParsed
  - Add: (Linux) YAML config file support
  - Add: (Linux) file storage for last state log
  - Add: (Arduino ESPx) WebInterface Arduino example



## 2.4.1 (2022-03-23)

  - Change: (Arduino) move WiFi events for ESP8266 Arduino WiFi class to protected section
  - Change: (Arduino) Arduino ESP32 boards switch to version 2.x. Older boards will not compile ([see instructions](https://github.com/SUPLA/supla-device/commit/c533e73a4c811c026187374635dd812d4e294c8b))
  - Add: `Supla::Device::StatusLed` element (LED informing about currend device connection status)
  - Add: (Linux) support for compilation under Linux environment (partial implementation). Provided docker file for environment setup and example application.
  - Add: (FreeRTOS) support for compilation under FreeRTOS environment (partial implementation). Provided docker file for environment setup and example application.
  - Add: (ESP8266 RTOS) support for ESP8266 RTOS SDK envirotnment (partial implementation). Provided docker file for environment setup and example application.
  - Add: (ESP IDF) support for ESP IDF envirotnment (partial implementation). Provided docker file for environment setup and example application.
  - Add: `Supla::Io::pulseIn` and `Supla::Io::analogWrite` to interface
  - Add: (ESP8266 RTOS, ESP IDF) logging via ESP IDF logging lib
  - Add: (ESP8266 RTOS, ESP IDF) implementation for `delay`, `delayMicroseconds`, `millis`
  - Add: (ESP8266 RTOS, ESP IDF) implementation for `pinMode`, `digitalRead`, `digitalWrite`
  - Add: (ESP8266 RTOS, ESP IDF, Linux, FreeRTOS) `ChannelState` basic data reporting
  - Add: (ESP8266 RTOS, ESP IDF) `Supla::Storage` implementation on `spiffs`
  - Add: getters for electricity meter measured values
  - Add: PZEMv3 with custom PZEM address setting (allow to use single TX/RX pair for multiple PZEM units)

## 2.4.0 (2021-12-07)

All changes for this and older releases are for Arduino IDE target.

  - Add: Action Trigger support
  - Add: Conditions for Electricity meter
  - Add: MAX thermocouple
  - Change: Supla protocol version updated to 16
  - Fix: Roller shutter was sending invalid channel value for not calibrated state
  - Fix: Compilaton error and warnings cleanup (especially for ESP32)
