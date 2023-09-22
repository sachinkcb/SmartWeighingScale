#ifndef BSP_H
#define BSP_H

#include <stdint.h>
#include <string.h>
#include <Wire.h>
#include "time.h"
#include "PCF85063A.h"
#include "DISPLAY.h"

#define RELAYON     1
#define RELAYOFF    0

/*LED GPIO PINS DEFINITIONS*/
#define BSP_LED_1    2

/*Button State for detection*/
#define BUTTON_IDLE 0xff
#define BUTTONUP 0
#define BUTTONSEL 1
#define BUTTONDOWN 2
#define BUTTONM1ON 3
#define BUTTONM1OFF 4

/*BUTTON GPIO PINS DEFINITIONS*/
#define BSP_BTN_1           39  //Up
#define BSP_BTN_2           36 //Down
#define BSP_BTN_3           34 //Slect
#define BSP_BTN_4           15
#define BSP_BTN_5           4


/*I2C GPIO PINS DEFINITIONS*/
#define BSP_SDA             23
#define BSP_SCL             22


/*Relay  PINS DEFINITIONS*/
#define BSP_RELAY1          13

/*ADC input for battery Monitoring*/
#define BSP_WDT             12
#define WDT_TIMEOUT         60

/*DISPLAY GPIO PINS DEFINITIONS*/
#define BSP_DISP_CLK        26 
#define BSP_DISP_DATA       33
#define BSP_DISP_LATCH      25

class cBsp
{
    public:
        bool m_bIsRtcSynced;
        float pcbMake;
        int hooterPin;
        int enterButtonPin;       
    
        /*Construct*/
        cBsp(void);
        /*Destruct*/
        ~cBsp(void);
        /*Functions*/
        void setPcbMake(float);
        
        void turnOnModem(void);
        void turnOffModem(void);
        void OnOffRelay(uint8_t relayNo,uint8_t state);
        void gpioInitialization(void);
        void i2cInitialization(void);
        // void RTCInit(PCF85063A* );
        void spiInitialization(void);
        void wdtPinToggle(void);
        void indLedToggle(void);
        void indLedOff(void);
        uint8_t getButtonEvent(void);
        int syncRTCTime(cPCF85063A*, time_t, time_t);
        time_t ConvertEpoch(cPCF85063A* LoclRTC);
        void displayPinsInitialization(cMYDISPLAY* );
        void ioPinWrite(uint8_t , bool );
        uint8_t ioPinRead(uint8_t pin);
        void wdtInit(void);
        void wdtAdd(TaskHandle_t );
        void wdtfeed(void);
        uint8_t getBoardPowerStatus(void);
        uint8_t getGeneratorStatus(void);
        void hooterOn(void);
        void hooterOff(void);
};

#endif
