#ifndef SuplaWebPageSensor_h
#define SuplaWebPageSensor_h

#include "SuplaDeviceGUI.h"
#include "SuplaWebServer.h"

#define PATH_SENSOR        "sensor"
#define PATH_SAVE_SENSOR   "savesensor"
#define PATH_MULTI_DS      "multids"
#define PATH_SAVE_MULTI_DS "savemultids"

#define INPUT_MULTI_DS_GPIO   "mdsg"
#define INPUT_DHT11_GPIO      "dht11"
#define INPUT_DHT22_GPIO      "dht22"
#define INPUT_SDA_GPIO        "sdag"
#define INPUT_SCL_GPIO        "sclg"
#define INPUT_BME280          "bme280"
#define INPUT_ALTITUDE_BME280 "abme280"
#define INPUT_TRIG_GPIO       "trig"
#define INPUT_ECHO_GPIO       "echo"
#define INPUT_MAX_DHT11       "mdht11"
#define INPUT_MAX_DHT22       "mdht22"
#define INPUT_MAX_DS18B20     "maxds"

class SuplaWebPageSensor {
 public:
  SuplaWebPageSensor();
  void createWebPageSensor();
  void handleSensor();
  void handleSensorSave();

#ifdef SUPLA_DS18B20
  void handleSearchDS();
  void handleDSSave();
  void showDS18B20(String& content, bool readonly = false);
#endif

 private:
  String supla_webpage_sensor(int save);

#ifdef SUPLA_DS18B20
  String supla_webpage_search(int save);
#endif

#ifdef SUPLA_BME280
#define BME280_ADDRESS_0X76				1			
#define BME280_ADDRESS_0X77				2
#define BME280_ADDRESS_0X76_AND_0X77	3

  const char* SupportedBme280[4] = {
    "OFF", 
    "0x76", 
    "0x77",
    "0x76 & 0x77"};
};
#endif

extern SuplaWebPageSensor* WebPageSensor;
#endif  // SuplaWebPageSensor_h
