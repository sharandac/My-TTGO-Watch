# Main screen

![screenshot](images/screen1.png)

On startup you see the main screen (time tile). It show the time and widgets.

Widgets are:

* the current weather (if correctly configured).
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

![screenshot](images/screen9.png)

## Display

Set color, background, touch feedback with vibrations...

## Move

Enable:

* step counter
* double click
* tilt
* display of step counter

## Bluetooth

The bluetooth notification work with [gadgetbridge](https://gadgetbridge.org) very well. But keep in mind, bluetooth in standby reduces the battery runtime.

## WiFi

![screenshot](images/screen5.png)

## Time

* Enable synchronisation when connect
* Display 12/24 hours
* Select region and location

## Utilities

* Format
* Reboot
* Poweroff

## Sound 

* Enable sound
* Set volume

# Applications

![screenshot](images/screen7.png)

## weather app

![screenshot](images/screen10.png)

For the weather app you need an openweather.com api-id. http://openweathermap.org/appid is a good starting point.

## Stopwatch

Click play to start.

## Alarm

You can set an alarm, by setting time and day(s) of the week.

## ir-remote

For customise your ir-codes, use [WConfigurator](https://github.com/anakod/WConfigurator).

## OSMAnd

![screenshot](images/screen6.png)

In connection with [OsmAnd](https://osmand.net) the watch can also be used for navigation. Please use the osmand app, otherwise a lot of messages will be displayed.
 
# Updates

![screenshot](images/screen11.png)

# FAQ

## how to make a screenshot?

![screenshot](images/screen8.png)

In the display settings page 2 you can set what happens when the button is pressed for 2 seconds.
This will normally takes you back to the main tile.
If the option for a screenshot is activated, a screenshot is taken instead.
This can be downloaded via the built-in FTP server (binary and passive mode, username: TTWatch and password: passord), if activated.
The file name is screen.data.

Or the other way:

The firmware has an integrated webserver. Over this a screenshot can be triggered. The image has the format RGB565 and can be read with gimp. From bash it look like this
```bash
wget x.x.x.x/shot ; wget x.x.x.x/screen.565
```

## how to change background?

You can change background in the display settings.

If you want to use your own background image, simply upload a PNG with a resolution of 240x240 pixels via ftp to the Watch and name it bg.png and set it in the display settings page 2.