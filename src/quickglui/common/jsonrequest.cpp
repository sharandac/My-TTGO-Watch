/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#include "jsonrequest.h"
#include "utils/uri_load/uri_load.h"

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

    time(&now);
    localtime_r(&now, &timeStamp);
    /**
     * load uri file into ram
     */
    uri_load_dsc_t *uri_load_dsc = uri_load_to_ram( url );
    /**
     * if was success, pharse the json
     */
    if ( uri_load_dsc ) {
        httpcode = 200;

        dsError = deserializeJson(*this, uri_load_dsc->data );
        if (dsError) {
            log_e("deserializeJson() failed: %s", dsError.c_str());
            clear();
            return false;
        }
    }
    else {
        httpcode = -1;
    }
    /**
     * clear uri dsc
     */
    uri_load_free_all( uri_load_dsc );

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
        return String("HTTP error");
    else if (dsError)
        return String(dsError.c_str());
    else
        return "";
}