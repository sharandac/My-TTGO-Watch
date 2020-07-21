#ifndef _MOTOR_H
    #define _MOTOR_H

    #include "TTGO.h"

    /*
     * @ brief setup motor I/O
     */
    void motor_setup( void );
    /*
     * @brief let vibe motor for n * 10ms
     * @param   time    time in 10ms
     */
    void motor_vibe( int time );

#endif // _MOTOR_H