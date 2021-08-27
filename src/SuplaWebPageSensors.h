#ifndef SuplaWebPageSensors_h
#define SuplaWebPageSensors_h

enum _sensorI2C
{
  SENSOR_I2C_BME280,
  SENSOR_I2C_SHT3x,
  SENSOR_I2C_SI7021,
  SENSOR_I2C_MAX6675,
  SENSOR_I2C_OLED,
  SENSOR_I2C_MCP23017,
  SENSOR_I2C_BMP280,
  SENSOR_I2C_VL53L0X
};

void createWebPageSensors();

#endif  // SuplaWebPageSensors_h
