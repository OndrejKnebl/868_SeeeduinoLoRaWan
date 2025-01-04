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
 * Average module internal temperature, average battery voltage, battery status and CayenneLPP, 20. 12. 2023
 *******************************************************************************/
#include <SeeeduinoLoRaWan.h>
LoRaWanClass lora;

#include <RTCZero.h>                  // RTCZero
RTCZero rtc;

#include <CayenneLPP.h>               // Cayenne Low Power Payload (LPP)
CayenneLPP lpp(51);                   // https://lora.vsb.cz/index.php/cayenne-lpp/


// Comment out the line #define PRINT_TO_SERIAL_MONITOR
// in "C:\Users\User\Documents\Arduino\libraries\SeeeduinoLoRaWan\SeeeduinoLoRaWan.h"
// to disable printing to Serial Monitor.


//------------------- Sleep time -------------------
const byte hours = 0;
const byte minutes = 1;
const byte seconds = 0;
//---------------------------------------------------

//-------------- Here change your keys --------------
#define APP_EUI "0000000000000000"
#define DEV_EUI "0000000000000000"
#define APP_KEY "00000000000000000000000000000000"
//---------------------------------------------------


// Sleep counter
unsigned int countSleeps = 1;                 // Counting sleeps
unsigned int sendDataEvery = 5;               // Send data every 5 sleep

// Seeeduino
float batVoltage = 0.0;                       // Variable for battery voltage
bool batStatus = false;                       // Variable for battery status
float moduleTemp = 0.0;                       // Variable for module temperature
float numberOfSamples = 0.0;                  // Variable for number of measured samples

bool isDelay = true;
//------------------------------------------------------------------------------


void resetValues() {              // Reset values
    batVoltage = 0.0;
    moduleTemp = 0.0;
    numberOfSamples = 0.0;
    batStatus = false;
}


void measureValues() {
    batVoltage += lora.getBatteryVoltage();                 // Add the current battery voltage to all measured battery voltages
    moduleTemp += lora.getModuleTemperatureC();             // Add the current module temperature to all measured module temperatures
    numberOfSamples++;                                      // Add measured sample

    batStatus = lora.getBatteryStatus();                    // Get battery status. Return false while charging, return true while charge done or no battery insert.
}


void setSleepAndWakeUp() {
    lora.setDeviceLowPower();
    rtc.setTime(0, 0, 0);
    rtc.setAlarmTime(hours, minutes, seconds);
    rtc.standbyMode();                                      // Standby
    lora.setDeviceLowPowerWakeUp();
}


void sendAndReceiveData() {
    lpp.reset();
    lpp.addTemperature(1, moduleTemp / numberOfSamples);                // Add the average module temperature into channel 1
    lpp.addVoltage(2, batVoltage / numberOfSamples);                    // Add the average battery voltage into channel 2
    lpp.addDigitalInput(3, (uint8_t)batStatus);                         // Add the battery status into channel 3

    bool result = lora.transmitPacket(lpp.getBuffer(), lpp.getSize());  // Prepare upstream data transmission at the next possible time

    resetValues();                                                      // Reset values


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

    setSleepAndWakeUp();

    measureValues();

    if(countSleeps % sendDataEvery == 0){             // Every 5 sleep
        checkJoin(10);
        sendAndReceiveData();
        countSleeps = 0;                              // Zero sleep counter
    }
    countSleeps++; 
}
