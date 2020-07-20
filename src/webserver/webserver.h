#ifndef _ASYNCWEBSERVER_H

    #define _ASYNCWEBSERVER_H

    #define WEBSERVERPORT 80

    /*
     *  @brief setup builtin webserver, call after first wifi-connection. otherwise esp32 will crash
     */
    void asyncwebserver_setup(void);

#endif // _ASYNCWEBSERVER_H