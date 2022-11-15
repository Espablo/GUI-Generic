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

#include <stdio.h>
#include <string.h>
#include <supla/network/web_sender.h>
#include <supla/storage/storage.h>
#include <supla/tools.h>

#include "protocol_parameters.h"
#include "supla/network/network.h"

namespace Supla {

namespace Html {

ProtocolParameters::ProtocolParameters(bool addMqttParams,
                                       bool concurrentProtocols)
    : HtmlElement(HTML_SECTION_PROTOCOL), addMqtt(addMqttParams),
      concurrent(concurrentProtocols) {
}

ProtocolParameters::~ProtocolParameters() {
}

void ProtocolParameters::send(Supla::WebSender* sender) {
  auto cfg = Supla::Storage::ConfigInstance();
  if (cfg) {
    if (!concurrent) {
      // Protocol selector
      sender->send(
          "<div class=\"w\">"
          "<i><select name=\"pro\" onchange=\"protocolChanged();\" "
          "id=\"protocol\">"
          "<option value=\"0\"");
      sender->send(selected(cfg->isSuplaCommProtocolEnabled()));

      sender->send(
          ">Supla</option>"
          "<option value=\"1\"");
      sender->send(selected(cfg->isMqttCommProtocolEnabled()));
      sender->send(
          ">MQTT</option>"
          "</select>"
          "<label>Protocol</label>"
          "</i>"
          "</div>");
    }

    if (concurrent) {
      sender->send(
          "<div class=\"w\">");
      sender->send(
          "<h3>Supla Settings</h3>");
      sender->send(
          "<i><select name=\"protocol_supla\" id=\"protocol_supla\" "
          "onchange=\"protocolChanged();\">"
          "<option value=\"0\"");
      sender->send(selected(!cfg->isSuplaCommProtocolEnabled()));
      sender->send(
          ">DISABLED</option>"
          "<option value=\"1\"");
      sender->send(selected(cfg->isSuplaCommProtocolEnabled()));
      sender->send(
          ">ENABLED</option>"
          "</select>"
          "<label>Supla protocol</label>");
      sender->send(
          "<div id=\"proto_supla\">");
    } else {
      sender->send(
          "<div class=\"w\" id=\"proto_supla\">");
      sender->send(
          "<h3>Supla Settings</h3>");
    }
    // Parameters for Supla protocol
    sender->send(
        "<i><input name=\"svr\" maxlength=\"64\" value=\"");
    char buf[512];
    if (cfg->getSuplaServer(buf)) {
      sender->send(buf);
    }
    sender->send(
        "\"><label>Server</label></i>"
        "<i><input name=\"eml\" maxlength=\"255\" value=\"");
    if (cfg->getEmail(buf)) {
      sender->send(buf);
    }
    sender->send(
        "\"><label>E-mail</label></i>");

    sender->send(
        "<script>"
        "function securityChange(){"
        "var e=document.getElementById(\"sec\"),"
        "c=document.getElementById(\"custom_ca\"),"
        "l=\"1\"==e.value?\"block\":\"none\";"
        "c.style.display=l;}"
        "</script>");

    uint8_t securityLevel = 0;
    cfg->getUInt8("security_level", &securityLevel);
    sender->send(
        "<i><select name=\"sec\" id=\"sec\" onchange=\"securityChange();\">"
        "<option value=\"0\"");
    sender->send(selected(securityLevel == 0));
    sender->send(
        ">Supla CA</option>"
        "<option value=\"1\"");
    sender->send(selected(securityLevel == 1));
    sender->send(
        ">Custom CA</option>"
        "<option value=\"2\"");
    sender->send(selected(securityLevel == 2));
    sender->send(">Skip certificate verification (INSECURE)</option>");

    sender->send(
        "</select>"
        "<label>Certificate verification</label>"
        "</i>");
    sender->send("<i id=\"custom_ca\" style=\"display:");
    if (securityLevel == 1) {
      sender->send("block");
    } else {
      sender->send("none");
    }
    sender->send(
        "\">"
        "<textarea name=\"custom_ca\" maxlength=\"4000\">");
    char* bufCert = new char[4000];
    memset(bufCert, 0, 4000);
    if (cfg->getCustomCA(bufCert, 4000)) {
      sender->send(bufCert);
    }
    delete[] bufCert;
    sender->send(
        "</textarea><label>Custom CA (paste here CA certificate in PEM "
        "format)</label>"
        "</i>"
        "</div>");

    if (concurrent) {
      sender->send("</div>");
    }

    if (addMqtt) {
      if (concurrent) {
        sender->send(
            "<div class=\"w\">");
        sender->send(
            "<h3>MQTT Settings</h3>");
        sender->send(
            "<i><select name=\"protocol_mqtt\" id=\"protocol_mqtt\" "
            "onchange=\"protocolChanged();\">"
            "<option value=\"0\"");
        sender->send(selected(!cfg->isMqttCommProtocolEnabled()));
        sender->send(
            ">DISABLED</option>"
            "<option value=\"1\"");
        sender->send(selected(cfg->isMqttCommProtocolEnabled()));
        sender->send(
            ">ENABLED</option>"
            "</select>"
            "<label>MQTT protocol</label>");
        sender->send(
            "<div class=\"mqtt\">");
      } else {
        sender->send(
            "<div class=\"w\" class=\"mqtt\">");
        sender->send(
            "<h3>MQTT Settings</h3>");
      }
      // Parameters for MQTT protocol
      sender->send(
          "<i><input name=\"mqttserver\" maxlength=\"64\" value=\"");
      if (cfg->getMqttServer(buf)) {
        sender->send(buf);
      }
      sender->send(
          "\"><label>Server</label></i>");
      sender->send(
          "<script>"
            "function mqttTlsChange(){"
              "var port=document.getElementById(\"mqtt_port\"),"
              "value=document.getElementById(\"mqtt_tls\");"
              "if(mqtt_tls.value==\"0\")"
              "{port.value=1883;}else"
              "{port.value=8883;}"
            "}"
          "</script>"
          "<i><select name=\"mqtttls\" onchange=\"mqttTlsChange();\""
          " id=\"mqtt_tls\">"
          "<option value=\"0\" ");
      sender->send(selected(!cfg->isMqttTlsEnabled()));
      sender->send(
          ">NO</option>"
          "<option value=\"1\" ");
      sender->send(selected(cfg->isMqttTlsEnabled()));
      sender->send(
          ">YES</option></select>"
          "<label>TLS</label></i>"
          "<i><input name=\"mqttport\" min=\"1\" max=\"65535\" type=\"number\""
          " value=\"");
      sender->send(cfg->getMqttServerPort());
      sender->send(
          "\" id=\"mqtt_port\"><label>Port</label></i>"
          "<i>"
          "<select name=\"mqttauth\" id=\"sel_mauth\" "
          "onchange=\"mAuthChanged();\">"
          "<option value=\"0\" ");
      sender->send(selected(!cfg->isMqttAuthEnabled()));
      sender->send(
          ">NO</option>"
          "<option value=\"1\" ");
      sender->send(selected(cfg->isMqttAuthEnabled()));
      sender->send(
          ">YES</option></select>"
          "<label>Auth</label>"
          "</i>"
          "<i id=\"mauth_usr\"><input name=\"mqttuser\" maxlength=\"64\" "
          "value=\"");
      if (cfg->getMqttUser(buf)) {
        sender->send(buf);
      }
      sender->send(
          "\">"
          "<label>Username</label></i>"
          "<i id=\"mauth_pwd\">"
          "<input name=\"mqttpasswd\" maxlength=\"64\">"
          "<label>Password (Required. Max 64)</label></i>"
          "<i><input name=\"mqttprefix\" maxlength=\"48\" value=\"");
      if (cfg->getMqttPrefix(buf)) {
        sender->send(buf);
      }
      sender->send(
          "\">"
          "<label>Topic prefix</label></i>"
          "<i><input name=\"mqttqos\" min=\"0\" max=\"2\" type=\"number\" "
          "value=\"");
      sender->send(cfg->getMqttQos());
      sender->send(
          "\"><label>QoS</label></i>"
          "<i><select name=\"mqttretain\">"
          "<option value=\"0\" ");
      sender->send(selected(!cfg->isMqttRetainEnabled()));
      sender->send(
          ">NO</option>"
          "<option value=\"1\" ");
      sender->send(selected(cfg->isMqttRetainEnabled()));
      sender->send(
          ">YES</option></select>"
          "<label>Retain</label></i>"
          "</div>");
      if (concurrent) {
        sender->send("</div>");
      }
    }
  }
}

bool ProtocolParameters::handleResponse(const char* key, const char* value) {
  auto cfg = Supla::Storage::ConfigInstance();
  if (strcmp(key, "pro") == 0) {
    int protocol = stringToUInt(value);
    switch (protocol) {
      default:
      case 0: {
        cfg->setSuplaCommProtocolEnabled(true);
        cfg->setMqttCommProtocolEnabled(false);
        break;
      }
      case 1: {
        cfg->setSuplaCommProtocolEnabled(false);
        cfg->setMqttCommProtocolEnabled(true);
        break;
      }
    }
    return true;
  } else if (strcmp(key, "protocol_supla") == 0) {
    int enabled = stringToUInt(value);
    cfg->setSuplaCommProtocolEnabled(enabled == 1);
    return true;
  } else if (strcmp(key, "protocol_mqtt") == 0) {
    int enabled = stringToUInt(value);
    cfg->setMqttCommProtocolEnabled(enabled == 1);
    return true;
  } else if (strcmp(key, "svr") == 0) {
    cfg->setSuplaServer(value);
    return true;
  } else if (strcmp(key, "eml") == 0) {
    cfg->setEmail(value);
    return true;
  } else if (strcmp(key, "sec") == 0) {
    uint8_t securityLevel = stringToUInt(value);
    if (securityLevel > 2) {
      securityLevel = 0;
    }
    cfg->setUInt8("security_level", securityLevel);
    return true;
  } else if (strcmp(key, "custom_ca") == 0) {
    cfg->setCustomCA(value);
    return true;
  } else if (strcmp(key, "mqttserver") == 0) {
    cfg->setMqttServer(value);
    return true;
  } else if (strcmp(key, "mqttport") == 0) {
    int port = stringToUInt(value);
    cfg->setMqttServerPort(port);
    return true;
  } else if (strcmp(key, "mqtttls") == 0) {
    int enabled = stringToUInt(value);
    cfg->setMqttTlsEnabled(enabled == 1);
    return true;
  } else if (strcmp(key, "mqttauth") == 0) {
    int enabled = stringToUInt(value);
    cfg->setMqttAuthEnabled(enabled == 1);
    return true;
  } else if (strcmp(key, "mqttuser") == 0) {
    cfg->setMqttUser(value);
    return true;
  } else if (strcmp(key, "mqttpasswd") == 0) {
    if (strlen(value) > 0) {
      cfg->setMqttPassword(value);
    }
    return true;
  } else if (strcmp(key, "mqttprefix") == 0) {
    cfg->setMqttPrefix(value);
    return true;
  } else if (strcmp(key, "mqttqos") == 0) {
    int qos = stringToUInt(value);
    cfg->setMqttQos(qos);
    return true;
  } else if (strcmp(key, "mqttretain") == 0) {
    int enabled = stringToUInt(value);
    cfg->setMqttRetainEnabled(enabled == 1);
    return true;
  }

  return false;
}

};  // namespace Html
};  // namespace Supla
