## Applications

For quick clock application development use the new QuickGLUI - high level API. See [here](https://github.com/sharandac/My-TTGO-Watch/pull/163).

Internal RAM is very limited, use PSRAM as much as possible. When you work with ArduinoJson, include this

```#include "hardware/json_psram_allocator.h"```

and create your json with

```SpiRamJsonDocument doc( 1000 );```

to move your json into PSRAM, here is enough RAM for all the crazy stuff you will do. And use

```
#include "hardware/alloc.h"
```
with
```
MALLOC( ... ); // repleace malloc or ps_malloc
CALLOC( ... ); // repleace calloc or ps_calloc
REALLOC( ... ); // repleace realloc or ps_realloc
```

as often as possible. It managed internal or PSRAM for you.
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
