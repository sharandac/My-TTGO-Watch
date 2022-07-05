/**
 * BMA423 interrupts
 */
#if defined( VERSION_2 )
    #define BMA_INT_1   32
    #define BMA_INT_2   32
    /**
     * motor Pin
     */
    #define MOTOR_PIN   4
    /**
     * Button pins
     */
    #define BTN_1       0
    #define BTN_2       34
    #define BTN_3       35
    /**
     * I2C pins
     */
    #define IICSCL      25
    #define IICSDA      26
    /**
     * 
     */
    #define RTC_Int     32
    /**
     * touch pins
     */
    #define Touch_Res   33
    #define Touch_Int   32
    /**
     * power control pins
     */
    #define CHARGE      2
    #define PWR_ON      5
    #define BAT_ADC     36
#else
    #define BMA_INT_1   22
    #define BMA_INT_2   39
    /**
     * motor Pin
     */
    #define MOTOR_PIN   4
    /**
     * Button pins
     */
    #define BTN_1       0
    #define BTN_2       34
    #define BTN_3       35
    /**
     * I2C pins
     */
    #define IICSCL      25
    #define IICSDA      26
    /**
     * touch pins
     */
    #define Touch_Res   33
    #define Touch_Int   32
    /**
     * power control pins
     */
    #define CHARGE      2
    #define PWR_ON      5
    #define BAT_ADC     36

#endif
