#ifndef GUI_Generic_Config_h
#define GUI_Generic_Config_h

//#define DEBUG_MODE
//#define SUPLA_OTA
// Language en - english, pl - polish (default if not defined UI_LANGUAGE)
// #define UI_LANGUAGE en

//#define GUI_Generic
//#define GUI_Generic_minimal
//#define GUI_Generic_lite
//#define GUI_Generic_sensors

/*********************************************************************************************\
 * GUI_Generic
\*********************************************************************************************/
#ifdef GUI_Generic

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
#define SUPLA_IMPULSE_COUNTER

#endif  // GUI_Generic

/*********************************************************************************************\
 * GUI_Generic_minimal
\*********************************************************************************************/
#ifdef GUI_Generic_minimal
#define SUPLA_RELAY
#define SUPLA_BUTTON
#define SUPLA_LIMIT_SWITCH
#define SUPLA_CONFIG
#endif  // GUI_Generic_minimal

/*********************************************************************************************\
 * GUI_Generic_lite
\*********************************************************************************************/
#ifdef GUI_Generic_lite
#define SUPLA_RELAY
#define SUPLA_BUTTON
#define SUPLA_LIMIT_SWITCH
#define SUPLA_CONFIG

// 1Wire
#define SUPLA_DS18B20
#define SUPLA_DHT22
#define SUPLA_SI7021_SONOFF
#endif  // GUI_Generic_lite

/*********************************************************************************************\
 * GUI_Generic_sensors
\*********************************************************************************************/
#ifdef GUI_Generic_sensors
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
#define SUPLA_PZEMv2           // NOT SUPPORTED
#define SUPLA_PZEMv3           // NOT SUPPORTED
#define SUPLA_IMPULSE_COUNTER  // NOT SUPPORTED

#endif  // GUI_Generic_sensors

#endif  // GUI-Generic_Config_h
