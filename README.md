<p align="center">
<img src="https://img.shields.io/github/last-commit/sharandac/My-TTGO-Watch.svg?style=for-the-badge" />
&nbsp;
<img src="https://img.shields.io/github/license/sharandac/My-TTGO-Watch.svg?style=for-the-badge" />
&nbsp;
<a href="https://www.buymeacoffee.com/sharandac" target="_blank"><img src="https://img.shields.io/badge/Buy%20me%20a%20coffee-%E2%82%AC5-orange?style=for-the-badge&logo=buy-me-a-coffee" /></a>
</p>
<hr/>

# My-TTGO-Watch

A smartwatch based on ESP32 from LilyGo. Currently under development.

# Telegram chatgroup

Telegram chatgroup is here:
https://t.me/TTGO_Watch

# Install

Clone this repository and open it with platformIO. Build and upload. On a terminal in vscode you can do it with

```bash
pio run -t upload
```

or simple press "build and upload" in platformIO.

# known issues

* the webserver crashes the ESP32 really often
* the battery indicator is not accurate, rather a problem with the power management unit ( axp202 )
* from time to time the esp32 crashes accidentally
* and some other small things

# how to use

## weather app

On startup you see the main screen (time tile). It show the time and the current weather (if correct configure). Now you can swipe with you fingers up, down, left and right between the four main screens. The four screens are organized in time, apps, note and setup tile.
For the weather app you need an openweather.com api-id. http://openweathermap.org/appid is a good starting point.

## bluetooth

The bluetooth notification work with [gadgetbridge](https://gadgetbridge.org) very well. But keep in mind, bluetooth in standby reduces the battery runtime. In connection with [OsmAnd](https://osmand.net) the watch can also be used for navigation. Please use the osmand app, otherwise a lot of messages will be displayed.

# Forks that are recommended

[FantasyFactory](https://github.com/FantasyFactory/My-TTGO-Watch)<br>
[NorthernDIY](https://github.com/NorthernDIY/My-TTGO-Watch)<br>

# for the programmers

Internal RAM is very limited, use PSRAM as much as possible. When you work with ArduinoJson, include this

```#include "hardware/json_psram_allocator.h"```

and create your json with

```SpiRamJsonDocument doc( 1000 );```

to move your json into PSRAM, here is enough RAM for all the crazy stuff you will do. And use

```ps_malloc(), ps_calloc() and ps_realloc()```

as often as possible.
And one very important thing: Do not talk directly to the hardware!

## Sound
To play sounds from the inbuild speakers use `hardware/sound.h`:

```
#include "hardware/sound.h"
[...]
// MP3 from SPIFFS:
// void sound_play_spiffs_mp3( const char *filename );
// example:
sound_play_spiffs_mp3( "/sound.mp3" )

// or WAV from PROGMEM via
//void sound_play_progmem_wav( const void *data, uint32_t len );

```

There is a configuration tile to enable/disable all sound output and set the global volume.

# how to make a screenshot
The firmware has an integrated webserver. Over this a screenshot can be triggered. The image has the format RGB565 and can be read with gimp. From bash it look like this
```bash
wget x.x.x.x/shot ; wget x.x.x.x/screen.565
```

# Interface

![screenshot](https://github.com/sharandac/My-TTGO-Watch/blob/master/images/screen1.png)
![screenshot](https://github.com/sharandac/My-TTGO-Watch/blob/master/images/screen2.png)
![screenshot](https://github.com/sharandac/My-TTGO-Watch/blob/master/images/screen3.png)
![screenshot](https://github.com/sharandac/My-TTGO-Watch/blob/master/images/screen4.png)
![screenshot](https://github.com/sharandac/My-TTGO-Watch/blob/master/images/screen5.png)
![screenshot](https://github.com/sharandac/My-TTGO-Watch/blob/master/images/screen6.png)
![screenshot](https://github.com/sharandac/My-TTGO-Watch/blob/master/images/screen7.png)
![screenshot](https://github.com/sharandac/My-TTGO-Watch/blob/master/images/screen8.png)
![screenshot](https://github.com/sharandac/My-TTGO-Watch/blob/master/images/screen9.png)
![screenshot](https://github.com/sharandac/My-TTGO-Watch/blob/master/images/screen10.png)
![screenshot](https://github.com/sharandac/My-TTGO-Watch/blob/master/images/screen11.png)


# Contributors

Special thanks to the following people for their help:

[5tormChild](https://github.com/5tormChild)<br>
[bwagstaff](https://github.com/bwagstaff)<br>
[chrismcna](https://github.com/chrismcna)<br>
[datacute](https://github.com/datacute)<br>
[jakub-vesely](https://github.com/jakub-vesely)<br>
[joshvito](https://github.com/joshvito)<br>
[JoanMCD](https://github.com/JoanMCD)<br>
[NorthernDIY](https://github.com/NorthernDIY)<br>
[rnisthal](https://github.com/rnisthal)<br>
[paulstueber](https://github.com/paulstueber)<br>
[ssspeq](https://github.com/ssspeq)<br>

and the following projects:

[ArduinoJson](https://github.com/bblanchon/ArduinoJson)<br>
[AsyncTCP](https://github.com/me-no-dev/AsyncTCP)<br>
[ESP32SSDP](https://github.com/luc-github/ESP32SSDP)<br>
[ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)<br>
[LVGL](https://github.com/lvgl)<br>
[TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)<br>
[TTGO_TWatch_Library](https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library)<br>
[ESP8266Audio](https://github.com/earlephilhower/ESP8266Audio)<br>
[pubsubclient](https://github.com/knolleary/pubsubclient)<br>

Every Contribution to this repository is highly welcome! Don't fear to create pull requests which enhance or fix the project, you are going to help everybody.
<p>
If you want to donate to the author then you can buy me a coffee.
<br/><br/>
<a href="https://www.buymeacoffee.com/sharandac" target="_blank"><img src="https://img.shields.io/badge/Buy%20me%20a%20coffee-%E2%82%AC5-orange?style=for-the-badge&logo=buy-me-a-coffee" /></a>
</p>
