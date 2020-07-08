#ifndef _BMA_H
    #define _BMA_H
    
    #define     BMA_EVENT_INT   _BV(0)

    void bma_setup( TTGOClass *ttgo );
    void bma_loop( TTGOClass *ttgo );

#endif // _BMA_H