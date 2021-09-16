#ifndef _DELAY_H
    #define _DELAY_H

    #ifdef NATIVE_64BIT
        void delay(unsigned int tms);
    #endif
#endif // _DELAY_H