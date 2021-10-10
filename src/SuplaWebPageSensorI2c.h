
#ifndef SuplaWebPageSensorI2c_h
#define SuplaWebPageSensorI2c_h

#include "SuplaDeviceGUI.h"

#if defined(SUPLA_BME280) || defined(SUPLA_SHT3x) || defined(SUPLA_SI7021) || defined(SUPLA_OLED) || defined(SUPLA_MCP23017) || \
    defined(SUPLA_BMP280) || defined(SUPLA_VL53L0X) || defined(SUPLA_HDC1080)
#define GUI_SENSOR_I2C
#endif

#ifdef GUI_SENSOR_I2C

enum _sensorI2C
{
  SENSOR_I2C_BME280,
  SENSOR_I2C_SHT3x,
  SENSOR_I2C_SI7021,
  SENSOR_I2C_MAX6675,
  SENSOR_I2C_OLED,
  SENSOR_I2C_MCP23017,
  SENSOR_I2C_BMP280,
  SENSOR_I2C_VL53L0X,
  SENSOR_I2C_HDC1080,
};

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

#define INPUT_SDA_GPIO "sdag"
#define INPUT_SCL_GPIO "sclg"
#define INPUT_SHT3x    "sht30"
#define INPUT_SI7021   "si7021"

#ifdef SUPLA_OLED
#define INPUT_OLED                 "oled"
#define INPUT_OLED_ANIMATION       "oleda"
#define INPUT_OLED_BRIGHTNESS_TIME "oledb"
#define INPUT_OLED_BRIGHTNESS_LVL  "oledc"
#define INPUT_OLED_NAME            "ion"
#ifndef INPUT_BUTTON_GPIO
#define INPUT_BUTTON_GPIO "btg"
#endif
#endif  // SUPLA_OLED

#define INPUT_MCP23017 "mcp"

void createWebPageSensorI2c();
void handleSensorI2c(int save = 0);
void handleSensorI2cSave();

#ifdef SUPLA_VL53L0X
#define INPUT_VL53L0X "ilox"
#endif

#ifdef SUPLA_HDC1080
#define INPUT_HDC1080 "ihdc"
#endif

#endif

#endif  // ifndef SuplaWebPageSensorI2c_h
