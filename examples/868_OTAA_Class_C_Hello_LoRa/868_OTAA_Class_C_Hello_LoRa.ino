/*******************************************************************************
 * The Things Network - Seeeduino - EU868, Class C, OTAA
 * 
 * Copyright (c) 2024 Ondřej Knebl, LoRa@VSB
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 * 
 * Hello, LoRa!, 27. 10. 2024
 *******************************************************************************/

#include <SeeeduinoLoRaWan.h>
LoRaWanClass lora;

// Comment out the line #define PRINT_TO_SERIAL_MONITOR
// in "C:\Users\User\Documents\Arduino\libraries\SeeeduinoLoRaWan\SeeeduinoLoRaWan.h"
// to disable printing to Serial Monitor.

//-------------- Here change your keys --------------
#define APP_EUI "0000000000000000"
#define DEV_EUI "0000000000000000"
#define APP_KEY "00000000000000000000000000000000"
//---------------------------------------------------

const unsigned TX_INTERVAL = 300;                                 // Transmission interval in seconds
static uint8_t mydata[] = "Hello, LoRa!";

// Timer
unsigned long previousMillis = 0;                                 // Previous time
const long interval = 1000;                                       // Interval 1000 ms
unsigned int countSeconds = 0;                                    // Counting seconds

bool isDelay = true;


void receiveData() {
    short length;
    char buffer[256];
    short rssi;

    memset(buffer, 0, 256);
    length = lora.receivePacket(buffer, 256, &rssi);
    
    if(length) {
        SerialUSB.print("Length: ");
        SerialUSB.println(length);
        SerialUSB.print("RSSI: ");
        SerialUSB.println(rssi);
        SerialUSB.print("Data: ");
        for(unsigned char i = 0; i < length; i ++) {
            SerialUSB.print("0x");
            SerialUSB.print((buffer[i] >> 4) & 0x0F, HEX);
            SerialUSB.print(buffer[i] & 0x0F, HEX);
            SerialUSB.print(" ");
        }
        SerialUSB.println();
    }
}


void sendAndReceiveData() {
  
    SerialUSB.println("Sending - Hello, LoRa!");
    bool result = lora.transmitPacket(mydata, sizeof(mydata)-1);

    if(result) {
        SerialUSB.println("Data sent successfully!");
        receiveData();
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

    SerialUSB.begin(9600);
    //while(!SerialUSB);

    lora.setDeviceDefault();
    lora.getVersion();
    lora.setActivation(LWOTAA);
    lora.setKeysOTAA(APP_EUI, DEV_EUI, APP_KEY);
    lora.setEU868();
    lora.setClassType(CLASS_C);
    lora.setPort(1);

    checkJoin(10);
}


void loop(void) {

    unsigned long currentMillis = millis();                       // Current millis

    if(currentMillis - previousMillis >= interval) {              // Timer set to 1 second
        previousMillis = currentMillis;

        checkJoin(10);

        receiveData();
        
        if(countSeconds % TX_INTERVAL == 0){                      // Every 300 seconds
            sendAndReceiveData();                                 // Call sendAndReceiveData
            countSeconds = 0;                                     // Zero seconds counter
        }
        countSeconds++;                                           // + 1 second
    }
}
