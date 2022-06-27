/****************************************************************************
 *   Tu May 22 21:23:51 2020
 *   Copyright  2020  Dirk Brosswick
 *   Email: dirk.brosswick@googlemail.com
 ****************************************************************************/
 
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include "webserver.h"
#include "config.h"

#if defined( ENABLE_WEBSERVER )
    #ifdef NATIVE_64BIT
        void asyncwebserver_start(void){ return; };
        void asyncwebserver_end(void){ return; }
    #else
        #include <WiFi.h>
        #include <WiFiClient.h>
        #include <Update.h>
        #include <SPIFFS.h>
        #include <FS.h>
        #include <AsyncTCP.h>
        #include <ESPAsyncWebServer.h>
        #include <SPIFFSEditor.h>
        #include <ESP32SSDP.h>

        AsyncWebServer asyncserver( WEBSERVERPORT );
        TaskHandle_t _WEBSERVER_Task;
        AsyncWebHandler mHandler_SPIFFSEditor;
        SPIFFSEditor * mSPIFFSEditor = nullptr;

    static const char* serverIndex =
        "<!DOCTYPE html>\n <html><head>\n <script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
        "\n <script src='/jquery.min.js'></script>"

        "\n <style>"
        "\n #progressbarfull {"
        "\n background-color: #20201F;"
        "\n border-radius: 20px;"
        "\n width: 320px;"
        "\n padding: 4px;"
        "\n}"
        "\n #progressbar {"
        "\n background-color: #20CC00;"
        "\n width: 3%;"
        "\n height: 16px;"
        "\n border-radius: 10px;"
        "\n}"
        "\n</style>"
        "\n </head><body>"
        "<h2>Update by Browser</h2>"
        "\n <form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
        "\n <input type='file' name='update'>"
        "\n <br><br><input type='submit' value='Update'>"
        "\n </form>"
        "\n <div id='prg'>Progress: 0%</div>"
        "\n <div id=\"progressbarfull\"><div id=\"progressbar\"></div></div>"
        "\n <script>"
        "\n $('form').submit(function(e){"
        "\n e.preventDefault();"
        "\n var form = $('#upload_form')[0];"
        "\n var data = new FormData(form);"
        "\n $.ajax({"
        "\n url: '/update',"
        "\n type: 'POST',"
        "\n data: data,"
        "\n contentType: false,"
        "\n processData:false,"
        "\n xhr: function() {"
        "\n var xhr = new window.XMLHttpRequest();"
        "\n xhr.upload.addEventListener('progress', function(evt) {"
        "\n if (evt.lengthComputable) {"
        "\n var per = evt.loaded / evt.total;"
        "\n document.getElementById(\"prg\").innerHTML = 'Progress: ' + Math.round(per*100) + '%';"
        "\n document.getElementById(\"progressbar\").style.width=Math.round(per*100)+ '%';"
        "}"
        "}, false);"
        "return xhr;"
        "},"
        "\n success:function(d, s) {"
        "\n document.getElementById(\"prg\").innerHTML = 'Progress: success';"
        "\n console.log('success!')"
        "},"
        "\n error: function (a, b, c) {"
        "\n document.getElementById(\"prg\").innerHTML = 'Progress: error';"
        "}"
        "});"
        "});"
        "\n </script>"
        "\n </body></html>";

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
    void asyncwebserver_start(void){
    asyncserver.on("/index.htm", HTTP_GET, [](AsyncWebServerRequest *request) {
        String html = (String) "<!DOCTYPE html>"
        "<html>"
        "<frameset cols=\"300, *\">"
        "<frame src=\"/nav.htm\" name=\"nav\">"
        "<frame name=\"cont\">"
        "</frameset>"
        "</html>";
        request->send(200, "text/html", html);
    });

    asyncserver.on("/nav.htm", HTTP_GET, [](AsyncWebServerRequest *request) {
        String html = (String) "<!DOCTYPE html>"
        "<html><head>"
        "<meta http-equiv='Content-type' content='text/html; charset=utf-8'>"
        "<title>Web Interface</title>"
        "</head><body>"
        "<h1>TTGo Watch Web Server</h1>"
        "<p>This is your device, program it as you see fit."
        "<p>Here are some URLs the device already supports, which you might find helpful:"
        "<ul>"
        "<li><a target=\"cont\" href=\"/info\">/info</a> - Display information about the device"
        "<li><a target=\"cont\" href=\"/memory\">/memory</a> - Display memory information"
        "<li><a target=\"cont\" href=\"/battery\">/battery</a> - Display battery charging information"
        "<li><a target=\"cont\" href=\"/touch\">/touch</a> - Display touch screen information"
        "<li><a target=\"cont\" href=\"/network\">/network</a> - Display network information"
        "<li><a target=\"cont\" href=\"/shot\">/shot</a> - Capture a screen shot"
        "<li><a target=\"cont\" href=\"/screen.png\">/screen.png</a> - Retrieve the image in png format, open it with gimp"
        "<li><a target=\"_blank\" href=\"/edit\">/edit</a> - View, edit, upload, and delete files"
        "</ul>"
        "<p><div style=\"color:red;\">Caution:</div> Use these with care:"
        "<ul><li><a target=\"cont\"  href=\"/reset\">/reset</a> Reboot the device"
        "<li><a target=\"_top\" href=\"/update\">/update</a> Transmit a firmware update through POST request"
        "</body></html>";
        request->send(200, "text/html", html);
    });

    asyncserver.on("/info", HTTP_GET, [](AsyncWebServerRequest *request) {
        FlashMode_t mode = ESP.getFlashChipMode();
        uint32_t FreeSketchSpace = ESP.getFreeSketchSpace();
        uint32_t SketchFull = ESP.getSketchSize() + FreeSketchSpace;

        String html = (String) "<html><head><meta charset=\"utf-8\"></head><body><h3>Information</h3>" +
                    "<b><u>Memory</u></b><br>" +
                    "<b>Heap size: </b>" + ESP.getHeapSize() + "<br>" +
                    "<b>Heap free: </b>" + ESP.getFreeHeap() + "<br>" +
                    "<b>Heap free min: </b>" + ESP.getMinFreeHeap() + "<br>" +
                    "<b>Psram size: </b>" + ESP.getPsramSize() + "<br>" +
                    "<b>Psram free: </b>" + ESP.getFreePsram() + "<br>" +

    //                  "<br><b><u>System</u></b><br>" +
    //                  "\t<b>Battery voltage: </b>" + TTGOClass::getWatch()->power->getBattVoltage() / 1000 + " Volts" + "<br>" +

                    "\t<b>Uptime: </b>" + millis() / 1000 + "<br>" +
                    "<br><b><u>Chip</u></b>" +
                    "<br><b>SdkVersion: </b>" + String(ESP.getSdkVersion()) + "<br>" +
                    "<b>CpuFreq: </b>" + String(ESP.getCpuFreqMHz()) + " MHz<br>" +
                    
                    "<br><b><u>Flash</u></b><br>" +
                    "<b>FlashChipSpeed: </b>" + String(ESP.getFlashChipSpeed() / 1000000) + " MHz<br>" +
                    "<b>Flash mode: </b>" + String( mode == FM_QIO ? "QIO" : mode == FM_QOUT ? "QOUT" : mode == FM_DIO ? "DIO" : mode == FM_DOUT ? "DOUT" : "UNKNOWN") + "</b><br>" +
                    "<b>Flash sector size: </b>" + String( SPI_FLASH_SEC_SIZE) + "<br>" +
                    "<b>FlashChipMode: </b>" + mode + "<br>" +
                    "<b>FlashChipSize (SDK): </b>" + ESP.getFlashChipSize() + "<br>" +
                    
                    "<br><b><u>Firmware</u></b><br>" +
                    "<b>SketchSpace free: </b>" + FreeSketchSpace + " (" + (FreeSketchSpace / (SketchFull / 100)) + "%)<br>" +
                    "<b>BuildTime: </b>" +  __DATE__ + " " + __TIME__  + "<br>" +
                    "<b>Version: </b>" + __FIRMWARE__ + "<br>" +
                    "<b>GCC-Version: </b>" + __VERSION__ + "<br>" +
                    //"<b>SketchMD5: </b>" + String(ESP.getSketchMD5()) + "<br>" +
                    
                    //"<br><b><u>Filesystem</u></b><br>" +
                    //"<b>Total size: </b>" + SPIFFS.totalBytes() + "<br>" +
                    //"<b>Used size: </b>" + SPIFFS.usedBytes() + "<br>" +

                    "</body></html>";
        request->send(200, "text/html", html);
    });

    asyncserver.on("/memory", HTTP_GET, [](AsyncWebServerRequest *request) {
        String html = (String) "<html><head><meta charset=\"utf-8\"></head><body><h3>Memory Details</h3>" +
                    "<b>Heap size: </b>" + ESP.getHeapSize() + "<br>" +
                    "<b>Heap free: </b>" + ESP.getFreeHeap() + "<br>" +
                    "<b>Heap free min: </b>" + ESP.getMinFreeHeap() + "<br>" +
                    "<b>Psram size: </b>" + ESP.getPsramSize() + "<br>" +
                    "<b>Psram free: </b>" + ESP.getFreePsram() + "<br>" +

                    "<br><b><u>System</u></b><br>" +
                    "<b>Uptime: </b>" + millis() / 1000 + "<br>" +
                    "</body></html>";
        request->send(200, "text/html", html);
    });

    /*
    asyncserver.on("/battery", HTTP_GET, [](AsyncWebServerRequest *request) {
        TTGOClass * ttgo = TTGOClass::getWatch();

        String html = (String) "<html><head><meta charset=\"utf-8\"></head><body><h3>Battery Details</h3>" +
                    "<b>Battery voltage: </b>" + ttgo->power->getBattVoltage() / 1000 + " Volts" + "<br>" +
                    "<b>Coulomb Charge: </b>" + ttgo->power->getBattChargeCoulomb() + "<br>" +
                    "<b>Coulomb Discharge: </b>" + ttgo->power->getBattDischargeCoulomb() + "<br>" +
                    "<b>Current Charge: </b>" + ttgo->power->getBattChargeCurrent() + "<br>" +
                    "<b>Current Discharge: </b>" + ttgo->power->getBattDischargeCurrent() + "<br>" +
                    "<b>Fuel Gauge: </b>" + ttgo->power->getBattPercentage() + "%" + "<br>" +
                    "<b>Calculated: </b>" + ttgo->power->getCoulombData() + "mAh remaining" + "<br>" +

                    "<br><b><u>System</u></b><br>" +
                    "<b>Uptime: </b>" + millis() / 1000 + "<br>" +
                    "</body></html>";
        request->send(200, "text/html", html);
    });

    asyncserver.on("/touch", HTTP_GET, [](AsyncWebServerRequest *request) {
        TTGOClass * ttgo = TTGOClass::getWatch();

        String html;
        if ( touch_lock_take() ) {

            html = (String) "<html><head><meta charset=\"utf-8\"></head><body><h3>Touch Parameters</h3>" +
                        "<b>Device Mode: </b>" + ttgo->touch->getDeviceMode() + "<br>" +
                        "<b>Interrupt Mode: </b>" + ttgo->touch->getINTMode() + "<br>" +
                        "<b>Control: </b>" + ttgo->touch->getControl() + "<br>" +
                        "<b>Power Mode: </b>" + ttgo->touch->getPowerMode() + "<br>" +
                        "<b>Monitor time: </b>" + ttgo->touch->getMonitorTime() + "<br>" +
                        "<b>Active period: </b>" + ttgo->touch->getActivePeriod() + "<br>" +
                        "<b>Monitor period: </b>" + ttgo->touch->getMonitorPeriod() + "<br>" +

                        "<br><b><u>System</u></b><br>" +
                        "<b>Uptime: </b>" + millis() / 1000 + "<br>" +
                        "</body></html>";
            touch_lock_give();
        } else {
            html = (String) "<html><head><meta charset=\"utf-8\"></head><body>No Lock</body></html>";
        }
        request->send(200, "text/html", html);
    });

    asyncserver.on("/temp", HTTP_GET, [](AsyncWebServerRequest *request) {
        TTGOClass * ttgo = TTGOClass::getWatch();

        String html = (String) "<html><head><meta charset=\"utf-8\"></head><body><h3>Device Temperature</h3>" +
                    "<b>AXP202: </b>" + ttgo->power->getTemp() + "<br>" +
                    "<b>BMA423: </b>" + ttgo->bma->temperature() + "<br>" +

                    "<br><b><u>System</u></b><br>" +
                    "<b>Uptime: </b>" + millis() / 1000 + "<br>" +
                    "</body></html>";
        request->send(200, "text/html", html);
    });
    */
    asyncserver.on("/network", HTTP_GET, [](AsyncWebServerRequest *request) {
        String html = (String) "<html><head><meta charset=\"utf-8\"></head><body><h3>Network</h3>" +
                    "<b>IP Addr: </b>" + WiFi.localIP().toString() + "<br>" +
                    "<b>MAC: </b>" + WiFi.macAddress() + "<br>" +
                    "<b>SNMask: </b>" + WiFi.subnetMask().toString() + "<br>" +
                    "<b>GW IP: </b>" + WiFi.gatewayIP().toString() + "<br>" +
                    "<b>DNS 1: </b>" + WiFi.dnsIP(0).toString() + "<br>" +
                    "<b>DNS 2: </b>" + WiFi.dnsIP(1).toString() + "<br>" +
                    "<b>RSSI: </b>" + String(WiFi.RSSI()) + "dB<br>" +
                    "<b>Hostname: </b>" + WiFi.getHostname() + "<br>" +
                    "<b>SSID: </b>" + WiFi.SSID() + "<br>" +
                    "<br>Upnp Info: <a target=\"_blank\" href='/description.xml'>description.xml</a>" + "<br>" +
                    "</body></html>";
        request->send(200, "text/html", html);
    });
    /*
    asyncserver.on("/shot", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send(200, "text/plain", "screen is taken\r\n" );
        screenshot_take();
        screenshot_save();
    });
    */
    //start FsEditor with SPIFFS
    setFsEditorFilesystem(SPIFFS);

    asyncserver.rewrite("/", "/index.htm");
    asyncserver.serveStatic("/", SPIFFS, "/");

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

    asyncserver.on("/update", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send(200, "text/html", serverIndex);
    });

    asyncserver.on(
        "/update", HTTP_POST,
        [](AsyncWebServerRequest *request) {},
        [](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) { handleUpdate(request, filename, index, data, len, final); }
    );

    asyncserver.on("/description.xml", HTTP_GET, [](AsyncWebServerRequest *request) {
        byte mac[6];
        WiFi.macAddress(mac);
        char tmp[6 + 1];
        snprintf(tmp, sizeof(tmp), "%02X%02X%02X", mac[3], mac[4], mac[5]);
        String MacStrPart = String(tmp);

        String xmltext = String("<?xml version=\"1.0\"?>\n") +
                "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">\n"
                "<specVersion>\n"
                "\t<major>1</major>\n"
                "\t<minor>0</minor>\n"
                "</specVersion>\n"
                "<URLBase>http://" + WiFi.localIP().toString() + "/</URLBase>\n" 
                "<device>\n"
                "\t<deviceType>upnp:rootdevice</deviceType>\n"

                /*this is the icon name in Windows*/
                /*"\t<friendlyName>" + WiFi.getHostname() + "</friendlyName>\n"*/ 
                "\t<friendlyName>" + DEV_NAME + " " + MacStrPart + "</friendlyName>\n" /*because the hostename is 'Espressif' */

                "\t<presentationURL>/</presentationURL>\n"
                "\t<manufacturer>" + "Dirk Broßwick (sharandac)" + "</manufacturer>\n"
                "\t<manufacturerURL>https://github.com/sharandac/My-TTGO-Watch</manufacturerURL>\n"
                "\t<modelName>" +  DEV_INFO + "</modelName>\n"

                "\t<modelNumber>" + WiFi.getHostname() + "</modelNumber>\n"
                "\t<modelURL>" +
                "/" + "</modelURL>\n"

                "\t<serialNumber>Build: " + __FIRMWARE__ + "</serialNumber>\n"
                //The last six bytes of the UUID are the hardware address of the first Ethernet adapter in the system the UUID was generated on.
                "\t<UDN>uuid:38323636-4558-4DDA-9188-CDA0E6" + MacStrPart + "</UDN>\n"
                "</device>\n"
                "</root>\r\n"
                "\r\n";

        request->send(200, "text/xml", xmltext);
    });

    //Upnp / SSDP presentation - Multicast  - link to description.xml
    SSDP.setSchemaURL("description.xml");
    SSDP.setHTTPPort( UPNPPORT );
    SSDP.setURL("/");
    SSDP.setDeviceType("upnp:rootdevice");
    SSDP.begin();

    asyncserver.begin();

    log_d("enable webserver and ssdp");
    }

    void asyncwebserver_end(void) {
        SSDP.end();
        asyncserver.end();
        log_d("disable webserver and ssdp");
    }

    void setFsEditorFilesystem(const fs::FS& fs) {
        log_d("asyncserver.removeHandler");
        asyncserver.removeHandler(&mHandler_SPIFFSEditor);
        if(mSPIFFSEditor!=nullptr)
            delete mSPIFFSEditor;  
        mSPIFFSEditor = new SPIFFSEditor(fs);
        log_d("asyncserver.addHandler");
        mHandler_SPIFFSEditor = asyncserver.addHandler(mSPIFFSEditor);
    }
    #endif
#endif // ENABLE_WEBSERVER