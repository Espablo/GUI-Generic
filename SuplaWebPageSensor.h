#ifndef SuplaWebPageSensor_h
#define SuplaWebPageSensor_h

#include "SuplaWebServer.h"
#include "SuplaDeviceGUI.h"

#define  PATH_SERCH             "serch"
#define  PATH_SAVE_DS           "saveds"
#define  PATH_SENSOR            "sensor"
#define  PATH_SAVE_SENSOR       "savesensor"
#define  PATH_MULTI_DS          "multids"
#define  PATH_SAVE_MULTI_DS     "savemultids"

#define  INPUT_MULTI_DS_GPIO       "mdsg"
#define  INPUT_DHT11_GPIO          "dht11"
#define  INPUT_DHT22_GPIO          "dht22"
#define  INPUT_SDA_GPIO            "sdag"
#define  INPUT_SCL_GPIO            "sclg"
#define  INPUT_BME280              "bme280"
#define  INPUT_ALTITUDE_BME280      "abme280"
#define  INPUT_TRIG_GPIO            "trig"
#define  INPUT_ECHO_GPIO            "echo"
#define  INPUT_MAX_DHT11            "mdht11"
#define  INPUT_MAX_DHT22            "mdht22"
#define  INPUT_MAX_DS18B20          "maxds"


class SuplaWebPageSensor {
  public:
    SuplaWebPageSensor();
    void createWebPageSensor();
    void handleSensor();
    void handleSensorSave();
    
#ifdef SUPLA_DS18B20
    void handleSearchDS();
    void handleDSSave();
#endif
    
  private:
    String supla_webpage_sensor(int save);
#ifdef SUPLA_DS18B20
    String supla_webpage_search(int save);
#endif
    const char* SupportedBme280[3] = {
      "OFF",
      "0x76",
      "0x77"
    };
};

extern SuplaWebPageSensor *WebPageSensor;
#endif // SuplaWebPageSensor_h
