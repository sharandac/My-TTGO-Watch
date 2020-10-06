/****************************************************************************
 *   July 23 00:23:05 2020
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
#include "config.h"
#include "HTTPClient.h"

#include "image_ticker.h"
#include "image_ticker_fetch.h"

// Include SPIFFS
#include "SPIFFS.h" // ESP32 only
#include "lodepng.h"


bool image_ticker_fetch_jpg() {

  image_ticker_config_t * image_ticker_config = image_ticker_get_config();

  log_i("Fetching image from %s!", image_ticker_config->url );

  if (strlen(image_ticker_config->url)==0) return (false);

  if (getFile(image_ticker_config->url, "/imgtick.png")) {

    log_i("Image loaded !");
    return true;
    
  } else { 
    log_e("Some error !");
    return false; }

}

// Fetch a file from the URL given and save it in SPIFFS
// Return 1 if a web fetch was needed or 0 if file already exists
bool getFile(String url, String filename) {

/*   // If it exists then no need to fetch it
  if (SPIFFS.exists("/") == true) {
    log_e("Removing old image %s", filename );
    SPIFFS.remove(filename);
    return 0;
  }
 */
  int httpcode = -1;
  HTTPClient http_client;

  http_client.useHTTP10( true );
  http_client.begin( url );
  http_client.addHeader("force-unsecure","true");
  httpcode = http_client.GET();

  if ( httpcode != 200 ) {
      log_e("HTTPClient error %d from %s", httpcode, url );
      return( -1 );
  } else {

    String in=http_client.getString();

    if(in[0] != 137 || in[1] != 80 || in[2] != 78 || in[3] != 71
     || in[4] != 13 || in[5] != 10 || in[6] != 26 || in[7] != 10) {
      log_e("Not a valid png");
      return 0;
    } else {

      fs::File f = SPIFFS.open(filename, "w");
      if (!f) {
        log_e("file open failed");
        return 0;
      }
      
      f.print(in);

      f.close();
      log_i("file %s was fetched from %s", filename, url);
      return 1; // File was fetched from web
    }
  }

  http_client.end();
  
}



