/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#include "jsonrequest.h"
#include "HTTPClient.h"

JsonRequest::JsonRequest(size_t maxJsonBufferSize) : SpiRamJsonDocument(maxJsonBufferSize)
{
}

JsonRequest::~JsonRequest() {
    clear();
}

bool JsonRequest::process(const char* url)
{
    if (httpcode != -1)
      clear();

    HTTPClient client;

    client.useHTTP10(true);
    client.begin(url);
    client.addHeader("force-unsecure","true");
    httpcode = client.GET();

    time(&now);
    localtime_r(&now, &timeStamp);
    
    if (httpcode != 200) {
        log_e("HTTPClient error %d", httpcode, url);
        client.end();
        return false;
    }

    dsError = deserializeJson(*this, client.getStream());
    if (dsError) {
        log_e("deserializeJson() failed: %s", dsError.c_str());
        clear();
        client.end();
        return false;
    }
    client.end();

    return true;
}

String JsonRequest::formatCompletedAt(const char* format)
{
    char txttime[64];
    strftime(txttime, sizeof(txttime), format, &timeStamp);
    return String(txttime);
}

String JsonRequest::errorString() {
    if (httpcode != 200)
        return String("HTTP error: ") + httpcode;
    else if (dsError)
        return String(dsError.c_str());
    else
        return "";
}