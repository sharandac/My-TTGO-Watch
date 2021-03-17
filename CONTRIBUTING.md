## Applications

For quick clock application development use the new QuickGLUI - high level API. See [here](https://github.com/sharandac/My-TTGO-Watch/pull/163).

### Activate/Hibernate

To run actions when application is activated or hibernatedn use `mainbar_add_tile_activate_cb(...)` and `mainbar_add_tile_hibernate_cb(...)`.
Thess callbacks are called when entering or exiting a Tile ID. By example:

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
#include "hardware/json_psram_allocator.h"
```

and create your json with

```c
SpiRamJsonDocument doc( 1000 );
```

to move your json into PSRAM, here is enough RAM for all the crazy stuff you will do. And use

```c
#include "hardware/alloc.h"
```
with
```c
MALLOC( ... ); // repleace malloc or ps_malloc
CALLOC( ... ); // repleace calloc or ps_calloc
REALLOC( ... ); // repleace realloc or ps_realloc
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
