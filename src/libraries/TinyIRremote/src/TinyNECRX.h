/*
 *  TinyNECRX.h
 *
 *
 *  Copyright (C) 2022  chris miller
 *  miller4@rose-hulman.edu
 *
 *  Modified 2022 chris miller to support MSP432 on Energia
 *  Modified 2020-2021  Armin Joachimsmeyer
 *  armin.joachimsmeyer@gmail.com
 *
 *  This file adapted from source in IRMP https://github.com/ukw100/IRMP.
 *  This file adapted from source in Arduino-IRremote https://github.com/Arduino-IRremote/Arduino-IRremote.
 *
 *  TinyIR is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/gpl.html>.
 *
 */

#ifndef TINY_NEC_RX_H
#define TINY_NEC_RX_H

#include "TinyNEC.h"
#include "IRData.h"
#include "IRFeedbackLED.h"
#include "LongUnion.h"


/**
 *  @brief  IRData callback function type
 *
 *  @param  address     Address value from received IR command
 *  @param  command     Commmand value from received IR command
 *  @param  isRepeat    True if the last received IR command is a repeat
 */
typedef void (*IRData_CallbackFxn)(uint16_t address, uint8_t command, bool isRepeat);

/**
 * Control and data variables of the state machine for TinyReceiver
 */
//struct TinyIRReceiverStruct {
class IRreceiver {
public:
    /* Default (only) constructor. Must call initIRReceiver() and verify success
     *  before using object.
     *  
     *  @param receivePin   The Arduino pin number, where an IR receiver is connected.
     */
    IRreceiver(uint8_t receivePin);

    /**
     * Initializes hardware interrupt generation for receiver pin. If callback
     * is enabled, every new IR command received will result in a callback to 
     * user defined function. Callback function must be defined as:
     *  void handleReceivedTinyIRData(uint16_t aAddress, uint8_t aCommand, bool isRepeat);
     * 
     *  @param includeRepeats       if false, only new IR commands will be reported
     *  @param enableCallback       enables callback to user defined function
     *  @param callbackFunction     reference to callback function handler
     * 
     * @return true if successful (should always succeed for valid GPIO pin)
     */
    bool initIRReceiver(bool includeRepeats = true, bool enableCallback = false,
                void (*callbackFunction)(uint16_t , uint8_t , bool) = NULL);

//    size_t write(IRData *aIRSendData, uint8_t aNumberOfRepeats = NO_REPEATS);

    /**
     * Check for new IR command received. Will never return true if user
     *  callback function is enabled.
     * 
     * @return true if new command available. Future calls will return FALSE 
     *  until new IR command received.
     */
    bool newCommandReceived();

    /**
     * Fill IRData struct if new command has been received. Will never return 
     *  true if user callback function is enabled. Calls to newCommandReceived() 
     *  will return FALSE after calling this method.
     * 
     * @param results Reference to IRData struct to be populated with new command data.
     * 
     * @return true if new command available (results populated with new command).
     */
    bool decodeIR(IRData *results);

    /**
     * Get address value of last received IR command.
     * 
     * @return address value of last received IR command
     */
    uint16_t getAddress();
    /**
     * Get command value of last received IR command.
     * 
     * @return command value of last received IR command
     */
    uint16_t getCommand();
    /**
     * Get repeat value of last received IR command.
     * 
     * @return true if last received IR command is a repeat
     */
    bool getIsRepeat();

private:
    /* pin configuration */
    uint8_t rcvPin;
    PinConfig *config;
//    uint16_t pinMask;
    volatile uint8_t *iesReg;
    volatile uint8_t *ifgReg;
    /* state machine */
    uint32_t LastChangeMicros;      ///< microseconds of last Pin Change Interrupt.
    uint8_t IRReceiverState;        ///< the state of the state machine.
    uint8_t IRRawDataBitCounter;
    /* data */
    uint32_t IRRawDataMask;
    LongUnion IRRawData;
    IRData_CallbackFxn callback;
    bool enCallback;
    bool incRepeats;
    bool IRRepeatDetected;
    bool newCommandAvailable;
    bool initialized;
    /**
     * The ISR of TinyIRRreceiver.
     * It handles the NEC protocol decoding and calls the user callback function on complete.
     * 5 us + 3 us for push + pop for a 16MHz ATmega
     */
    void interruptHandler(void);
    friend void IRPinChangeInterruptHandler(uint_least8_t);
};

//#if defined(HANDLE_IR_EVENT)
/*
 * This function is called if a complete command was received and must be 
 *  implemented by the including file (user code)
 */
//void handleReceivedTinyIRData(uint16_t aAddress, uint8_t aCommand, bool isRepeat);
//#endif


#endif // TINY_NEC_RX_H

//#pragma once
