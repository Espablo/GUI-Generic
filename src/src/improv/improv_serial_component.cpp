// https://github.com/esphome/esphome/blob/dev/esphome/components/improv_serial/improv_serial_component.cpp
#include "improv_serial_component.h"
#include "../../SuplaDeviceGUI.h"

ImprovSerialComponent::ImprovSerialComponent() {
#ifdef USE_ARDUINO
  this->hw_serial_ = &Serial;
#endif
#ifdef USE_ESP_IDF
  this->uart_num_ = &UART_NUM_0;
#endif
  this->state_ = improv::STATE_AUTHORIZED;
};

int ImprovSerialComponent::available_() {
#ifdef USE_ARDUINO
  return this->hw_serial_->available();
#endif
#ifdef USE_ESP_IDF
  size_t available;
  uart_get_buffered_data_len(this->uart_num_, &available);
  return available;
#endif
}

uint8_t ImprovSerialComponent::read_byte_() {
  uint8_t data;
#ifdef USE_ARDUINO
  this->hw_serial_->readBytes(&data, 1);
#endif
#ifdef USE_ESP_IDF
  uart_read_bytes(this->uart_num_, &data, 1, 20 / portTICK_RATE_MS);
#endif
  return data;
}

void ImprovSerialComponent::write_data_(std::vector<uint8_t> &data) {
  data.push_back('\n');
#ifdef USE_ARDUINO
  this->hw_serial_->write(data.data(), data.size());
#endif
#ifdef USE_ESP_IDF
  uart_write_bytes(this->uart_num_, data.data(), data.size());
#endif
}

void ImprovSerialComponent::onInit() {
  if (Supla::Network::IsReady()) {
    this->state_ = improv::STATE_PROVISIONED;
  }
}

void ImprovSerialComponent::iterateAlways() {
  if (this->state_ == improv::STATE_AUTHORIZED || this->state_ == improv::STATE_PROVISIONING) {
    if (Supla::Network::IsReady()) {
      this->set_state_(improv::STATE_PROVISIONED);

      std::vector<uint8_t> url = this->build_rpc_settings_response_(improv::WIFI_SETTINGS);
      this->send_response_(url);
    }
  }

  const uint32_t now = millis();
  if (now - this->last_read_byte_ > 50) {
    this->rx_buffer_.clear();
    this->last_read_byte_ = now;
  }

  while (this->available_()) {
    uint8_t byte = this->read_byte_();
    if (this->parse_improv_serial_byte_(byte)) {
      this->last_read_byte_ = now;
    }
    else {
      this->rx_buffer_.clear();
    }
  }

  //   if (!this->available_()) {
  //     return;
  //   }

  //   long now = millis();
  //   long lastMsg = now;

  //   while (millis() - lastMsg < 50) {
  //     if (this->available_()) {
  //       lastMsg = now;
  //       uint8_t byte = this->read_byte_();

  //       if (!this->parse_improv_serial_byte_(byte)) {
  //         this->rx_buffer_.clear();
  //       }
  //     }
  //   }
}

std::vector<uint8_t> ImprovSerialComponent::build_rpc_settings_response_(improv::Command command) {
  std::vector<std::string> urls;

  std::string ip = WiFi.localIP().toString().c_str();
  std::string webserver_url = "http://" + ip;
  urls.push_back(webserver_url);

  std::vector<uint8_t> data = improv::build_rpc_response(command, urls, false);
  return data;
}

std::vector<uint8_t> ImprovSerialComponent::build_version_info_() {
#ifdef ARDUINO_ARCH_ESP8266
  std::string type = "ESP8266";
#else
  std::string type = "ESP32";
#endif

  std::vector<std::string> infos = {"GUI-Generic", Supla::Channel::reg_dev.SoftVer, type, ConfigManager->get(KEY_HOST_NAME)->getValue()};
  std::vector<uint8_t> data = improv::build_rpc_response(improv::GET_DEVICE_INFO, infos, false);
  return data;
};

bool ImprovSerialComponent::parse_improv_serial_byte_(uint8_t byte) {
  int at = this->rx_buffer_.size();
  this->rx_buffer_.push_back(byte);
  const uint8_t *raw = &this->rx_buffer_[0];
  if (at == 0)
    return byte == 'I';
  if (at == 1)
    return byte == 'M';
  if (at == 2)
    return byte == 'P';
  if (at == 3)
    return byte == 'R';
  if (at == 4)
    return byte == 'O';
  if (at == 5)
    return byte == 'V';

  if (at == 6)
    return byte == IMPROV_SERIAL_VERSION;

  if (at == 7)
    return true;
  uint8_t type = raw[7];

  if (at == 8)
    return true;
  uint8_t data_len = raw[8];

  if (at < 8 + data_len)
    return true;

  if (at == 8 + data_len)
    return true;

  if (at == 8 + data_len + 1) {
    uint8_t checksum = 0x00;
    for (int i = 0; i < at; i++) checksum += raw[i];

    if (checksum != byte) {
      // Serial.print("Error decoding Improv payload");
      this->set_error_(improv::ERROR_INVALID_RPC);
      return false;
    }

    if (type == TYPE_RPC) {
      this->set_error_(improv::ERROR_NONE);
      auto command = improv::parse_improv_data(&raw[9], data_len, false);
      return this->parse_improv_payload_(command);
    }
  }

  // If we got here then the command coming is is improv, but not an RPC command
  return false;
}

bool ImprovSerialComponent::parse_improv_payload_(improv::ImprovCommand &command) {
  switch (command.command) {
    case improv::WIFI_SETTINGS: {
      ConfigManager->set(KEY_WIFI_SSID, command.ssid.c_str());
      ConfigManager->set(KEY_WIFI_PASS, command.password.c_str());
      ConfigManager->save();

      Supla::GUI::setupConnection();

      this->set_state_(improv::STATE_AUTHORIZED);
      // this->set_state_(improv::STATE_PROVISIONING);
      //  Serial.printf(
      //     "Received Improv wifi settings ssid=%s, password="
      //     "%s",
      //     command.ssid.c_str(), command.password.c_str());

      // auto f = std::bind(&ImprovSerialComponent::on_wifi_connect_timeout_, this);
      //  this->set_timeout("wifi-connect-timeout", 30000, f);
      return true;
    }
    case improv::GET_CURRENT_STATE:
      this->set_state_(this->state_);
      if (this->state_ == improv::STATE_PROVISIONED) {
        std::vector<uint8_t> url = this->build_rpc_settings_response_(improv::GET_CURRENT_STATE);
        this->send_response_(url);
      }
      return true;
    case improv::GET_DEVICE_INFO: {
      std::vector<uint8_t> info = this->build_version_info_();
      this->send_response_(info);

      return true;
    }
    default: {
      Serial.println("Unknown Improv payload");
      this->set_error_(improv::ERROR_UNKNOWN_RPC);
      return false;
    }
  }
}

void ImprovSerialComponent::set_state_(improv::State state) {
  this->state_ = state;

  std::vector<uint8_t> data = {'I', 'M', 'P', 'R', 'O', 'V'};
  data.resize(11);
  data[6] = IMPROV_SERIAL_VERSION;
  data[7] = TYPE_CURRENT_STATE;
  data[8] = 1;
  data[9] = state;

  uint8_t checksum = 0x00;
  for (uint8_t d : data) checksum += d;
  data[10] = checksum;

  this->write_data_(data);
}

void ImprovSerialComponent::set_error_(improv::Error error) {
  std::vector<uint8_t> data = {'I', 'M', 'P', 'R', 'O', 'V'};
  data.resize(11);
  data[6] = IMPROV_SERIAL_VERSION;
  data[7] = TYPE_ERROR_STATE;
  data[8] = 1;
  data[9] = error;

  uint8_t checksum = 0x00;
  for (uint8_t d : data) checksum += d;
  data[10] = checksum;
  this->write_data_(data);
}

void ImprovSerialComponent::send_response_(std::vector<uint8_t> &response) {
  std::vector<uint8_t> data = {'I', 'M', 'P', 'R', 'O', 'V'};
  data.resize(9);
  data[6] = IMPROV_SERIAL_VERSION;
  data[7] = TYPE_RPC_RESPONSE;
  data[8] = response.size();
  data.insert(data.end(), response.begin(), response.end());

  uint8_t checksum = 0x00;
  for (uint8_t d : data) checksum += d;
  data.push_back(checksum);

  this->write_data_(data);
}

void ImprovSerialComponent::on_wifi_connect_timeout_() {
  this->set_error_(improv::ERROR_UNABLE_TO_CONNECT);
  this->set_state_(improv::STATE_AUTHORIZED);
  // Serial.println("Timed out trying to connect to given WiFi network");
}
