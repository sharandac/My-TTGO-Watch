
#include <WiFi.h>
#include <WiFiClient.h>
#include <Update.h>
#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>

#include "webserver.h"
#include "config.h"
#include "gui/screenshot.h"

AsyncWebServer asyncserver( WEBSERVERPORT );
TaskHandle_t _WEBSERVER_Task;


void handleUpdate( AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {

  if (!index){
    /*
     * if filename includes spiffs, update the spiffs partition
     */
    int cmd = (filename.indexOf("spiffs") > 0) ? U_SPIFFS : U_FLASH;
    if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) {
      Update.printError(Serial);
    }
  }

  /*
   * Write Data an type message if fail
   */
  if (Update.write(data, len) != len) {
    Update.printError(Serial);
  }

  /*
   * After write Update restart
   */
  if (final) {
    AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "Please wait while the switch reboots");
    response->addHeader("Refresh", "20");  
    response->addHeader("Location", "/");
    request->send(response);
    if (!Update.end(true)){
      Update.printError(Serial);
    } else {
      Serial.println("Update complete");
      Serial.flush();
      ESP.restart();
    }
  }
}

/*
 *
 */
void asyncwebserver_setup(void){

  asyncserver.on("/info", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", "Firmwarestand: " __DATE__ " " __TIME__ "\r\nGCC-Version: " __VERSION__ "\r\n" );
  });

  asyncserver.on("/shot", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", "Firmwarestand: " __DATE__ " " __TIME__ "\r\nGCC-Version: " __VERSION__ "\r\n" );
    screenshot_take();
    screenshot_save();
  });

  asyncserver.addHandler(new SPIFFSEditor(SPIFFS));
  asyncserver.serveStatic("/", SPIFFS, "/").setDefaultFile("index.htm");

  asyncserver.onNotFound([](AsyncWebServerRequest *request){
    Serial.printf( "NOT_FOUND: ");
    if(request->method() == HTTP_GET)
      Serial.printf( "GET");
    else if(request->method() == HTTP_POST)
      Serial.printf( "POST");
    else if(request->method() == HTTP_DELETE)
      Serial.printf( "DELETE");
    else if(request->method() == HTTP_PUT)
      Serial.printf( "PUT");
    else if(request->method() == HTTP_PATCH)
      Serial.printf( "PATCH");
    else if(request->method() == HTTP_HEAD)
      Serial.printf( "HEAD");
    else if(request->method() == HTTP_OPTIONS)
      Serial.printf( "OPTIONS");
    else
      Serial.printf( "UNKNOWN");
    Serial.printf( " http://%s%s\n", request->host().c_str(), request->url().c_str());

    if(request->contentLength()){
      Serial.printf( "_CONTENT_TYPE: %s\n", request->contentType().c_str());
      Serial.printf( "_CONTENT_LENGTH: %u\n", request->contentLength());
    }

    int headers = request->headers();
    int i;
    for(i=0;i<headers;i++){
      AsyncWebHeader* h = request->getHeader(i);
      Serial.printf( "_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    }

    int params = request->params();
    for(i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isFile()){
        Serial.printf( "_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      } else if(p->isPost()){
        Serial.printf( "_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      } else {
        Serial.printf( "_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }
    request->send(404);
  });

  asyncserver.onFileUpload([](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
    if(!index)
      Serial.printf( "UploadStart: %s\n", filename.c_str());
    Serial.printf("%s", (const char*)data);
    if(final)
      Serial.printf( "UploadEnd: %s (%u)\n", filename.c_str(), index+len);
  });

  asyncserver.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    if(!index) {
      Serial.printf( "BodyStart: %u\n", total);
    }
    Serial.printf( "%s", (const char*)data);
    if(index + len == total) {
      Serial.printf( "BodyEnd: %u\n", total);
    }
  });

  asyncserver.on("/reset", HTTP_GET, []( AsyncWebServerRequest * request ) {
    request->send(200, "text/plain", "Reset\r\n" );
    delay(3000);
    ESP.restart();    
  });

  asyncserver.on("/update", HTTP_POST,
    [](AsyncWebServerRequest *request) {},
    [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) { handleUpdate(request, filename, index, data, len, final); }
  );

  asyncserver.begin();
}