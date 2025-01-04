/*
  SeeeduinoLoRaWan.h
  2013 Copyright (c) Seeed Technology Inc.  All right reserved.

  Author: Wayne Weng
  Date: 2016-10-17

  add rgb backlight fucnction @ 2013-10-15
  
  The MIT License (MIT)

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.1  USA

  Modified for LoRa@VSB by Ondřej Knebl, 9. 12. 2024
*/

#ifndef _SEEEDUINOLORAWAN_H_
#define _SEEEDUINOLORAWAN_H_


#include <Arduino.h>


#define SerialLoRa          Serial1

#define PRINT_TO_SERIAL_MONITOR

#define _DEBUG_SERIAL_      1
#define DEFAULT_TIMEOUT     5 // second
#define DEFAULT_TIMEWAIT    100 // millisecond
#define DEFAULT_DEBUGTIME   100 // millisecond

#define BATTERY_POWER_PIN    A4
#define CHARGE_STATUS_PIN    A5

#define BEFFER_LENGTH_MAX    256


enum _class_type_t { CLASS_A = 0, CLASS_B, CLASS_C };
enum _physical_type_t { EU868 = 0 };
enum _device_mode_t { LWABP = 0, LWOTAA, TEST };
enum _otaa_join_cmd_t { JOIN = 0, FORCE };
enum _window_delay_t { RECEIVE_DELAY1 = 0, RECEIVE_DELAY2, JOIN_ACCEPT_DELAY1, JOIN_ACCEPT_DELAY2 };
enum _band_width_t { BW125 = 125, BW250 = 250, BW500 = 500 };
enum _spreading_factor_t { SF12 = 12, SF11 = 11, SF10 = 10, SF9 = 9, SF8 = 8, SF7 = 7 };
enum _data_rate_t { DR0 = 0, DR1, DR2, DR3, DR4, DR5, DR6, DR7 };

/******************************************************************
Type    DataRate    Configuration   BitRate| TxPower Configuration 
EU868   0           SF12/125 kHz    250    | 0       20dBm
        1           SF11/125 kHz    440    | 1       14dBm
        2           SF10/125 kHz    980    | 2       11dBm
        3           SF9 /125 kHz    1760   | 3       8 dBm
        4           SF8 /125 kHz    3125   | 4       5 dBm
        5           SF7 /125 kHz    5470   | 5       2 dBm
        6           SF7 /250 kHz    11000  | 6:15    RFU
        7           FSK:50 kbps     50000  | 
        8:15        RFU                    | 
*******************************************************************/


class LoRaWanClass
{
    public:
    
        LoRaWanClass(void);

        /**
         *  \brief Initialize the conmunication interface
         *  
         *  \return Return null
         */
        void init(void);

        /**
         *  \brief Set frequency plan Europe 863-870 MHz (SF9 for RX2 - recommended)
         *  
         *  \return Return null.
         */
        void setEU868(void);

        /**
         *  \brief Read the version from device
         *  
         *  \param [in] *buffer The output data cache
         *  \param [in] length The length of data cache
         *  \param [in] timeout The over time of read
         *  
         *  \return Return null.
         */
        void getVersion(void);

        /**
         *  \brief Read the ID from device
         *  
         *  \param [in] *buffer The output data cache
         *  \param [in] length The length of data cache
         *  \param [in] timeout The over time of read
         *  
         *  \return Return null.
         */
        void getId(void);

        /**
         *  \brief Set the OTAA keys
         * 
         * \param [in] *AppEUI The application identifier
         * \param [in] *DevEUI The end-device identifier
         * \param [in] *AppKey The Application key
         * 
         * \return Return null.
         */
        void setKeysOTAA(char *AppEUI, char *DevEUI, char *AppKey);

        /**
         *  \brief Set the ABP keys
         * 
         * \param [in] *DevAddr The end-device address
         * \param [in] *NwkSKey The network session key
         * \param [in] *AppSKey The application session key
         * 
         * \return Return null.
         */
        void setKeysABP(char *DevAddr, char *NwkSKey, char *AppSKey);
        
        /**
         *  \brief Set the data rate
         *  
         *  \param [in] physicalType The type of ISM
         *  
         *  \return Return null.
         */      
        void setDataRate(_physical_type_t physicalType = EU868); 

        /**
         *  \brief Set the output power
         *  
         *  \param [in] power The output power value
         *  
         *  \return Return null.
         */
        void setPower(short power);

        /**
         *  \brief Set the port number
         *  
         *  \param [in] port The port number, range from 1 to 255
         *  
         *  \return Return null.
         */
        void setPort(unsigned char port);

        /**
         *  \brief ON/OFF adaptive data rate mode
         *  
         *  \param [in] command The date rate of encoding
         *  
         *  \return Return null.
         */ 
        void setAdaptiveDataRate(bool command);
        
        /**
         *  \brief Set the channel parameter
         *  
         *  \param [in] channel The channel number, range from 0 to 71
         *  \param [in] frequency The frequency value
         *  
         *  \return Return null.
         */
        
        /**
         *  \brief Set the channel parameter
         *  
         *  \param [in] channel The channel number, range from 0 to 71
         *  \param [in] frequency The frequency value
         *  \param [in] dataRataMin The minimum date rate of channel
         *  \param [in] dataRataMax The maximum date rate of channel
         *  
         *  \return Return null.
         */
        void setChannel(unsigned char channel, float frequency, _data_rate_t dataRataMin, _data_rate_t dataRataMax);
        
        /**
         *  \brief Transmit the data
         *  
         *  \param [in] *buffer The transmit data cache
         *  \param [in] timeout The over time of transmit
         *  
         *  \return Return bool. Ture : transmit done, false : transmit failed
         */
        bool transmitPacket(char *buffer, unsigned char timeout = DEFAULT_TIMEOUT);

        /**
         *  \brief Transmit the data
         *  
         *  \param [in] *buffer The transmit data cache
         *  \param [in] length The length of data cache
         *  \param [in] timeout The over time of transmit
         *  
         *  \return Return bool. Ture : transmit done, false : transmit failed
         */
        bool transmitPacket(unsigned char *buffer, unsigned char length, unsigned char timeout = DEFAULT_TIMEOUT);

        /**
         *  \brief Transmit the packet data
         *  
         *  \param [in] *buffer The transmit data cache
         *  \param [in] timeout The over time of transmit
         *  
         *  \return Return bool. Ture : Confirmed ACK, false : Confirmed NOT ACK
         */
        bool transmitPacketWithConfirmed(char *buffer, unsigned char timeout = DEFAULT_TIMEOUT);

        /**
         *  \brief Transmit the data
         *  
         *  \param [in] *buffer The transmit data cache
         *  \param [in] length The length of data cache
         *  \param [in] timeout The over time of transmit
         *  
         *  \return Return bool. Ture : Confirmed ACK, false : Confirmed NOT ACK
         */
        bool transmitPacketWithConfirmed(unsigned char *buffer, unsigned char length, unsigned char timeout = DEFAULT_TIMEOUT);

        /**
         *  \brief Receive the data
         *  
         *  \param [in] *buffer The receive data cache
         *  \param [in] length The length of data cache
         *  \param [in] *rssi The RSSI cache
         *  
         *  \return Return Receive data number
         */
        short receivePacket(char *buffer, short length, short *rssi);
        
        /**
         *  \brief Transmit the proprietary data
         *  
         *  \param [in] *buffer The transmit data cache
         *  \param [in] timeout The over time of transmit
         *  
         *  \return Return bool. Ture : transmit done, false : transmit failed
         */
        bool transmitProprietaryPacket(char *buffer, unsigned char timeout = DEFAULT_TIMEOUT);

        /**
         *  \brief Transmit the proprietary data
         *  
         *  \param [in] *buffer The transmit data cache
         *  \param [in] length The length of data cache
         *  \param [in] timeout The over time of transmit
         *  
         *  \return Return bool. Ture : transmit done, false : transmit failed
         */
        bool transmitProprietaryPacket(unsigned char *buffer, unsigned char length, unsigned char timeout = DEFAULT_TIMEOUT);
        
        /**
         *  \brief Set device mode
         *  
         *  \param [in] mode The mode of device
         *  
         *  \return Return null
         */
        void setActivation(_device_mode_t mode);
        
        /**
         *  \brief Set device join a network
         *  
         *  \param [in] command The type of join
         *  \param [in] timeout The over time of join
         *  
         *  \return Return bool. True : join OK, false : join NOT OK
         */
        bool setOTAAJoin(_otaa_join_cmd_t command, unsigned char timeout = DEFAULT_TIMEOUT);
        
        /**
         *  \brief Set message unconfirmed repeat time
         *  
         *  \param [in] time The repeat time, range from 1 to 15
         *  
         *  \return Return null
         */
        void setUnconfirmedMessageRepeatTime(unsigned char time);
        
        /**
         *  \brief Set message retry times time
         *  
         *  \param [in] time The retry time, range from 0 to 254
         *  
         *  \return Return null
         */
        void setConfirmedMessageRetryTime(unsigned char time);
        
        /**
         *  \brief ON/OFF receice window 1
         *  
         *  \param [in] command The true : ON, false OFF
         *  
         *  \return Return null
         */
        void setReceiveWindowFirst(bool command);

        /**
         *  \brief Set receice window 1 channel mapping
         *  
         *  \param [in] channel The channel number, range from 0 to 71
         *  \param [in] frequency The frequency value of channel
         *  
         *  \return Return null
         */
        void setReceiveWindowFirst(unsigned char channel, float frequency);
        
        /**
         *  \brief Set receice window 2 channel mapping
         *  
         *  \param [in] frequency The frequency value of channel
         *  \param [in] dataRate The date rate value
         *  
         *  \return Return null
         */
        void setReceiveWindowSecond(float frequency, _data_rate_t dataRate);
        
        /**
         *  \brief Set receice window 2 channel mapping
         *  
         *  \param [in] frequency The frequency value of channel
         *  \param [in] spreadingFactor The spreading factor value
         *  \param [in] bandwidth The band width value
         *  
         *  \return Return null
         */
        void setReceiveWindowSecond(float frequency, _spreading_factor_t spreadingFactor, _band_width_t bandwidth);
        
        /**
         *  \brief ON/OFF duty cycle limitation
         *  
         *  \param [in] command The true : ON, false OFF
         *  
         *  \return Return null
         */
        void setDutyCycle(bool command);
        
        /**
         *  \brief ON/OFF join duty cycle limitation
         *  
         *  \param [in] command The true : ON, false OFF
         *  
         *  \return Return null
         */
        void setJoinDutyCycle(bool command);
        
        /**
         *  \brief Set receice window delay
         *  
         *  \param [in] command The delay type
         *  \param [in] _delay The delay value(millisecond)
         *  
         *  \return Return null
         */
        void setReceiveWindowDelay(_window_delay_t command, unsigned short _delay);
        
        /**
         *  \brief Set LoRaWAN class type
         *  
         *  \param [in] type The class type
         *  
         *  \return Return null
         */
        void setClassType(_class_type_t type);

        /**
         *  \brief Set beacon and ping slot configuration
         * 
         *  \param periodicity The ping slot period factor (2^periodicity seconds)
         *  
         *  \return Return null
         */
        void setBeaconAndPingSlot(int periodicity);

        /**
         *  \brief Wait for Class B setup
         *  
         *  \return True if successful
         */
        bool checkClassBDone();

        /**
         *  \brief Check for beacon loss
         *  
         *  \return True if beacon lost
         */
        bool checkBeaconLost();

        /**
         *  \brief Set device into low power mode
         *  
         *  \return Return null
         */
        void setDeviceLowPower(void);

        /**
         *  \brief Wake up device from low power mode
         *  
         *  \return Return null
         */
        void setDeviceLowPowerWakeUp(void);
        
        /**
         *  \brief Reset device
         *  
         *  \return Return null
         */
        void setDeviceReset();
        
        /**
         *  \brief Setup device default
         *  
         *  \return Return null
         */
        void setDeviceDefault(void);
        
        /**
         *  \brief Read battery voltage
         *  
         *  \return Return battery voltage
         */
        float getBatteryVoltage(void);

        /**
         *  \brief Read battery status
         *  
         *  \return Return false while charging, return true while charge done or no battery insert
         */
        bool getBatteryStatus(void);

        /**
         *  \brief Read module temperature
         *  
         *  \return Return module temperature
         */
        float getModuleTemperatureC(void);

        bool containsSubstring(const char* buffer, const char* substring);
        
        void loraPrint(unsigned char timeout);

    private:
        void sendCommand(char *command);
        short readBuffer(char* buffer, short length, unsigned char timeout = DEFAULT_TIMEOUT);
        char _buffer[256];

};

extern LoRaWanClass lora;

#endif
