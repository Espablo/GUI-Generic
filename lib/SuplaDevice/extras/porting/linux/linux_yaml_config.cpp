/*
 Copyright (C) AC SOFTWARE SP. Z O.O.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <IPAddress.h>
#include <supla-common/log.h>
#include <supla-common/proto.h>
#include <supla/control/virtual_relay.h>
#include <supla/parser/json.h>
#include <supla/parser/parser.h>
#include <supla/parser/simple.h>
#include <supla/pv/fronius.h>
#include <supla/sensor/binary_parsed.h>
#include <supla/sensor/electricity_meter_parsed.h>
#include <supla/sensor/impulse_counter_parsed.h>
#include <supla/sensor/thermometer_parsed.h>
#include <supla/source/cmd.h>
#include <supla/source/file.h>
#include <supla/source/source.h>
#include <supla/tools.h>
#include <yaml-cpp/exceptions.h>

#include <chrono>  // NOLINT(build/c++11)
#include <cstring>
#include <filesystem>
#include <fstream>
#include <random>
#include <string>

#include "linux_yaml_config.h"

namespace Supla {
const char Multiplier[] = "multiplier";

const char GuidAuthFileName[] = "/guid_auth.yaml";
const char GuidKey[] = "guid";
const char AuthKeyKey[] = "authkey";
};  // namespace Supla

Supla::LinuxYamlConfig::LinuxYamlConfig(const std::string& file) : file(file) {
}

Supla::LinuxYamlConfig::~LinuxYamlConfig() {
}

bool Supla::LinuxYamlConfig::init() {
  if (config.size() == 0) {
    try {
      config = YAML::LoadFile(file);
      loadGuidAuthFromPath(getStateFilesPath());
    } catch (const YAML::Exception& ex) {
      supla_log(LOG_ERR, "Config file YAML error: %s", ex.what());
      return false;
    }
  }
  return true;
}

bool Supla::LinuxYamlConfig::isDebug() {
  try {
    if (config["log_level"]) {
      auto logLevel = config["log_level"].as<std::string>();
      if (logLevel == "debug") {
        return true;
      }
    }
  } catch (const YAML::Exception& ex) {
    supla_log(LOG_ERR, "Config file YAML error: %s", ex.what());
  }
  return false;
}

bool Supla::LinuxYamlConfig::isVerbose() {
  try {
    if (config["log_level"]) {
      auto logLevel = config["log_level"].as<std::string>();
      if (logLevel == "verbose") {
        return true;
      }
    }
  } catch (const YAML::Exception& ex) {
    supla_log(LOG_ERR, "Config file YAML error: %s", ex.what());
  }
  return false;
}

void Supla::LinuxYamlConfig::removeAll() {
}

bool Supla::LinuxYamlConfig::generateGuidAndAuthkey() {
  char guid[SUPLA_GUID_SIZE] = {};
  char authkey[SUPLA_AUTHKEY_SIZE] = {};

  unsigned int randSeed = static_cast<unsigned int>(
      std::chrono::system_clock::now().time_since_epoch().count());

  std::mt19937 randGen(randSeed);
  std::uniform_int_distribution<unsigned char> distribution(0, 255);

  for (int i = 0; i < SUPLA_GUID_SIZE; i++) {
    guid[i] = static_cast<char>(distribution(randGen));
  }

  for (int i = 0; i < SUPLA_AUTHKEY_SIZE; i++) {
    authkey[i] = distribution(randGen);
  }

  if (isArrayEmpty(guid, SUPLA_GUID_SIZE)) {
    supla_log(LOG_ERR, "Failed to generate GUID");
    return false;
  }
  if (isArrayEmpty(authkey, SUPLA_AUTHKEY_SIZE)) {
    supla_log(LOG_ERR, "Failed to generate AUTHKEY");
    return false;
  }

  setGUID(guid);
  setAuthKey(authkey);

  return saveGuidAuth(getStateFilesPath());
}

// Generic getters and setters
bool Supla::LinuxYamlConfig::setString(const char* key, const char* value) {
  supla_log(LOG_WARNING, "Config setters are not supported (setString)");
  return false;
}
bool Supla::LinuxYamlConfig::getString(const char* key,
                                       char* value,
                                       size_t maxSize) {
  return false;
}
int Supla::LinuxYamlConfig::getStringSize(const char* key) {
  return false;
}

bool Supla::LinuxYamlConfig::setBlob(const char* key,
                                     const char* value,
                                     size_t blobSize) {
  supla_log(LOG_WARNING, "Config setters are not supported (setBlob)");
  return false;
}

bool Supla::LinuxYamlConfig::getBlob(const char* key,
                                     char* value,
                                     size_t blobSize) {
  return false;
}

int Supla::LinuxYamlConfig::getBlobSize(const char* key) {
  return 0;
}

bool Supla::LinuxYamlConfig::getInt8(const char* key, int8_t* result) {
  return false;
}
bool Supla::LinuxYamlConfig::getUInt8(const char* key, uint8_t* result) {
  return false;
}
bool Supla::LinuxYamlConfig::getInt32(const char* key, int32_t* result) {
  return false;
}
bool Supla::LinuxYamlConfig::getUInt32(const char* key, uint32_t* result) {
  return false;
}

bool Supla::LinuxYamlConfig::setInt8(const char* key, const int8_t value) {
  supla_log(LOG_WARNING, "Config setters are not supported (setInt8)");
  return false;
}
bool Supla::LinuxYamlConfig::setUInt8(const char* key, const uint8_t value) {
  supla_log(LOG_WARNING, "Config setters are not supported (setUInt8)");
  return false;
}
bool Supla::LinuxYamlConfig::setInt32(const char* key, const int32_t value) {
  supla_log(LOG_WARNING, "Config setters are not supported (setInt32)");
  return false;
}
bool Supla::LinuxYamlConfig::setUInt32(const char* key, const uint32_t value) {
  supla_log(LOG_WARNING, "Config setters are not supported (setUInt32)");
  return false;
}

void Supla::LinuxYamlConfig::commit() {
}

bool Supla::LinuxYamlConfig::setDeviceName(const char* name) {
  supla_log(LOG_WARNING, "setDeviceName is not supported on this platform");
  return false;
}

bool Supla::LinuxYamlConfig::setSuplaCommProtocolEnabled(bool enabled) {
  supla_log(LOG_WARNING,
            "setSuplaCommProtocolEnabled is not supported on this platform");
  return false;
}
bool Supla::LinuxYamlConfig::setSuplaServer(const char* server) {
  supla_log(LOG_WARNING, "setSuplaServer is not supported on this platform");
  return false;
}
bool Supla::LinuxYamlConfig::setSuplaServerPort(int32_t port) {
  supla_log(LOG_WARNING,
            "setSuplaServerPort is not supported on this platform");
  return false;
}
bool Supla::LinuxYamlConfig::setEmail(const char* email) {
  supla_log(LOG_WARNING, "setEmail is not supported on this platform");
  return false;
}

bool Supla::LinuxYamlConfig::getDeviceName(char* result) {
  try {
    if (config["name"]) {
      auto deviceName = config["name"].as<std::string>();
      strncpy(result, deviceName.c_str(), SUPLA_DEVICE_NAME_MAXSIZE);
      return true;
    }
  } catch (const YAML::Exception& ex) {
    supla_log(LOG_ERR, "Config file YAML error: %s", ex.what());
  }
  return false;
}

bool Supla::LinuxYamlConfig::isSuplaCommProtocolEnabled() {
  return true;
}

bool Supla::LinuxYamlConfig::getSuplaServer(char* result) {
  try {
    if (config["supla"] && config["supla"]["server"]) {
      auto server = config["supla"]["server"].as<std::string>();
      strncpy(result, server.c_str(), SUPLA_SERVER_NAME_MAXSIZE);
      return true;
    }
  } catch (const YAML::Exception& ex) {
    supla_log(LOG_ERR, "Config file YAML error: %s", ex.what());
  }
  return false;
}

int32_t Supla::LinuxYamlConfig::getSuplaServerPort() {
  try {
    if (config["port"]) {
      auto port = config["port"].as<int>();
      return port;
    }
  } catch (const YAML::Exception& ex) {
    supla_log(LOG_ERR, "Config file YAML error: %s", ex.what());
  }
  return 2016;
}

bool Supla::LinuxYamlConfig::getEmail(char* result) {
  try {
    if (config["supla"] && config["supla"]["mail"]) {
      auto mail = config["supla"]["mail"].as<std::string>();
      strncpy(result, mail.c_str(), SUPLA_EMAIL_MAXSIZE);
      return true;
    }
  } catch (const YAML::Exception& ex) {
    supla_log(LOG_ERR, "Config file YAML error: %s", ex.what());
  }
  return false;
}

bool Supla::LinuxYamlConfig::setGUID(const char* guidRaw) {
  char guidHex[SUPLA_GUID_HEXSIZE] = {};
  generateHexString(guidRaw, guidHex, SUPLA_GUID_SIZE);
  guid = std::string(guidHex);
  return true;
}

bool Supla::LinuxYamlConfig::getGUID(char* result) {
  if (guid.length()) {
    hexStringToArray(guid.c_str(), result, SUPLA_GUID_SIZE);
    return true;
  }
  return false;
}

bool Supla::LinuxYamlConfig::setAuthKey(const char* authkeyRaw) {
  char authkeyHex[SUPLA_AUTHKEY_HEXSIZE] = {};
  generateHexString(authkeyRaw, authkeyHex, SUPLA_AUTHKEY_SIZE);
  authkey = std::string(authkeyHex);
  return true;
}

bool Supla::LinuxYamlConfig::getAuthKey(char* result) {
  if (authkey.length()) {
    hexStringToArray(authkey.c_str(), result, SUPLA_AUTHKEY_SIZE);
    return true;
  }
  return false;
}

bool Supla::LinuxYamlConfig::loadChannels() {
  try {
    if (config["channels"]) {
      auto channels = config["channels"];
      int channelCount = 0;
      for (auto it : channels) {
        if (!parseChannel(it, channelCount)) {
          supla_log(LOG_ERR, "Config: parsing channel %d failed", channelCount);
          return false;
        }
        channelCount++;
      }
      if (channelCount == 0) {
        supla_log(LOG_ERR, "Config: \"channels\" section missing in file");
        return false;
      }
      return true;
    } else {
      supla_log(LOG_ERR, "Config: \"channels\" section missing in file");
      return false;
    }
  } catch (const YAML::Exception& ex) {
    supla_log(LOG_ERR, "Config file YAML error: %s", ex.what());
  }
  return false;
}

bool Supla::LinuxYamlConfig::parseChannel(const YAML::Node& ch,
                                          int channelNumber) {
  paramCount = 0;
  if (ch["type"]) {
    paramCount++;
    std::string type = ch["type"].as<std::string>();

    Supla::Source::Source* source = nullptr;
    Supla::Parser::Parser* parser = nullptr;

    if (ch["source"]) {
      paramCount++;
      if (!(source = addSource(ch["source"]))) {
        supla_log(LOG_ERR, "Adding source failed");
        return false;
      }
    }

    if (ch["parser"]) {
      paramCount++;
      if (!(parser = addParser(ch["parser"], source))) {
        supla_log(LOG_ERR, "Adding parser failed");
        return false;
      }
      parserCount++;
    }

    if (ch["name"]) {  // optional
      paramCount++;
      std::string name = ch["name"].as<std::string>();
      channelNames[name] = channelNumber;
    }

    if (type == "VirtualRelay") {
      return addVirtualRelay(ch, channelNumber);
    }
    if (type == "Fronius") {
      return addFronius(ch, channelNumber);
    }
    if (type == "ThermometerParsed") {
      if (!parser) {
        supla_log(LOG_ERR, "Channel[%d] config: missing parser", channelNumber);
        return false;
      }
      return addThermometerParsed(ch, channelNumber, parser);
    }
    if (type == "ImpulseCounterParsed") {
      if (!parser) {
        supla_log(LOG_ERR, "Channel[%d] config: missing parser", channelNumber);
        return false;
      }
      return addImpulseCounterParsed(ch, channelNumber, parser);
    }
    if (type == "ElectricityMeterParsed") {
      if (!parser) {
        supla_log(LOG_ERR, "Channel[%d] config: missing parser", channelNumber);
        return false;
      }
      return addElectricityMeterParsed(ch, channelNumber, parser);
    }
    if (type == "BinaryParsed") {
      if (!parser) {
        supla_log(LOG_ERR, "Channel[%d] config: missing parser", channelNumber);
        return false;
      }
      return addBinaryParsed(ch, channelNumber, parser);
    } else {
      supla_log(LOG_ERR,
                "Channel[%d] config: unknown type \"%s\"",
                channelNumber,
                type.c_str());
      return false;
    }

    if (ch.size() > paramCount) {
      supla_log(LOG_WARNING,
                "Channel[%d] config: too many parameters",
                channelNumber);
    }
    return true;

  } else {
    supla_log(LOG_ERR,
              "Channel[%d] config: missing mandatory \"type\" parameter",
              channelNumber);
  }
  return false;
}

bool Supla::LinuxYamlConfig::addVirtualRelay(const YAML::Node& ch,
                                             int channelNumber) {
  supla_log(LOG_INFO, "Channel[%d] config: adding VirtualRelay", channelNumber);
  new Supla::Control::VirtualRelay();
  return true;
}

bool Supla::LinuxYamlConfig::addFronius(const YAML::Node& ch,
                                        int channelNumber) {
  int port = 80;
  int deviceId = 1;
  if (ch["port"]) {
    paramCount++;
    port = ch["port"].as<int>();
  }
  if (ch["device_id"]) {
    paramCount++;
    deviceId = ch["device_id"].as<int>();
  }

  if (ch["ip"]) {  // mandatory
    paramCount++;
    std::string ip = ch["ip"].as<std::string>();
    supla_log(
        LOG_INFO,
        "Channel[%d] config: adding Fronius with IP %s, port: %d, deviceId: %d",
        channelNumber,
        ip.c_str(),
        port,
        deviceId);

    IPAddress ipAddr(ip);
    new Supla::PV::Fronius(ipAddr, port, deviceId);
  } else {
    return false;
  }

  return true;
}

bool Supla::LinuxYamlConfig::addThermometerParsed(
    const YAML::Node& ch, int channelNumber, Supla::Parser::Parser* parser) {
  supla_log(
      LOG_INFO, "Channel[%d] config: adding ThremometerParsed", channelNumber);
  auto therm = new Supla::Sensor::ThermometerParsed(parser);
  if (ch[Supla::Parser::Temperature]) {
    paramCount++;
    if (parser->isBasedOnIndex()) {
      int index = ch[Supla::Parser::Temperature].as<int>();
      therm->setMapping(Supla::Parser::Temperature, index);
    } else {
      std::string key = ch[Supla::Parser::Temperature].as<std::string>();
      therm->setMapping(Supla::Parser::Temperature, key);
    }
  } else {
    supla_log(LOG_ERR,
              "Channel[%d] config: missing \"%s\" parameter",
              channelNumber,
              Supla::Parser::Temperature);
    return false;
  }
  if (ch[Supla::Multiplier]) {
    paramCount++;
    double multiplier = ch[Supla::Multiplier].as<double>();
    therm->setMultiplier(Supla::Parser::Temperature, multiplier);
  }

  return true;
}

bool Supla::LinuxYamlConfig::addImpulseCounterParsed(
    const YAML::Node& ch, int channelNumber, Supla::Parser::Parser* parser) {
  supla_log(LOG_INFO,
            "Channel[%d] config: adding ImpulseCounterParsed",
            channelNumber);
  auto ic = new Supla::Sensor::ImpulseCounterParsed(parser);
  if (ch[Supla::Parser::Counter]) {
    paramCount++;
    if (parser->isBasedOnIndex()) {
      int index = ch[Supla::Parser::Counter].as<int>();
      ic->setMapping(Supla::Parser::Counter, index);
    } else {
      std::string key = ch[Supla::Parser::Counter].as<std::string>();
      ic->setMapping(Supla::Parser::Counter, key);
    }
  } else {
    supla_log(LOG_ERR,
              "Channel[%d] config: missing \"%s\" parameter",
              channelNumber,
              Supla::Parser::Counter);
    return false;
  }
  if (ch[Supla::Multiplier]) {
    paramCount++;
    double multiplier = ch[Supla::Multiplier].as<double>();
    ic->setMultiplier(Supla::Parser::Counter, multiplier);
  }

  return true;
}

bool Supla::LinuxYamlConfig::addElectricityMeterParsed(
    const YAML::Node& ch, int channelNumber, Supla::Parser::Parser* parser) {
  supla_log(LOG_INFO,
            "Channel[%d] config: adding ElectricityMeterParsed",
            channelNumber);
  auto em = new Supla::Sensor::ElectricityMeterParsed(parser);

  // set not phase releated parameters (currently only frequency)
  if (ch[Supla::Parser::Frequency]) {
    paramCount++;
    if (parser->isBasedOnIndex()) {
      int index = ch[Supla::Parser::Frequency].as<int>();
      em->setMapping(Supla::Parser::Frequency, index);
    } else {
      std::string key = ch[Supla::Parser::Frequency].as<std::string>();
      em->setMapping(Supla::Parser::Frequency, key);
    }
    if (ch[Supla::Multiplier]) {
      double multiplier = ch[Supla::Multiplier].as<double>();
      em->setMultiplier(Supla::Parser::Frequency, multiplier);
    }
  }

  const std::map<std::string, int> phases = {
      {"phase_1", 1}, {"phase_2", 2}, {"phase_3", 3}};

  for (auto i : phases) {
    if (ch[i.first]) {
      paramCount++;
      auto phaseParameters = ch[i.first];
      int phaseId = i.second;

      for (auto param : phaseParameters) {
        std::string paramName;
        for (const std::string& name : {"voltage",
                                        "current",
                                        "fwd_act_energy",
                                        "rvr_act_energy",
                                        "fwd_react_energy",
                                        "rvr_react_energy",
                                        "power_active",
                                        "rvr_power_active",
                                        "power_reactive",
                                        "power_apparent",
                                        "phase_angle",
                                        "power_factor"}) {
          if (param[name]) {
            paramName = name + "_" + std::to_string(phaseId);
            if (parser->isBasedOnIndex()) {
              int index = param[name].as<int>();
              em->setMapping(paramName, index);
            } else {
              std::string key = param[name].as<std::string>();
              em->setMapping(paramName, key);
            }
            if (param[Supla::Multiplier]) {
              double multiplier = param[Supla::Multiplier].as<double>();
              em->setMultiplier(paramName, multiplier);
            }
          }
        }
      }
    }
  }

  return true;
}

bool Supla::LinuxYamlConfig::addBinaryParsed(const YAML::Node& ch,
                                             int channelNumber,
                                             Supla::Parser::Parser* parser) {
  supla_log(LOG_INFO, "Channel[%d] config: adding BinaryParsed", channelNumber);
  auto binary = new Supla::Sensor::BinaryParsed(parser);
  if (ch[Supla::Parser::State]) {
    paramCount++;
    if (parser->isBasedOnIndex()) {
      int index = ch[Supla::Parser::State].as<int>();
      binary->setMapping(Supla::Parser::State, index);
    } else {
      std::string key = ch[Supla::Parser::State].as<std::string>();
      binary->setMapping(Supla::Parser::State, key);
    }
  } else {
    supla_log(LOG_ERR,
              "Channel[%d] config: missing \"%s\" parameter",
              channelNumber,
              Supla::Parser::State);
    return false;
  }

  return true;
}

Supla::Parser::Parser* Supla::LinuxYamlConfig::addParser(
    const YAML::Node& parser, Supla::Source::Source* src) {
  Supla::Parser::Parser* prs = nullptr;
  if (parser["use"]) {
    std::string use = parser["use"].as<std::string>();
    if (parserNames.count(use)) {
      prs = parsers[parserNames[use]];
    }
    if (!prs) {
      supla_log(LOG_ERR,
                "Config: can't find parser with \"name\"=\"%s\"",
                use.c_str());
      return nullptr;
    }
    if (parser["name"]) {
      supla_log(LOG_ERR,
                "Config: can't use \"name\" for parser with \"use\" parameter");
      return nullptr;
    }
    return prs;
  }

  if (parser["name"]) {
    std::string name = parser["name"].as<std::string>();
    parserNames[name] = parserCount;
  }

  if (!src) {
    supla_log(LOG_ERR, "Config: parser used without source");
    return nullptr;
  }

  if (parser["type"]) {
    std::string type = parser["type"].as<std::string>();
    if (type == "Simple") {
      prs = new Supla::Parser::Simple(src);
    } else if (type == "Json") {
      prs = new Supla::Parser::Json(src);
    } else {
      supla_log(LOG_ERR, "Config: unknown parser type \"%s\"", type.c_str());
      return nullptr;
    }
    if (parser["refresh_time_ms"]) {
      int timeMs = parser["refresh_time_ms"].as<int>();
      prs->setRefreshTime(timeMs);
    }
  } else {
    supla_log(LOG_ERR, "Config: type not defined for parser");
    return nullptr;
  }

  parsers[parserCount] = prs;
  parserCount++;
  return prs;
}

Supla::Source::Source* Supla::LinuxYamlConfig::addSource(
    const YAML::Node& source) {
  Supla::Source::Source* src = nullptr;
  if (source["use"]) {
    std::string use = source["use"].as<std::string>();
    if (sourceNames.count(use)) {
      src = sources[sourceNames[use]];
    }
    if (!src) {
      supla_log(LOG_ERR,
                "Config: can't find source with \"name\"=\"%s\"",
                use.c_str());
      return nullptr;
    }
    if (source["name"]) {
      supla_log(LOG_ERR,
                "Config: can't use \"name\" for source with \"use\" parameter");
      return nullptr;
    }
    return src;
  }

  if (source["name"]) {
    std::string name = source["name"].as<std::string>();
    sourceNames[name] = sourceCount;
  }

  if (source["type"]) {
    std::string type = source["type"].as<std::string>();
    if (type == "File") {
      std::string fileName = source["file"].as<std::string>();
      int expirationTimeSec = 10 * 60;
      if (source["expiration_time_sec"]) {
        expirationTimeSec = source["expiration_time_sec"].as<int>();
      }
      src = new Supla::Source::File(fileName.c_str(), expirationTimeSec);
    } else if (type == "Cmd") {
      std::string cmd = source["command"].as<std::string>();
      src = new Supla::Source::Cmd(cmd.c_str());
    } else {
      supla_log(LOG_ERR, "Config: unknown source type \"%s\"", type.c_str());
      return nullptr;
    }

  } else {
    supla_log(LOG_ERR, "Config: type not defined for source");
    return nullptr;
  }

  sources[sourceCount] = src;
  sourceCount++;

  return src;
}

std::string Supla::LinuxYamlConfig::getStateFilesPath() {
  std::string path;
  try {
    if (config["state_files_path"]) {
      path = config["state_files_path"].as<std::string>();
    }
  } catch (const YAML::Exception& ex) {
    supla_log(LOG_ERR, "Config file YAML error: %s", ex.what());
  }
  if (path.empty()) {
    supla_log(LOG_DEBUG, "Config: missing state files path - using default");
    path = "var/lib/supla-device";
  }
  return path;
}

void Supla::LinuxYamlConfig::loadGuidAuthFromPath(const std::string& path) {
  try {
    std::string file = path + Supla::GuidAuthFileName;
    supla_log(LOG_INFO, "GUID and AUTHKEY: loading from file %s", file.c_str());
    auto guidAuthYaml = YAML::LoadFile(file);
    std::string guidHex;
    std::string authHex;
    if (guidAuthYaml[Supla::GuidKey]) {
      guidHex = guidAuthYaml[Supla::GuidKey].as<std::string>();
      if (guidHex.length() != SUPLA_GUID_HEXSIZE - 1) {
        supla_log(LOG_WARNING, "GUID: invalid guid value length");
        guidHex = "";
      }
      guid = guidHex;
    } else {
      supla_log(LOG_WARNING, "GUID: missing guid key in yaml file");
    }
    if (guidAuthYaml[Supla::AuthKeyKey]) {
      authHex = guidAuthYaml[Supla::AuthKeyKey].as<std::string>();
      if (authHex.length() != SUPLA_AUTHKEY_HEXSIZE - 1) {
        supla_log(LOG_WARNING, "AUTHKEY: invalid authkey value length");
        authHex = "";
      }
      authkey = authHex;
    } else {
      supla_log(LOG_WARNING, "AUTHKEY: missing authkey key in yaml file");
    }
  } catch (const YAML::Exception& ex) {
    supla_log(LOG_ERR, "Guid/auth file YAML error: %s", ex.what());
  }
}

bool Supla::LinuxYamlConfig::saveGuidAuth(const std::string& path) {
  if (!std::filesystem::exists(path)) {
    std::error_code err;
    if (!std::filesystem::create_directories(path, err)) {
      supla_log(LOG_WARNING, "Config: failed to create folder for state files");
      return false;
    }
  }
  YAML::Node outputYaml;
  outputYaml[Supla::GuidKey] = guid;
  outputYaml[Supla::AuthKeyKey] = authkey;

  std::ofstream out(path + Supla::GuidAuthFileName);
  out << outputYaml;
  out.close();
  if (out.fail()) {
    supla_log(LOG_ERR, "Config: failed to write guid/authkey to file");
    return false;
  }

  return true;
}
