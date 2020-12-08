#include "SuplaWebPageUpload.h"
#include "SuplaDeviceGUI.h"

static const char uploadIndex[] PROGMEM =
    R"(<form class="formcenter" method="POST" action="/upload" enctype="multipart/form-data">
         <input type="file" accept=".dat" name="config">
         </br>
         <input type="submit" value="Upload">
     </form>)";

File dataFile;

void createWebUpload() {
  // WebServer->httpServer.on(F("/upload"), HTTP_GET, handleUpload);
  WebServer->httpServer.on(F(PATH_UPLOAD), HTTP_GET, []() { handleUpload(); });
  // WebServer->httpServer.on(F("/upload"), HTTP_POST, handleFileUpload);
  WebServer->httpServer.on(
      F(PATH_UPLOAD), HTTP_POST, []() { WebServer->httpServer.send(200); }, handleFileUpload);
}

void handleUpload(int save) {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  String content = "";
  content += WebServer->SuplaSaveResult(save);
  content += WebServer->SuplaJavaScript();
  content += F("<div class='w'>");
  content += F("<h3>");
  content += "Wgraj konfiguracje";
  content += F("</h3>");
  content += F("<br>");
  content += FPSTR(uploadIndex);
  content += F("</div>");
  content += F("<a href='");
  content += getURL(PATH_TOOLS);
  content += F("'><button>");
  content += S_RETURN;
  content += F("</button></a></div>");

  WebServer->sendContent(content);
  // WebServer->httpServer.send(200, PSTR("text/html"), FPSTR(uploadIndex));
}

void handleFileUpload() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  if (SPIFFS.begin()) {
    HTTPUpload& upload = WebServer->httpServer.upload();

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
        // WebServer->httpServer.sendHeader("Location", "/upload");
        // WebServer->httpServer.send(303);
        handleUpload(1);
      }
      else {
        handleUpload(6);
        //WebServer->httpServer.send(500, "text/plain", "500: couldn't create file");
      }
    }
  }
}