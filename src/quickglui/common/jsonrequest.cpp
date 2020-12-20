/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#include "jsonrequest.h"
#include "HTTPClient.h"

JsonRequest::JsonRequest(size_t maxJsonBufferSize) : document(maxJsonBufferSize)
{
}

JsonRequest::~JsonRequest() {
    document.clear();
}

bool JsonRequest::process(const char* url)
{
    if (httpcode != -1)
      document.clear();

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

    dsError = deserializeJson(document, client.getStream());
    if (dsError) {
        log_e("deserializeJson() failed: %s", dsError.c_str());
        document.clear();
        client.end();
        return false;
    }
    client.end();

    return true;
}

JsonDocument& JsonRequest::result() {
    return document;
}

String JsonRequest::fromatCompletedAt(const char* format)
{
    char txttime[64];
    strftime(txttime, sizeof(txttime), format, &timeStamp);
    return String(txttime);
}
