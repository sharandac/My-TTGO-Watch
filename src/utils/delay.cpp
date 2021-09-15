#ifdef NATIVE_64BIT
    #include "delay.h"
    #include <unistd.h>

    void delay(unsigned int tms) {
        usleep(tms * 1000);
    }
#endif