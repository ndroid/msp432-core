/*
 *  TinyNECTX.h
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
 *  TinyIRremote is free software: you can redistribute it and/or modify
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

#ifndef TINY_NEC_TX_H
#define TINY_NEC_TX_H

#include "TinyNEC.h"
#include "IRData.h"
#include "IRFeedbackLED.h"


/**
 * Just for better readability of code
 */
#define NO_REPEATS              0
#define SEND_STOP_BIT           true
#define SEND_NO_STOP_BIT        false
#define SEND_REPEAT_COMMAND     true ///< used for e.g. NEC, where a repeat is different from just repeating the data.

/**
 * Duty cycle in percent for sent signals.
 */
#if ! defined(IR_SEND_DUTY_CYCLE)
#define IR_SEND_DUTY_CYCLE 30 // 30 saves power and is compatible to the old existing code
#endif

/**
 * Main class for sending IR signals
 */
class IRsender {
public:
    IRsender();

    bool begin(uint8_t sendPin, bool enableLEDFeedback = false, uint8_t feedbackLEDPin = USE_DEFAULT_FEEDBACK_LED_PIN);

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


#endif // TINY_NEC_TX_H

#pragma once
