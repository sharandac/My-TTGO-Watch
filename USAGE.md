# how to use

## weather app

On startup you see the main screen (time tile). It show the time and the current weather (if correct configure). Now you can swipe with you fingers up, down, left and right between the four main screens. The four screens are organized in time, apps, note and setup tile.
For the weather app you need an openweather.com api-id. http://openweathermap.org/appid is a good starting point.

## ir-remote

For customise your ir-codes, use [WConfigurator](https://github.com/anakod/WConfigurator).

## bluetooth

The bluetooth notification work with [gadgetbridge](https://gadgetbridge.org) very well. But keep in mind, bluetooth in standby reduces the battery runtime. In connection with [OsmAnd](https://osmand.net) the watch can also be used for navigation. Please use the osmand app, otherwise a lot of messages will be displayed.

# how to make a screenshot

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
