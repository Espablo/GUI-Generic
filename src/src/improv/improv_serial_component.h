// https://github.com/esphome/esphome/blob/dev/esphome/components/improv_serial/improv_serial_component.cpp

#ifndef improv_serial_component_h
#define improv_serial_component_h

#include <Arduino.h>
#include <improv.h>
#include <supla/element.h>

#define USE_ARDUINO

#ifdef USE_ARDUINO
#include <HardwareSerial.h>
#endif
#ifdef USE_ESP_IDF
#include <driver/uart.h>
#endif

enum ImprovSerialType : uint8_t
{
  TYPE_CURRENT_STATE = 0x01,
  TYPE_ERROR_STATE = 0x02,
  TYPE_RPC = 0x03,
  TYPE_RPC_RESPONSE = 0x04
};

static const uint8_t IMPROV_SERIAL_VERSION = 1;

class ImprovSerialComponent : public Supla::Element {
 public:
  ImprovSerialComponent();
  void onInit();
  void iterateAlways();

  float get_setup_priority() {
    return 0;
  }

 protected:
  bool parse_improv_serial_byte_(uint8_t byte);
  bool parse_improv_payload_(improv::ImprovCommand &command);

  void set_state_(improv::State state);
  void set_error_(improv::Error error);
  void send_response_(std::vector<uint8_t> &response);
  void on_wifi_connect_timeout_();

  std::vector<uint8_t> build_rpc_settings_response_(improv::Command command);
  std::vector<uint8_t> build_version_info_();

  int available_();
  uint8_t read_byte_();
  void write_data_(std::vector<uint8_t> &data);

#ifdef USE_ARDUINO
  HardwareSerial *hw_serial_{nullptr};
#endif
#ifdef USE_ESP_IDF
  uart_port_t uart_num_;
#endif

  std::vector<uint8_t> rx_buffer_;
  improv::State state_;
};
#endif