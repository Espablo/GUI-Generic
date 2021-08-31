#include "SuplaWebPageUpload.h"

#ifdef ARDUINO_ARCH_ESP8266
#include "FS.h"
#elif ARDUINO_ARCH_ESP32
#include "SPIFFS.h"
#endif

static const char uploadIndex[] PROGMEM =
    R"(<form class="formcenter" method="POST" action="/upload" enctype="multipart/form-data">
         <input type="file" accept=".dat" name="config"></br>
         <input type="checkbox" name='generateGUIDandAUTHKEY' value='1'>{g}</br></br>
         <input type="submit" value="{u}">
     </form>)";

File dataFile;

void createWebUpload() {
  WebServer->httpServer->on(getURL(PATH_UPLOAD), HTTP_GET, []() { handleUpload(); });

  WebServer->httpServer->on(
      getURL(PATH_UPLOAD), HTTP_POST,
      []() {
        if (WebServer->httpServer->hasArg("generateGUIDandAUTHKEY")) {
          if (WebServer->httpServer->arg("generateGUIDandAUTHKEY") == "1") {
            ConfigManager->setGUIDandAUTHKEY();
            ConfigManager->save();
          }
        }
        handleUpload(1);
      },
      handleFileUpload);
}

void handleUpload(int save) {
  if (!WebServer->isLoggedIn()) {
    return;
  }

  String upload = FPSTR(uploadIndex);
  upload.replace("{g}", S_GENERATE_GUID_AND_KEY);
  upload.replace("{u}", S_UPLOAD);

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_UPLOAD);
  webContentBuffer += F("<div class='w'>");
  webContentBuffer += F("<h3>");
  webContentBuffer += S_LOAD_CONFIGURATION;
  webContentBuffer += F("</h3>");
  webContentBuffer += F("<br>");
  webContentBuffer += upload;
  webContentBuffer += F("</div>");
  webContentBuffer += F("<a href='");
  webContentBuffer += getURL(PATH_TOOLS);
  webContentBuffer += F("'><button>");
  webContentBuffer += S_RETURN;
  webContentBuffer += F("</button></a><br><br>");

  WebServer->sendContent();
}

void handleFileUpload() {
  if (!WebServer->isLoggedIn()) {
    return;
  }

  if (SPIFFS.begin()) {
    HTTPUpload& upload = WebServer->httpServer->upload();

    if (upload.status == UPLOAD_FILE_START) {
      dataFile = SPIFFS.open(CONFIG_FILE_PATH, "w");
    }
    else if (upload.status == UPLOAD_FILE_WRITE) {
      if (dataFile)
        dataFile.write(upload.buf, upload.currentSize);
    }
    else if (upload.status == UPLOAD_FILE_END) {
      if (dataFile) {
        dataFile.close();
        // WebServer->httpServer->sendHeader("Location", "/upload");
        // WebServer->httpServer->send(303);
        ConfigManager->load();
      }
      else {
        handleUpload(6);
        // WebServer->httpServer->send(500, "text/plain", "500: couldn't create file");
      }
    }
  }
}
