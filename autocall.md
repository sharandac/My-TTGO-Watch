# the magic behind autocall_function

How do I make sure that a function is called automatically without being called explicitly in the setup() function in main.c?
Actually this is quite simple. In the file src/gui/app/app.h is a function and type declared which looks like this:

```c
typedef void ( * APP_AUTOCALL_FUNC ) ( void );
int app_autocall_function( APP_AUTOCALL_FUNC function, size_t prio );
```

You pass a function pointer to this function. In this case the registration function ( ```app_setup_function()```) for our app. So far not very exciting.
The trick is to call this function in another file and store the return value in a static variable that has the file as scope. This ensures that this function is called at initialization before the actual program starts. We only need to store this function pointer in a list for later use like.

So only the following code snippet has to be in out app:
```c
#include "gui/app.h"

static int registed = app_autocall_function( &app_setup_function, 0 );
```
Later in the program we can simply process the list with functions pointers at a suitable place.
That was it already. Simple if you think about it.