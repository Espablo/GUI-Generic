#ifdef ARDUINO_ARCH_ESP8266
#include <Arduino.h>
#include <ESP8266httpUpdate.h>

#include "SuplaHTTPUpdateServer.h"
#include "UpdateURL.h"

#include "../../SuplaDeviceGUI.h"
#include "SuplaUpdateCommon.h"

ESP8266HTTPUpdateServer::ESP8266HTTPUpdateServer(bool serial_debug) {
  _serial_output = serial_debug;
}

void ESP8266HTTPUpdateServer::setup() {
  WebServer->httpServer->on(getURL(PATH_UPDATE_HENDLE), std::bind(&ESP8266HTTPUpdateServer::handleFirmwareUp, this));

  WebServer->httpServer->on(getURL(PATH_UPDATE), HTTP_GET, [&]() {
    if (!WebServer->isLoggedIn())
      return;

    String index = FPSTR(serverIndex);
    index.replace("{l}", S_LANG);
    index.replace("{f}", String(ESP.getFlashChipRealSize() / 1024));
    index.replace("{m}", String(ESP.getFlashChipSize() / 1024));
    index.replace("{s}", String(ESP.getFreeSketchSpace() / 1024));
    index.replace("{u}", String(ESP.getSketchSize() / 1024));
    index.replace("{F}", S_FLASH_MEMORY_SIZE);
    index.replace("{M}", S_SKETCH_MEMORY_SIZE);
    index.replace("{S}", S_SKETCH_UPLOAD_MAX_SIZE);
    index.replace("{U}", S_SKETCH_LOADED_SIZE);
    index.replace("{b}", S_UPDATE_FIRMWARE);
    if ((ESP.getFlashChipSize() / 1024) == 1024) {
      index.replace("{g}", twoStepButton);
      index.replace("{gu}", PATH_UPDATE_HENDLE_2STEP);
    }
    else {
      index.replace("{g}", "");
    }
    index.replace("{gg}", "GUI-GenericUpdater.bin");
    WebServer->httpServer->send(200, PSTR("text/html"), index.c_str());
  });

  WebServer->httpServer->on(getURL(PATH_UPDATE_HENDLE), std::bind(&ESP8266HTTPUpdateServer::handleFirmwareUp, this));

  // handler for the /update form POST (once file upload finishes)
  WebServer->httpServer->on(
      getURL(PATH_UPDATE), HTTP_POST,
      [&]() {
        if (!WebServer->isLoggedIn())
          return;

        successUpdateManualRefresh();
      },
      [&]() { updateManual(); });
}

void ESP8266HTTPUpdateServer::handleFirmwareUp() {
  if (!WebServer->isLoggedIn())
    return;

  String sCommand = WebServer->httpServer->arg(ARG_PARM_URL);

  if (!sCommand.isEmpty()) {
    UpdateURL* update = nullptr;

    if (strcasecmp_P(sCommand.c_str(), PATH_UPDATE_URL) == 0) {
      if (strcmp(WebServer->httpServer->arg(INPUT_UPDATE_URL).c_str(), "") != 0) {
        update = new UpdateURL(WebServer->httpServer->arg(INPUT_UPDATE_URL).c_str());
      }
    }

    if (strcasecmp_P(sCommand.c_str(), PATH_UPDATE_HENDLE_2STEP) == 0) {
      update = new UpdateURL("http://gui-generic-builder.supla.io/files/GUI-GenericUploader.bin.gz");
    }

    if (update) {
      switch (update->update()) {
        case HTTP_UPDATE_FAILED:
          Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          suplaWebPageUpddate(SaveResult::UPDATE_ERROR, PATH_UPDATE_HENDLE);
          break;

        case HTTP_UPDATE_NO_UPDATES:
          Serial.println("HTTP_UPDATE_NO_UPDATES");
          break;

        case HTTP_UPDATE_OK:
          Serial.println("HTTP_UPDATE_OK");

          if (strcasecmp_P(sCommand.c_str(), PATH_UPDATE_HENDLE_2STEP) == 0) {
            successUpdateManualRefresh();
          }
          else {
            suplaWebPageUpddate(SaveResult::UPDATE_SUCCESS, PATH_START);
            ConfigESP->rebootESP();
          }
          break;
      }
    }
  }

  suplaWebPageUpddate();
}

void ESP8266HTTPUpdateServer::suplaWebPageUpddate(int save, const String& location) {
  WebServer->sendHeaderStart();

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(location);

  addFormHeader(webContentBuffer, S_UPDATE_FIRMWARE);
  webContentBuffer += F("<iframe src='");
  webContentBuffer += getURL(PATH_UPDATE);
  webContentBuffer += F("' frameborder='0' height='200'></iframe>");
  addFormHeaderEnd(webContentBuffer);

  addForm(webContentBuffer, F("post"), getParameterRequest(PATH_UPDATE_HENDLE, ARG_PARM_URL, PATH_UPDATE_URL));
  addFormHeader(webContentBuffer, S_EMPTY);
  addTextBox(webContentBuffer, INPUT_UPDATE_URL, F("URL"), F(""), 0, 600, false);
  addButtonSubmit(webContentBuffer, S_UPDATE_FIRMWARE);
  addFormEnd(webContentBuffer);

  addFormHeaderEnd(webContentBuffer);

  addButton(webContentBuffer, S_RETURN, PATH_TOOLS);
  WebServer->sendHeaderEnd();
}

void ESP8266HTTPUpdateServer::setUpdaterError() {
  if (_serial_output)
    Update.printError(Serial);
  StreamString str;
  Update.printError(str);
  Serial.println(str.c_str());
  _updaterError = str.c_str();
}

void ESP8266HTTPUpdateServer::successUpdateManualRefresh() {
  String succes = FPSTR(successResponse);
  succes.replace("{m}", S_UPDATE_SUCCESS_REBOOTING);
  WebServer->httpServer->client().setNoDelay(true);
  WebServer->httpServer->send(200, F("text/html"), succes.c_str());
  delay(100);
  WebServer->httpServer->client().stop();

  ESP.restart();
}

void ESP8266HTTPUpdateServer::updateManual() {
  if (!WebServer->isLoggedIn()) {
    return;
  }
  // handler for the file upload, get's the sketch bytes, and writes
  // them through the Update object
  HTTPUpload& upload = WebServer->httpServer->upload();

  if (upload.status == UPLOAD_FILE_START) {
    _updaterError = String();
    if (_serial_output)
      Serial.setDebugOutput(true);

    WiFiUDP::stopAll();
    if (_serial_output)
      Serial.printf("Update: %s\n", upload.filename.c_str());
    if (upload.name == "filesystem") {
      size_t fsSize = ((size_t)&_FS_end - (size_t)&_FS_start);
      close_all_fs();
      if (!Update.begin(fsSize, U_FS, ConfigESP->getGpio(FUNCTION_CFG_LED),
                        ConfigESP->getLevel(ConfigESP->getGpio(FUNCTION_CFG_LED)))) {  // start with max available size
        if (_serial_output)
          Update.printError(Serial);
      }
    }
    else {
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if (!Update.begin(maxSketchSpace, U_FLASH, ConfigESP->getGpio(FUNCTION_CFG_LED),
                        ConfigESP->getLevel(ConfigESP->getGpio(FUNCTION_CFG_LED)))) {  // start with max available size
        setUpdaterError();
      }
    }
  }
  else if (upload.status == UPLOAD_FILE_WRITE && !_updaterError.length()) {
    if (_serial_output)
      Serial.printf(".");
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      String twoStep = FPSTR(twoStepResponse);
      twoStep.replace("{w}", S_WARNING);
      twoStep.replace("{o}", S_ONLY_2_STEP_OTA);
      WebServer->httpServer->send(200, F("text/html"), twoStep.c_str());

      setUpdaterError();
    }
  }
  else if (upload.status == UPLOAD_FILE_END && !_updaterError.length()) {
    if (Update.end(true)) {  // true to set the size to the current progress
      if (_serial_output)
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
    }
    else {
      setUpdaterError();
    }
    if (_serial_output)
      Serial.setDebugOutput(false);
  }
  else if (upload.status == UPLOAD_FILE_ABORTED) {
    Update.end();
    if (_serial_output)
      Serial.println(F("Update was aborted"));
  }
  delay(0);
}

#endif