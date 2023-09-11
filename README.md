<p align="center">
<img src="https://img.shields.io/github/last-commit/sharandac/My-TTGO-Watch.svg?style=for-the-badge" />
&nbsp;
<img src="https://img.shields.io/github/license/sharandac/My-TTGO-Watch.svg?style=for-the-badge" />
&nbsp;
<a href="https://www.buymeacoffee.com/sharandac" target="_blank"><img src="https://img.shields.io/badge/Buy%20me%20a%20coffee-%E2%82%AC5-orange?style=for-the-badge&logo=buy-me-a-coffee" /></a>
</p>
<hr/>

# My-TTGO-Watch

A GUI named hedge for smartwatch like devices based on ESP32. Currently support for T-Watch2020 (V1,V2,V3), T-Watch2021  (V1 and V2, no ota-updates), M5Paper, M5Core2, WT32-SC01 and native Linux support for testing.

## Features

* BLE communication
* Time synchronization via BLE
* Notification via BLE
* Step counting
* Wake-up on wrist rotation
* Quick actions:

  * WiFi
  * Bluetooth
  * GPS
  * Luminosity
  * Sound volume

* Multiple watch faces:

  * Embedded (digital)
  * [Community based watchfaces](https://sharandac.github.io/My-TTGO-Watchfaces/)

* Multiple 'apps':

  * Music (control the playback of the music on your phone)
  * Navigation (displays navigation instructions coming from the companion app)
  * Map (displays a map)
  * Notification (displays the last notification received)
  * Stopwatch (with all the necessary functions such as play, pause, stop)
  * Alarm
  * Step counter (displays the number of steps and daily objective)
  * Weather
  * Calendar
  * IR remote
  * ...

* Companion apps: Gadgetbridge

## Install

Clone this repository and open it with platformIO. Select the right env and then build and upload.
Or follow the great step by step [tutorial](https://www.youtube.com/watch?v=wUGADCnerCs) from [ShotokuTech](https://github.com/ShotokuTech).

Please check out
    https://github.com/sharandac/My-TTGO-Watch/blob/709ed0c5863435aa966c1d6f44552ddc0909a57c/src/hardware/wifictl.cpp#L256-L261
to setup your wifi when wps or input via display is not possible.

If you are interested in native Linux support, please install sdl2, curl and mosquitto dev lib and change the env to emulator_* in platformIO.

```bash
sudo apt-get install libsdl2-dev libcurl4-gnutls-dev libmosquitto-dev build-essential
```

# Known issues

* the webserver crashes the ESP32 really often
* the battery indicator is not accurate, rather a problem with the power management unit ( axp202 )

## Development on the Windows platform

The development tools have a known issue with the size of the project on Windows platforms. When the program is built you may receive the following error:

    xtensa-esp32-elf-g++: error: CreateProcess: No such file or directory
    *** [.pio\build\t-watch2020-v1\firmware.elf] Error 1

This issue has not been seen on Linux or other platforms. This is a linker issue and can be fixed by removing apps. To remove unneeded apps you can simply delete the appropriate directory in /src/app. Then simply recompile. App can be added in the same way. But note that the app must support autocall_function . This allows the automatic integration of apps without touching the rest of the code ( [the magic behind autocall_function](autocall.md) ).

Since each app includes a different set of files, you may need to delete several apps to reduce it small enough for the Windows build.

# How to use

Cf. [Usage](USAGE.md)

# Forks that are recommended

[Pickelhaupt](https://github.com/Pickelhaupt/EUC-Dash-ESP32)<br>
[FantasyFactory](https://github.com/FantasyFactory/My-TTGO-Watch)<br>
[NorthernDIY](https://github.com/NorthernDIY/My-TTGO-Watch)<br>
[linuxthor](https://github.com/linuxthor/Hackers-TTGO-Watch)<br>
[d03n3rfr1tz3](https://github.com/d03n3rfr1tz3/TTGO.T-Watch.2020)<br>
[lunokjod](https://github.com/lunokjod/watch)<br>

# For the programmers

Cf. [contribution guide](CONTRIBUTING.md)<br>
app autocall function [the magic behind autocall_function](autocall.md) or add a app without touching the rest

# Interface

## TTGO T-Watch 2020

![screenshot](images/screen1.png)
![screenshot](images/screen2.png)
![screenshot](images/screen3.png)
![screenshot](images/screen4.png)
![screenshot](images/screen5.png)
![screenshot](images/screen6.png)
![screenshot](images/screen7.png)
![screenshot](images/screen8.png)
![screenshot](images/screen9.png)
![screenshot](images/screen10.png)
![screenshot](images/screen11.png)
![screenshot](images/screen12.png)

## M5Paper ( downscaled )

![screenshot](images/image1.png)
![screenshot](images/image2.png)
![screenshot](images/image3.png)

## M5Core2

![screenshot](images/m5core2_img1.png)
![screenshot](images/m5core2_img2.png)
![screenshot](images/m5core2_img3.png)
![screenshot](images/m5core2_img4.png)

## WT32-SC01

![screenshot](images/WT32_SC01_img1.png)
![screenshot](images/WT32_SC01_img2.png)
![screenshot](images/WT32_SC01_img3.png)

## TTGO T-Watch 2021

![screenshot](images/twatch2021_img1.png)
![screenshot](images/twatch2021_img2.png)
![screenshot](images/twatch2021_img3.png)

# Contributors

Special thanks to the following people for their help:

[5tormChild](https://github.com/5tormChild)<br>
[bwagstaff](https://github.com/bwagstaff)<br>
[chrismcna](https://github.com/chrismcna)<br>
[datacute](https://github.com/datacute)<br>
[fliuzzi02](https://github.com/fliuzzi02)<br>
[guyou](https://github.com/guyou)<br>
[jakub-vesely](https://github.com/jakub-vesely)<br>
[joshvito](https://github.com/joshvito)<br>
[JoanMCD](https://github.com/JoanMCD)<br>
[NorthernDIY](https://github.com/NorthernDIY)<br>
[Neuroplant](https://github.com/Neuroplant)<br>
[paulstueber](https://github.com/paulstueber)<br>
[pavelmachek](https://github.com/pavelmachek)<br>
[rnisthal](https://github.com/rnisthal)<br>
[ssspeq](https://github.com/ssspeq)<br>

and the following projects:

[ArduinoJson](https://github.com/bblanchon/ArduinoJson)<br>
[AsyncTCP](https://github.com/me-no-dev/AsyncTCP)<br>
[ESP32SSDP](https://github.com/luc-github/ESP32SSDP)<br>
[ESP32-targz](https://github.com/tobozo/ESP32-targz)<br>
[ESP8266Audio](https://github.com/earlephilhower/ESP8266Audio)<br>
[ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)<br>
[LVGL](https://github.com/lvgl)<br>
[NimBLE-Arduino]()h2zero/NimBLE-Arduino<br>
[pubsubclient](https://github.com/knolleary/pubsubclient)<br>
[TinyGPSPlus](mikalhart/TinyGPSPlus)<br>
[TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)<br>
[TTGO_TWatch_Library](https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library)<br>

Every Contribution to this repository is highly welcome! Don't fear to create pull requests which enhance or fix the project, you are going to help everybody.
<p>
If you want to donate to the author then you can buy me a coffee.
<br/><br/>
<a href="https://www.buymeacoffee.com/sharandac" target="_blank"><img src="https://img.shields.io/badge/Buy%20me%20a%20coffee-%E2%82%AC5-orange?style=for-the-badge&logo=buy-me-a-coffee" /></a>
</p>
