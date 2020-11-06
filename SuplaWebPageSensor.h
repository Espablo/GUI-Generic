#ifndef SuplaWebPageSensor_h
#define SuplaWebPageSensor_h

#include "SuplaDeviceGUI.h"
#include "SuplaWebServer.h"

#define PATH_MULTI_DS      "multids"
#define PATH_SAVE_MULTI_DS "savemultids"
#define PATH_1WIRE         "1wire"
#define PATH_SAVE_1WIRE    "save1wire"
#define PATH_I2C           "i2c"
#define PATH_SAVE_I2C      "savei2c"
#define PATH_SPI           "spi"
#define PATH_SAVE_SPI      "savespi"

#define INPUT_MULTI_DS_GPIO   "mdsg"
#define INPUT_DHT11_GPIO      "dht11"
#define INPUT_DHT22_GPIO      "dht22"
#define INPUT_SDA_GPIO        "sdag"
#define INPUT_SCL_GPIO        "sclg"
#define INPUT_BME280          "bme280"
#define INPUT_ALTITUDE_BME280 "abme280"
#define INPUT_SHT30           "sht30"
#define INPUT_SI7021          "si7021"
#define INPUT_SI7021_SONOFF   "si7021sonoff"
#define INPUT_TRIG_GPIO       "trig"
#define INPUT_ECHO_GPIO       "echo"
#define INPUT_MAX_DHT11       "mdht11"
#define INPUT_MAX_DHT22       "mdht22"
#define INPUT_MAX_DS18B20     "maxds"

enum _sensorI2C
{
  SENSOR_BME280,
  SENSOR_SHT30,
  SENSOR_SI7021
};

#ifdef SUPLA_BME280
enum _bmeAdress
{
  BME280_ADDRESS_0X76 = 1,
  BME280_ADDRESS_0X77,
  BME280_ADDRESS_0X76_AND_0X77
};
#endif

#ifdef SUPLA_SHT30
enum _shtAdress
{
  SHT30_ADDRESS_0X44 = 1,
  SHT30_ADDRESS_0X45,
  SHT30_ADDRESS_0X44_AND_0X45
};
#endif

class SuplaWebPageSensor {
 public:
  SuplaWebPageSensor();
  void createWebPageSensor();

#if defined(SUPLA_DS18B20) || defined(SUPLA_DHT11) || defined(SUPLA_DHT22) || defined(SUPLA_SI7021_SONOFF)
  void handle1Wire();
  void handle1WireSave();
#endif
  
#ifdef SUPLA_DS18B20
  void handleSearchDS();
  void handleDSSave();
  void showDS18B20(String& content, bool readonly = false);
#endif

#if defined(SUPLA_BME280) || defined(SUPLA_HC_SR04) || defined(SUPLA_SHT30) || defined(SUPLA_SI7021)
  void handlei2c();
  void handlei2cSave();
#endif

 private:
#if defined(SUPLA_DS18B20) || defined(SUPLA_DHT11) || defined(SUPLA_DHT22) || defined(SUPLA_SI7021_SONOFF)
  String supla_webpage_1wire(int save);
#ifdef SUPLA_DS18B20
  String supla_webpage_search(int save);
#endif
#endif

#if defined(SUPLA_BME280) || defined(SUPLA_HC_SR04) || defined(SUPLA_SHT30) || defined(SUPLA_SI7021)
  String supla_webpage_i2c(int save);
#endif
};

extern SuplaWebPageSensor* WebPageSensor;
#endif  // SuplaWebPageSensor_h
