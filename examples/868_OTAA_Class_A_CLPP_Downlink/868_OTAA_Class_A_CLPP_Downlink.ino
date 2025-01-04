/*******************************************************************************
 * The Things Network - Seeeduino - EU868, Class A, OTAA, Downlink reception
 * 
 * Copyright (c) 2024 Ondřej Knebl, LoRa@VSB
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 * 
 * Hello, LoRa!, 14. 12. 2024
 *******************************************************************************/

#include <SeeeduinoLoRaWan.h>
LoRaWanClass lora;

#include <CayenneLPP.h>               // Cayenne Low Power Payload (LPP)
CayenneLPP lpp(51);                   // https://lora.vsb.cz/index.php/cayenne-lpp/


// Comment out the line #define PRINT_TO_SERIAL_MONITOR
// in "C:\Users\User\Documents\Arduino\libraries\SeeeduinoLoRaWan\SeeeduinoLoRaWan.h"
// to disable printing to Serial Monitor.

//-------------- Here change your keys --------------
#define APP_EUI "0000000000000000"
#define DEV_EUI "0000000000000000"
#define APP_KEY "00000000000000000000000000000000"
//---------------------------------------------------

unsigned TX_INTERVAL = 300;                                       // Transmission interval in seconds

// Timer
unsigned long previousMillis = 0;                                 // Previous time
const long interval = 1000;                                       // Interval 1000 ms
unsigned int countSeconds = 0;                                    // Counting seconds

// Seeeduino
float batVoltage = 0.0;                                           // Variable for battery voltage
bool batStatus = false;                                           // Variable for battery status
float moduleTemp = 0.0;                                           // Variable for module temperature
float numberOfSamples = 0.0;                                      // Variable for number of measured samples

bool isDelay = true;
//------------------------------------------------------------------------------

void printDebugData2(JsonObject root){
    serializeJsonPretty(root, Serial);
    Serial.println();
    Serial.println(root.size());
}


void downlinkSetSendTime(JsonObject root){                              // Downlink - Send data every
    if(root.containsKey("time_1")){                                     // If JsonObject contains "time_1"
        uint32_t time_1 = root["time_1"];                               // Get value of the key "time_1" and store it to the variable time_1
        if(time_1 >= 60 && time_1 <= 300){                              // 60 - 300 seconds
            TX_INTERVAL = time_1;                                       // Setting the time_1 value to the TX_INTERVAL variable used in the program
        }else{                                                          // If value is not in range => Error
            Serial.println("Time Error!!!");
        }
    }
}


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
      

        DynamicJsonDocument jsonBuffer(1024);                                 // A DynamicJsonDocument and JsonObject
        JsonObject root = jsonBuffer.to<JsonObject>();

        lpp.decodeTTN(reinterpret_cast<uint8_t*>(buffer), length, root);      // Decoding data from Cayenne LPP format to JSON root object using lpp.decodeTTN() function
        printDebugData2(root);

        if(root.size() != 0){
            downlinkSetSendTime(root);
        }

        jsonBuffer.clear();                                                   // Finally, the jsonBuffer memory is freed.
    }
}


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


void sendAndReceiveData() {
    lpp.reset();
    lpp.addTemperature(1, moduleTemp / numberOfSamples);                // Add the average module temperature into channel 1
    lpp.addVoltage(2, batVoltage / numberOfSamples);                    // Add the average battery voltage into channel 2
    lpp.addDigitalInput(3, (uint8_t)batStatus);                         // Add the battery status into channel 3

    bool result = lora.transmitPacket(lpp.getBuffer(), lpp.getSize());  // Prepare upstream data transmission at the next possible time

    resetValues();                                                      // Reset values

    receiveData();
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
    lora.setClassType(CLASS_A);
    lora.setPort(1);

    checkJoin(10);
}


void loop(void) {

    unsigned long currentMillis = millis();                       // Current millis

    if(currentMillis - previousMillis >= interval) {              // Timer set to 1 second
        previousMillis = currentMillis;

        checkJoin(10);

        if(countSeconds % 10 == 0){                               // Every 10 seconds
            measureValues();                                      // Call measureValues
        }
        
        if(countSeconds % TX_INTERVAL == 0){                      // Every 300 seconds
            sendAndReceiveData();                                 // Call sendAndReceiveData
            countSeconds = 0;                                     // Zero seconds counter
        }
        countSeconds++;                                           // + 1 second
    }
}
