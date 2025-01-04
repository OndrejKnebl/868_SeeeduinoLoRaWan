/*******************************************************************************
 * The Things Network - Seeeduino - EU868, Class A, OTAA, Low Power
 * 
 * Copyright (c) 2023 Ondřej Knebl, LoRa@VSB
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 * 
 * Hello, LoRa!, 20. 12. 2023
 *******************************************************************************/
#include <SeeeduinoLoRaWan.h>
LoRaWanClass lora;

#include <RTCZero.h>                  // RTCZero
RTCZero rtc;

// Comment out the line #define PRINT_TO_SERIAL_MONITOR
// in "C:\Users\User\Documents\Arduino\libraries\SeeeduinoLoRaWan\SeeeduinoLoRaWan.h"
// to disable printing to Serial Monitor.

//------------------- Sleep time -------------------
const byte hours = 0;
const byte minutes = 5;
const byte seconds = 0;
//---------------------------------------------------

//-------------- Here change your keys --------------
#define APP_EUI "0000000000000000"
#define DEV_EUI "0000000000000000"
#define APP_KEY "00000000000000000000000000000000"
//---------------------------------------------------

static uint8_t mydata[] = "Hello, LoRa!";

bool isDelay = true;


void setSleepAndWakeUp(){
    lora.setDeviceLowPower();
    rtc.setTime(0, 0, 0);
    rtc.setAlarmTime(hours, minutes, seconds);
    rtc.standbyMode();                          // Standby
    lora.setDeviceLowPowerWakeUp();
}


void sendAndReceiveData() {
    bool result = lora.transmitPacket(mydata, sizeof(mydata)-1);

    if(result) {
        short length;
        char buffer[256];
        short rssi;

        memset(buffer, 0, 256);
        length = lora.receivePacket(buffer, 256, &rssi);
    }
}


void checkJoin(unsigned char timeout) {
    while(true) {
        if(lora.setOTAAJoin(JOIN, timeout)) {
            if(isDelay) {
                delay(15000);
            }
            isDelay = false;
            break;
        } else {
            isDelay = true;
        }
    }
}


void setup(void) {

    lora.init();
    lora.setDeviceReset();
    lora.getVersion();
    lora.setActivation(LWOTAA);
    lora.setKeysOTAA(APP_EUI, DEV_EUI, APP_KEY);
    lora.setEU868();
    lora.setClassType(CLASS_A);
    lora.setPort(1);

    checkJoin(10);

    rtc.begin();                        // RTCZero
    rtc.enableAlarm(rtc.MATCH_MMSS);
}


void loop(void) {
    
    checkJoin(10);

    sendAndReceiveData();

    setSleepAndWakeUp();
}
