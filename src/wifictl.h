#ifndef _WIFICTL_H
    #define _WIFICTL_H

    #define WIFICTL_DELAY       10
    #define NETWORKLIST_ENTRYS  20

    struct networklist {
        char ssid[64]="";
        char password[64]="";
    };

    /*
     * @brief setup wifi controller routine
     */
    void wifictl_setup( void );
    /*
     * @brief check if networkname known
     * 
     * @param   networkname network name to check
     */
    bool wifictl_is_known( const char* networkname );
    /*
     * @brief insert or add an new ssid/password to the known network list
     * 
     * @param ssid      pointer to an network name
     * @param password  pointer to the password
     * 
     * @return  true if was success or false if fail
     */
    bool wifictl_insert_network( const char *ssid, const char *password );
    /*
     * @brief switch on wifi
     */
    void wifictl_on( void );
    /*
     * @brief switch off wifi
     */
    void wifictl_off( void );

#endif // _WIFICTL_H