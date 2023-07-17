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

#ifndef TINY_NEC_H
#define TINY_NEC_H

#include <Energia.h>
#include <ti/drivers/PWM.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOMSP432.h>

/* driverlib header files */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/devices/msp432p4xx/driverlib/gpio.h>

#include "LongUnion.h"

/** \addtogroup TinyReceiver Minimal receiver for NEC protocol
 * @{
 */

/*
 * Debug directives
 */
#if defined(DEBUG) || defined(TRACE)
#  define IR_DEBUG_PRINT(...)    {Serial.print(__VA_ARGS__);Serial.flush();}
#  define IR_DEBUG_PRINTLN(...)  {Serial.println(__VA_ARGS__);Serial.flush();}
#else
/**
 * If DEBUG, print the arguments, otherwise do nothing.
 */
#  define IR_DEBUG_PRINT(...) void()
/**
 * If DEBUG, print the arguments as a line, otherwise do nothing.
 */
#  define IR_DEBUG_PRINTLN(...) void()
#endif

#if defined(TRACE)
#  define TRACE_PRINT(...)    {Serial.print(__VA_ARGS__);Serial.flush();}
#  define TRACE_PRINTLN(...)  {Serial.println(__VA_ARGS__);Serial.flush();}
#  define IR_TRACE_PRINT(...)    {Serial.print(__VA_ARGS__);Serial.flush();}
#  define IR_TRACE_PRINTLN(...)  {Serial.println(__VA_ARGS__);Serial.flush();}
#else
#  define TRACE_PRINT(...) void()
#  define TRACE_PRINTLN(...) void()
#  define IR_TRACE_PRINT(...) void()
#  define IR_TRACE_PRINTLN(...) void()
#endif


#if defined(HANDLE_IR_EVENT)
/*
 * This function is called if a complete command was received and must be 
 *  implemented by the including file (user code)
 */
void handleReceivedTinyIRData(uint16_t aAddress, uint8_t aCommand, bool isRepeat);
#endif

#define IR_FREQUENCY_KHZ        38

// LSB first, 1 start bit + 16 bit address + 8 bit data + 8 bit inverted data + 1 stop bit.
// see: https://www.sbprojects.net/knowledge/ir/nec.php

#define NEC_ADDRESS_BITS        16 // 16 bit address or 8 bit address and 8 bit inverted address
#define NEC_COMMAND_BITS        16 // Command and inverted command

#define NEC_BITS                (NEC_ADDRESS_BITS + NEC_COMMAND_BITS)
#define NEC_UNIT                560

#define NEC_HEADER_MARK         (16 * NEC_UNIT) // 9000
#define NEC_HEADER_SPACE        (8 * NEC_UNIT)  // 4500

#define NEC_BIT_MARK            NEC_UNIT
#define NEC_ONE_SPACE           (3 * NEC_UNIT)  // 1690
#define NEC_ZERO_SPACE          NEC_UNIT

#define NEC_REPEAT_HEADER_SPACE (4 * NEC_UNIT)  // 2250

#define NEC_AVERAGE_DURATION    62000 // NEC_HEADER_MARK + NEC_HEADER_SPACE + 32 * 2,5 * NEC_UNIT + NEC_UNIT // 2.5 because we assume more zeros than ones
#define NEC_REPEAT_DURATION     (NEC_HEADER_MARK  + NEC_REPEAT_HEADER_SPACE + NEC_BIT_MARK) // 12 ms
#define NEC_REPEAT_PERIOD       110000 // Commands are repeated every 110 ms (measured from start to start) for as long as the key on the remote control is held down.
#define NEC_REPEAT_SPACE        (NEC_REPEAT_PERIOD - NEC_AVERAGE_DURATION) // 48 ms

/*
 * Macros for comparing timing values
 */
#define lowerValue25Percent(aDuration)   (aDuration - (aDuration / 4))
#define upperValue25Percent(aDuration)   (aDuration + (aDuration / 4))
#define lowerValue(aDuration)   (aDuration - (aDuration / 2))
#define upperValue(aDuration)   (aDuration + (aDuration / 2))

/*
 * The states for the state machine
 */
#define IR_RECEIVER_STATE_WAITING_FOR_START_MARK        0
#define IR_RECEIVER_STATE_WAITING_FOR_START_SPACE       1
#define IR_RECEIVER_STATE_WAITING_FOR_FIRST_DATA_MARK   2
#define IR_RECEIVER_STATE_WAITING_FOR_DATA_SPACE        3
#define IR_RECEIVER_STATE_WAITING_FOR_DATA_MARK         4
#define IR_RECEIVER_STATE_WAITING_FOR_STOP_MARK         5

#define MAX_SUPPORTED_PIN   40

static const uint32_t GPIO_PORT_TO_BASE[] =
{   0x00,
    (uint32_t)P1,
    (uint32_t)P1+1,
    (uint32_t)P3,
    (uint32_t)P3+1,
    (uint32_t)P5,
    (uint32_t)P5+1,
    (uint32_t)P7,
    (uint32_t)P7+1,
    (uint32_t)P9,
    (uint32_t)P9+1,
    (uint32_t)PJ
};

extern const GPIOMSP432_Config GPIOMSP432_config;
/*
 * Device specific interpretation of the GPIO_PinConfig content
 */
typedef struct PinConfig {
    uint8_t pin;
    uint8_t port;
    uint16_t config;
} PinConfig;

/**
 * An enum consisting of all formats supported by IRRemote library.
 * Included for consistancy with existing libraries.
 */
typedef enum {
    UNKNOWN = 0,
    PULSE_DISTANCE,
    PULSE_WIDTH,
    DENON,
    DISH,
    JVC,
    LG,
    LG2,
    NEC,
    PANASONIC,
    KASEIKYO,
    KASEIKYO_JVC,
    KASEIKYO_DENON,
    KASEIKYO_SHARP,
    KASEIKYO_MITSUBISHI,
    RC5,
    RC6,
    SAMSUNG,
    SHARP,
    SONY,
    ONKYO,
    APPLE,
    BOSEWAVE,
    LEGO_PF,
    MAGIQUEST,
    WHYNTER,
} decode_type_t;

/**
 * Data structure for the user application, adapted from IRRemote library.
 * Minimal struct to only include necessary data.
 */
typedef struct IRDataStruct {
    decode_type_t protocol;     ///< UNKNOWN, NEC, SONY, RC5, ...
    uint16_t address;           ///< Decoded address
    uint16_t command;           ///< Decoded command
    bool isRepeat;
} IRData;


/****************************************************
 *                     LED FEEDBACK
 ****************************************************/

#define USE_DEFAULT_FEEDBACK_LED_PIN        0
#define DO_NOT_ENABLE_LED_FEEDBACK          0x00
#define LED_FEEDBACK_DISABLED_COMPLETELY    0x00
#define LED_FEEDBACK_ENABLED_FOR_RECEIVE    0x01
#define LED_FEEDBACK_ENABLED_FOR_SEND       0x02

/**
 * Contains pin number and enable status of the feedback LED
 */
//typedef struct FeedbackLEDControlStruct {
//    uint8_t FeedbackLEDPin;         ///< if 0, then take board specific FEEDBACK_LED_ON() and FEEDBACK_LED_OFF() functions
//    uint8_t LedFeedbackEnabled; ///< LED_FEEDBACK_ENABLED_FOR_RECEIVE or LED_FEEDBACK_ENABLED_FOR_SEND -> enable blinking of pin on IR processing
//} FeedbackLEDControl;

// Mapping of pin # to LED Feedback enable status (allows multiple objects to use same feedback LED)
uint8_t feedbackLEDs[MAX_SUPPORTED_PIN + 1] = {0,    // BUILTIN_LED
    DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, 
    DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, 
    DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, 
    DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, 
    DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, 
    DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, 
    DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, 
    DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK, DO_NOT_ENABLE_LED_FEEDBACK
};

uint8_t* setLEDFeedback(uint8_t aFeedbackLEDPin, uint8_t aEnableLEDFeedback);
void setTxFeedbackLED(uint8_t aFeedbackLEDPin, bool aSwitchLedOn);
void setRxFeedbackLED(uint8_t aFeedbackLEDPin, bool aSwitchLedOn);


/****************************************************
 *                     RECEIVING
 ****************************************************/

/**
 * Control and data variables of the state machine for TinyReceiver
 */
struct TinyIRReceiverStruct {
    /*
     * State machine
     */
    uint32_t LastChangeMicros;      ///< microseconds of last Pin Change Interrupt.
    uint8_t IRReceiverState;        ///< the state of the state machine.
    uint8_t IRRawDataBitCounter;
    /*
     * Data
     */
    uint32_t IRRawDataMask;
    LongUnion IRRawData;
    bool IRRepeatDetected;
    bool newCommandAvailable;
};

//const __FlashStringHelper* getProtocolString(decode_type_t aProtocol);

bool initTinyIRReceiver(uint8_t aRcvPin, bool aEnableLEDFeedback = false, uint8_t aFeedbackLEDPin = USE_DEFAULT_FEEDBACK_LED_PIN);
bool decodeIR(IRData *results);


/****************************************************
 *                     SENDING
 ****************************************************/

/**
 * Just for better readability of code
 */
#define NO_REPEATS  0
#define SEND_STOP_BIT true
#define SEND_NO_STOP_BIT false
#define SEND_REPEAT_COMMAND true ///< used for e.g. NEC, where a repeat is different from just repeating the data.

/**
 * Duty cycle in percent for sent signals.
 */
#if ! defined(IR_SEND_DUTY_CYCLE)
#define IR_SEND_DUTY_CYCLE 30 // 30 saves power and is compatible to the old existing code
#endif

/**
 * Main class for sending IR signals
 */
class IRsend {
public:
    IRsend();

    bool begin(uint8_t aSendPin, bool aEnableLEDFeedback = false, uint8_t aFeedbackLEDPin = USE_DEFAULT_FEEDBACK_LED_PIN);

    size_t write(IRData *aIRSendData, uint8_t aNumberOfRepeats = NO_REPEATS);

private:
    void enableIROut();

    void sendPulseDistanceWidthData(unsigned int aOneMarkMicros, unsigned int aOneSpaceMicros, unsigned int aZeroMarkMicros, unsigned int aZeroSpaceMicros, uint32_t aData, uint8_t aNumberOfBits, bool aSendStopBit);

    void mark(unsigned int aMarkMicros);
    static void space(unsigned int aSpaceMicros);
    void IRLedOff();
    
    void pwmStart();
    void pwmStop();
    bool timerConfigForSend(uint8_t aFrequencyKHz);

    void setRegisters(uint8_t aSendPin);
    
    /*
     * New send functions
     */
    void sendNECRepeat();
    void sendNEC(uint16_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats = NO_REPEATS, bool aIsRepeat = false);
    void sendNECRaw(uint32_t aRawData, uint8_t aNumberOfRepeats = NO_REPEATS, bool aIsRepeat = false);


    uint8_t feedbackLEDpin; 
    uint8_t sendPin;
    uint8_t pwmIndex;
    uint8_t timerIndex;
    PWM_Handle pwmHandle;
//    volatile uint8_t *outReg;
    uint16_t pinMask;
    uint16_t sel0Mask;
    uint16_t sel1Mask;
    volatile uint16_t *dirReg;
    volatile uint16_t *sel0Reg;
    volatile uint16_t *sel1Reg;
    volatile uint16_t *outReg;
    volatile uint16_t *renReg;
//#define HWREG8(x)         (*((volatile uint8_t *)(x)))

    static void customDelayMicroseconds(unsigned long aMicroseconds);
};


/** @}*/

#endif // TINY_NEC_H

#pragma once
