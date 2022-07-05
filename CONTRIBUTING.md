## Install

Clone this repository and open it with platformIO. Build and upload. On a terminal in vscode you can do it with

```bash
pio run -t upload
```
or simple press "build and upload" in platformIO, 
or follow the great step by step [tutorial](https://www.youtube.com/watch?v=wUGADCnerCs) from ShotokuTech[ShotokuTech](https://github.com/ShotokuTech).

Please pay attention to which version you have for the T-Watch 2020 and T-Watch-2021!

## Applications

For quick clock application development use the new QuickGLUI - high level API. See [here](https://github.com/sharandac/My-TTGO-Watch/pull/163).
Or follow the [My-TTGO-Watch-Dev](https://github.com/linuxthor/My-TTGO-Watch-Dev) instruction from [linuxthor](https://github.com/linuxthor) to use native LVGL and the present framework. Native LVGL is more powerful but also more difficult to learn. And don't forget to take a look in to inline documentation and the online [LVGL](https://docs.lvgl.io/v7/en/html/) documentation.

### Activate/Hibernate

To run actions when application is activated or hibernatedn use `mainbar_add_tile_activate_cb(...)` and `mainbar_add_tile_hibernate_cb(...)`.
These callbacks are called when entering or exiting a Tile ID. For example:

```C
#include "gui/mainbar/mainbar.h"
#include "quickglui/quickglui.h"

LV_IMG_DECLARE( foo_64px);
Application foo;

void foo_activation_cb( void );
void foo_hibernate_cb( void );

void foo_setup( void ) {
   foo.init( "foo", &foo_64px );
   mainbar_add_tile_activate_cb( foo.mainTileId(), foo_activate_cb );
   mainbar_add_tile_hibernate_cb( foo.mainTileId(), foo_hibernate_cb );
}

void foo_activation_cb( void ) {
    // do something when enter the tile
}

void foo_hibernate_cb( void ) {
    // do something when exit the tile
}
```

## RAM

Internal RAM is very limited, use PSRAM as much as possible. When you work with ArduinoJson, include this

```c
#include "utils/json_psram_allocator.h"
```

and create your json with

```c
SpiRamJsonDocument doc( 1000 );
```

to move your json into PSRAM, here is enough RAM for all the crazy stuff you will do. And use

```c
#include "utils/alloc.h"
```
with
```c
MALLOC( ... );      // repleace malloc or ps_malloc
CALLOC( ... );      // repleace calloc or ps_calloc
REALLOC( ... );     // repleace realloc or ps_realloc
```

or the save version with assert

```c
MALLOC_ASSERT( size, format, ... );         // like printf output on fail
CALLOC_ASSERT( nmemb, size, format, ... );  // like printf output on fail
REALLOC_ASSERT( size, format, ... );        // like printf output on fail
```

as often as possible. It managed internal or PSRAM for you.
And one very important thing: Do not talk directly to the hardware!

## Sound
To play sounds from the inbuild speakers use `hardware/sound.h`:

```c
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

## Gadgetbridge

In order to debug features, it could be useful to access raw data stored on Gadgetbridge.
On Gadgetbridge, go to Export/Import feature.

The `Gadgetbridge` file is a SQLite3 database.
Open it with `sqlite3 Gadgetbridge`.

### Step counter

As the firmware reuse the Espruino / BangleJS, the activity is stored in `BANGLE_JSACTIVITY_SAMPLE` table.

```SQL
select datetime(TIMESTAMP, 'unixepoch'),device_id,steps from BANGLE_JSACTIVITY_SAMPLE;
```
