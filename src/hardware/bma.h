#ifndef _BMA_H
    #define _BMA_H
    
    #define     BMA_EVENT_INT   _BV(0)

    typedef struct {
        bool enable=true;
    } bma_config_t;

    enum {  
        BMA_STEPCOUNTER,
        BMA_DOUBLECLICK,
        BMA_CONFIG_NUM
    };

    #define BMA_COFIG_FILE  "/bma.cfg"

    void bma_setup( TTGOClass *ttgo );
    void bma_loop( TTGOClass *ttgo );

    void bma_reload_settings( void );
    void bma_save_config( void );
    void bma_read_config( void );
    bool bma_get_config( int config );
    void bma_set_config( int config, bool enable );

#endif // _BMA_H