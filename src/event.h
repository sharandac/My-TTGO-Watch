#ifndef _EVENT_H

    #define _EVENT_H
    #define G_EVENT_VBUS_PLUGIN         _BV(0)
    #define G_EVENT_VBUS_REMOVE         _BV(1)
    #define G_EVENT_CHARGE_DONE         _BV(2)

    #define G_EVENT_WIFI_SCAN_START     _BV(3)
    #define G_EVENT_WIFI_SCAN_DONE      _BV(4)
    #define G_EVENT_WIFI_CONNECTED      _BV(5)
    #define G_EVENT_WIFI_BEGIN          _BV(6)
    #define G_EVENT_WIFI_OFF            _BV(7)

    enum {
        Q_EVENT_WIFI_SCAN_DONE,
        Q_EVENT_WIFI_CONNECT,
        Q_EVENT_BMA_INT,
        Q_EVENT_AXP_INT,
    } ;

    #define DEFAULT_SCREEN_TIMEOUT  30*1000

    #define WATCH_FLAG_SLEEP_MODE   _BV(1)
    #define WATCH_FLAG_SLEEP_EXIT   _BV(2)
    #define WATCH_FLAG_BMA_IRQ      _BV(3)
    #define WATCH_FLAG_AXP_IRQ      _BV(4)

    QueueHandle_t g_event_queue_handle = NULL;
    EventGroupHandle_t isr_group = NULL;
    bool lenergy = false;

#endif // _EVENT_H