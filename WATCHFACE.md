# watchface.tar.gz
This compressed file containts all file as a package to downoad and run a watchface. all image and fontfiles must are included here. a minimum watchface.tar.gz included a watchface_theme.json. from here all fancy things begins.

# watchface_theme.json overview
file format
```json
{
	"api_level": 2,
	"dial": {
		"enable": true,
		"x_offset": 0,
		"y_offset": 0
	},
	...
	"label": [
		...
	],
	"image": [
		...
	]
}
```
# dial, hour, min, sec, hour_shadow, min_shadow and sec_shadow

A dial, hour and so on has the fowllowing three options:

`enable`:		true or false
`x_offset`:		x postion on the screen in pixel. Top is 0.
`y_offset`:		y position on the screen in pixel. Left is 0.

Remember, a center to the screen and the rotation center is in the middle of the image.
```json
"dial": {
	"enable": true,
	"x_offset": 0,
	"y_offset": 0
},
"hour": {
	"enable": true,
	"x_offset": 0,
	"y_offset": 0
},
...
```
The image name convention is `watchface_dial.png`, `watchface_hour.png` and so on and will automaticly loaded if enabled. No entry mean disabled.

# label
A label entry looks like this:
```json
"label": [
	{
		"enable": true,
		"type": "date",
		"label": "%H %M",
		"font": "LCD",
		"font_color": "#304e65",
		"font_size": 48,
		"align": "center",
		"hide_interval": -1,
		"x_offset": 38,
		"y_offset": 118,
		"x_size": 134,
		"y_size": 40
	},
	{
		"enable": true,
		"type": "date",
		"label": "%H:%M",
		"font": "LCD",
		"font_color": "#304e65",
		"font_size": 48,
		"align": "center",
		"hide_interval": 1,
		"x_offset": 38,
		"y_offset": 118,
		"x_size": 134,
		"y_size": 40
	},
	{
	....
	}
]
```
|option|value type|discription|
|--|--|--|
|`enable`|bool|true or false
|`type`|string|see label types
|`label`|string|label string
|`font`|string|font name or a font filename. the font filename ends with `.font`. see font types and size.
|`font_color`|string|font color like "#808080" for pue grey, format is "#rrggbb" in hex
|`font_size`|string|font size in pixel, see font types and size
|`align`|string|`center`, `left` and `right`
|`hide_interval`|int|0 mean not used. positive values hide this label in a show/hide intveral. and negative values is hide/show.
|`x_offset`|int|x postion on the screen in pixel. Top is 0.
|`y_offset`|int|y position on the screen in pixel. Left is 0.
|`x_size`|int|x container size on the screen in pixel.
|`y_size`|int|y container size on the screen in pixel.

The example above schow how a blinking time works.

## label types


|label type|discription|
|--|--|
|`text`|a raw text.|
|`date`|an information related to date/time. See `strftime` function for format.|
|`battery_percent`|the battery percent as an integer. Example: `"%d%%"` for `87%`.|
|`battery_voltage`|the battery voltage, as a float. Example: `"%.2fV"` for `1.34V`.|
|`bluetooth_messages`|the number of messages, as an integer. Example: `"msg: %d"` for `msg: 2`|
|`steps`|the number of steps, as an integer.|

## font type and size
Available font are: `Ubuntu`, `LCD`.
Available font sizes are: `12`, `16`, `32`, `48`, `72`.
On api_level 3 it is possible to load a font on runtime from spiffs. As example: `lcd.font` loads a font file from spiffs. location is: `/spiffs/watchface/lcd.font`
[here is a manual how create your own font file](https://github.com/lvgl/lv_font_conv/)

# image
A label entry looks like this:
```json
"image": [
	{
		"enable": true,
		"type": "time_sec",
		"file": "klingon_yellow.png",
		"hide_interval": 0,
		"rotation_range": 359,
		"rotation_start": 0,
		"rotation_x_origin": 8,
		"rotation_y_origin": 26,
		"x_offset": 135,
		"y_offset": 115,
		"x_size": 52,
		"y_size": 52
	},
	{
		"enable": true,
		"type": "time_sec",
		"file": "klingon_red.png",
		"hide_interval": 0,
		"rotation_range": 359,
		"rotation_start": 0,
		"rotation_x_origin": 8,
		"rotation_y_origin": 26,
		"x_offset": 165,
		"y_offset": 105,
		"x_size": 52,
		"y_size": 52
	}
]
```
|option|value type|discription|
|--|--|--|
|`enable`|bool|true or false
|`type`|string|see image types
|`file`|string|image filename. Example: `"klingon_red.png"` loads a png from spiffs on location `/spiffs/watchface/klingon_red.png`.
|`hide_interval`|int|0 mean not used. positive values hide this label in a show/hide intveral. negative values in a hide/show interval.
|`rotaion_range`|int|ratation range in degree.
|`rotaion_start`|int|ratation start angle in degree.
|`rotation_x_origin`|int|x origin for rotation center in pixel. top is 0.
|`rotation_y_origin`|int|y origin for rotation center in pixel. left is 0.
|`x_offset`|int|x postion on the screen in pixel. Top is 0.
|`y_offset`|int|y position on the screen in pixel. Left is 0.
|`x_size`|int|x container size on the screen in pixel.
|`y_size`|int|y container size on the screen in pixel.

## image types
|image type|discription|
|--|--|
|`image`|a raw image without interaction.|
|`battery_percent`|`rotaion_range` divide by battery percent and add to `rotaion_start`, battery range is 0-100.|
|`battery_voltage`|`rotaion_range` divide by battery voltage and add to `rotaion_start`. voltage range is 0-5.|
|`time_hour`|`rotaion_range` divide by current hour and add to `rotaion_start`. hour range is 0-23|
|`time_min`|`rotaion_range` divide by current min and add to `rotaion_start`. min range is 0-59|
|`time_sec`|`rotaion_range` divide by current min and add to `rotaion_start`. hour range is 0-59|

