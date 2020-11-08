#ifndef GUI_Generic_Config_h
#define GUI_Generic_Config_h

// #define DEBUG_MODE

#define SUPLA_RELAY
#define SUPLA_BUTTON
#define SUPLA_LIMIT_SWITCH
#define SUPLA_ROLLERSHUTTER
#define SUPLA_CONFIG

// 1Wire
#define SUPLA_DS18B20
#define SUPLA_DHT11
#define SUPLA_DHT22
#define SUPLA_SI7021_SONOFF

// i2c
#define SUPLA_BME280
#define SUPLA_SHT30
#define SUPLA_SI7021
// #define SUPLA_HTU21D    // 0x40 NOT SUPPORTED
// #define SUPLA_SHT71     // 0x44 AND 0x45 NOT SUPPORTED
// #define SUPLA_BH1750    // 0x23 AND 0x5C NOT SUPPORTED
// #define SUPLA_MAX44009  // A0 NOT SUPPORTED

// SPI
#define SUPLA_MAX6675

// Other
#define SUPLA_HC_SR04
#define SUPLA_IMPULSE_COUNTER  // NOT SUPPORTED

#endif  // GUI-Generic_Config_h
