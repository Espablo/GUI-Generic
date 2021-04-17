
#ifndef SuplaWebPageSensorI2c_h
#define SuplaWebPageSensorI2c_h

#include "SuplaDeviceGUI.h"

#if defined(SUPLA_BME280) || defined(SUPLA_SHT3x) || defined(SUPLA_SI7021) || defined(SUPLA_OLED) || defined(SUPLA_MCP23017) || defined(SUPLA_BMP280)
#define GUI_SENSOR_I2C
#endif

#ifdef GUI_SENSOR_I2C

#if defined(SUPLA_BME280) || defined(SUPLA_BMP280)
enum _bmeAdress
{
  BMx280_ADDRESS_0X76 = 1,
  BMx280_ADDRESS_0X77,
  BMx280_ADDRESS_0X76_AND_0X77
};

#define INPUT_BME280          "bme280"
#define INPUT_BMP280          "bmp280"
#define INPUT_ALTITUDE_BMx280 "abme280"
#endif

#ifdef SUPLA_SHT3x
enum _shtAdress
{
  SHT3x_ADDRESS_0X44 = 1,
  SHT3x_ADDRESS_0X45,
  SHT3x_ADDRESS_0X44_AND_0X45
};
#endif

#define PATH_I2C "i2c"

#define INPUT_SDA_GPIO        "sdag"
#define INPUT_SCL_GPIO        "sclg"
#define INPUT_SHT3x           "sht30"
#define INPUT_SI7021          "si7021"
#define INPUT_OLED            "oled"
#define INPUT_OLED_ANIMATION  "oleda"
#define INPUT_OLED_BRIGHTNESS_TIME "oledb"
#define INPUT_OLED_BRIGHTNESS_LVL "oledc"
#define INPUT_OLED_NAME       "ion"
#define INPUT_MCP23017        "mcp"

void createWebPageSensorI2c();
void handleSensorI2c(int save = 0);
void handleSensorI2cSave();
#endif

#endif  // ifndef SuplaWebPageSensorI2c_h
