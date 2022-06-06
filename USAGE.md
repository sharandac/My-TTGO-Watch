# Main screen

![screenshot](images/screen1.png)

On startup you see the main screen (time tile). It shows the time and widgets.

Widgets are:

* the current weather (if correctly configured).
* the next alarm.
* the notifications.

# Screen Navigation

 You can swipe with you fingers up, down, left and right between the four main screens. The four screens are organized in time, apps, note and setup tile.

# Quick Settings

![screenshot](images/screen2.png)

A subset of settings can be accessed via a swipe from the top of the screen.

# Settings

![screenshot](images/screen3.png)
![screenshot](images/screen4.png)

Once a setting is selected, you can leave the form with the exit button.

## Battery

Battery status.

## Display

Set color, background, touch feedback with vibrations...

![screenshot](images/display_1.png)
![screenshot](images/display_2.png)

## Touch

Touch calibration menu.

![screenshot](images/touch1.png)
![screenshot](images/touch2.png)

## Move

Enable:

* step counter
* double click
* tilt
* display of step counter

The stepcounter value is published to [gadgetbridge](https://gadgetbridge.org) automatically if bluetooth is enabled.
The frequency of publication is driven by gadgetbridge.
Initially, it is on a 30 minutes frequency.
When the realtime tab of gadgetbridge is selected, the frequency is set to every 5 seconds.
If the watch lost contact with gadgetbridge for more than 30 minutes, the stepcounter is also refreshed when bluetooth is reconnected.

![screenshot](images/move.png)

## Bluetooth

The bluetooth notification work with [gadgetbridge](https://gadgetbridge.org) very well. But keep in mind, bluetooth in standby reduces the battery runtime.

![screenshot](images/bluetooth.png)

## WiFi

![screenshot](images/screen5.png)

## Time

* Enable synchronisation when connect
* Display 12/24 hours
* Select region and location

![screenshot](images/time.png)

## Updates

It is possible to update over the air.

![screenshot](images/update_1.png)
![screenshot](images/update_2.png)

## Utilities

* Format
* Reboot
* Poweroff
* GPS injection

![screenshot](images/utilities.png)

## Sound 

* Enable sound
* Set volume

![screenshot](images/sound.png)

## GPS

![screenshot](images/screen11.png)

## Watchfaces

If you want to customize your own watchface, copy a ![watchface.tar.gz](images/watchface/undone/watchface.tar.gz) to your watch and decompress it with the watchface app.

A `watchface.tar.gz` includes the following files and a extra `watchface_theme.json`. Some example:

![dial](images/watchface/watchface_dial.png)
![hour](images/watchface/watchface_hour.png)
![min](images/watchface/watchface_min.png)
![sec](images/watchface/watchface_sec.png)
![hour shadow](images/watchface/watchface_hour_s.png)
![min shadow](images/watchface/watchface_min_s.png)
![sec shadow](images/watchface/watchface_sec_s.png)

In the file `watchface_theme.json` you will describe the position of information via the `label` or 'image' node. See Cf. [here](WATCHFACE.md) for a node list.
Here you can find some finish watchface packages:

[![swiss clock](images/watchface/swiss/watchface_theme_prev.png)](images/watchface/swiss/watchface.tar.gz)
[![undone clock](images/watchface/undone/watchface_theme_prev.png)](images/watchface/undone/watchface.tar.gz)
[![star trek clock](images/watchface/startrek/watchface_theme_prev.png)](images/watchface/startrek/watchface.tar.gz)
[![rainbow clock](images/watchface/rainbow/watchface_theme_prev.png)](images/watchface/rainbow/watchface.tar.gz)
[![hal9000](images/watchface/hal9000/watchface_theme_prev.png)](images/watchface/hal9000/watchface.tar.gz)
[![black](images/watchface/black/watchface_theme_prev.png)](images/watchface/black/watchface.tar.gz)

alternative [watchfaces](https://github.com/PGNetHun/PG-TTGO-Watchfaces)

# Applications

![screenshot](images/screen7.png)

## weather app

![screenshot](images/screen10.png)

For the weather app you need an openweather.com api-id. http://openweathermap.org/appid is a good starting point.

## Stopwatch

![screenshot](images/stopwatch.png)

Click play to start.

## Alarm

![screenshot](images/alarm.png)

You can set an alarm, by setting time and day(s) of the week.

The main switch controls if alarm is enabled or not.

Next, select the day(s) of the week for the alarm.
Note that if no days are select, it means an every day alarm.
Finally, select the hour and minute for the alarm.

In the settings, you can select the reminder: vibe, fade, beep.
You can also have the next alarm displayed on the main face.

## ir-remote

For customise your ir-codes, use [WConfigurator](https://github.com/anakod/WConfigurator). For an example ir-remote.json configuration file see [here](https://github.com/d03n3rfr1tz3/TTGO.T-Watch.2020/blob/master/conf/ir-remote.json.example).

```json
{
    "pages": [{
            "Power": {
                "m": 7,
                "hex": "E0E040BF"
            },
            .
            .
            .
            "Stop": {
                "m": 7,
                "hex": "E0E0629D"
            }
        }
    ],
    "defBtnHeight": 33,
    "defBtnWidth": 65,
    "defSpacing": 2
}
```

IR-modes supported:

RC5 = 1, 
RC6 = 2,
NEC = 3,
SONY = 4,
PANASONIC = 5,
JVC = 6,
SAMSUNG = 7,
LG = 10,
SHARP = 14,
RAW = 30,
SAMSUNG36 = 56

IR-data format supported:

raw,
hex

## watchface

This application let you download community based watch faces.
Browse watch face with left/right button.
Clic on the icon when you find yours.

Here you can find an overview of all [watchfaces](https://sharandac.github.io/My-TTGO-Watchfaces/) on github.

Note that the information are downloaded in real time (remember to activate WiFi):

* The list of watchfaces.
* The preview of each watch.

## OSMmap

![screenshot](images/screen9.png)
![maps](images/osmmap_maps.png)
![menu](images/osmmap_menu.png)
![navigation](images/osmmap_navigation.png)

A long press in the middle centers the map to the current gps position.

## OSMAnd

![screenshot](images/screen6.png)

In connection with [OsmAnd](https://osmand.net) the watch can also be used for navigation. Please use the osmand app, otherwise a lot of messages will be displayed.

## gps tracker

![screenshot](images/tracker1.png)
![screenshot](images/tracker2.png)

gps tracker that generates .gpx files. Only works properly with watches that have GPS. A long press on the crosshairs starts and stops the logging. The .gpx files can be downloaded via FTP and imported directly into e.g. GoogleMaps or OSM. The trash icon deletes all files to save space.

## gps status

![screenshot](images/gps_status.png)

## astro

![screenshot](images/astro.png)

## powermeter

![screenshot](images/powermeter.png)

Get realtime data from a [powermeter](https://github.com/sharandac/powermeter) over mqtt.

## wfif mon

![screenshot](images/wifi_mon.png)

## Activity tracker

![screenshot](images/screen12.png)

The activity tracker let you check your activity.

In the settings, set your step length and your goals in step and meters.

When associated to Gadgetbrige, activity is reported regularly.
If you need to ensure a synchronization, for example at the beginning of an activity or at the end, you can use the refresh button.
It will force a synchronization.

The trash can button allows to reset step counter.
Useful when starting a new activity an keeping exact track of it.

## Sailing

![screenshot](images/sailing.png)

This app connects to your [OpenPlotter](https://openmarine.net/openplotter) and shows some of your boat stats.

In order to make it work you have to configure your OpenCPN plotter in the connections tab as follows:

![screenshot](images/openplotter.png)

Set "Output filtering" to trasmit the sentences: RMB,RMC,APB

Contact [fliuzzi02](https://github.com/fliuzzi02) for further info and help.
Some improvements might come in the future.

## Kodi Remote

A remote for controlling Kodi. Includes a player tile and a tile for a remote control.

![screenshot](images/kodi_remote_player.png)
![screenshot](images/kodi_remote_control.png)

## Calc

A simple calculator.
Beware that the button C/CE has two functions. A short touch uses CE, which clears only the
recent input. A longer touch uses C, which clears all inputs and basically resets the calculator.

![screenshot](images/calc.png)

# Updates

See `Updates` in settings.

# FAQ

## how to make a screenshot?

![screenshot](images/screen8.png)

Press the button for 2 seconds, after that an quickmenu appears. Here you can select the tiny camera icon to take a screenshot.
This can be downloaded via the built-in FTP server (binary and passive mode, username: TTWatch and password: passord), if activated.
The file name is screen.png.

Or the other way:

The firmware has an integrated webserver. Over this a screenshot can be triggered. The image store as png and can be read with gimp. From bash it look like this
```bash
wget x.x.x.x/shot ; wget x.x.x.x/screen.png
```

Pro-tipp:

[lgrossard](https://github.com/lgrossard)! made a little Python script to generate and download the screenshots from the t-watch [here](https://ludovic.grossard.fr/media/twatch_screenshot.py).

## how to change background?

You can change background in the display settings.

If you want to use your own background image, simply upload a PNG with a resolution of 240x240 pixels via ftp to the Watch and name it bg.png and set it in the display settings page 2.
