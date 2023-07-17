/*
 *  TinyNECRX.cpp
 *
 *  Receives IR protocol data of NEC protocol using pin change interrupts.
 *  NEC is the protocol of most cheap remote controls for Arduino.
 *
 *  No parity check is done!
 *  If HANDLE_IR_EVENT is enabled, on a completely received IR command the user function 
 *    handleReceivedTinyIRData(uint16_t aAddress, uint8_t aCommand, bool isRepetition)
 *    is called in Interrupt context but with interrupts being enabled to enable 
 *    use of delay() etc.
 *  !!!!!!!!!!!!!!!!!!!!!!
 *  Functions called in interrupt context should be running as short as possible,
 *  so if you require longer action, save the data (address + command) and handle 
 *  them in the main loop.
 *  !!!!!!!!!!!!!!!!!!!!!
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

#ifndef TINY_NEC_RX_CPP_H
#define TINY_NEC_RX_CPP_H

#include <Energia.h>

#include "TinyNEC.h"


/** \addtogroup TinyReceiver Minimal receiver for NEC protocol
 * @{
 */
//#define TRACE
#define PIN_NOT_SET         0
#define INVALID_PIN_PORT    0

/**
 * IR receiver is defined as a single instance because of the limitation of the interrupt handler.
 *  The ISR must know the Port/Pin in order to change the interrupt edge and clear the flag, and there 
 *  is no efficient means for passing this to multiple ISR instances. Therefore, only one may be defined.
 */
TinyIRReceiverStruct TinyIRReceiverControl;

static uint8_t feedbackLEDpin; 
static uint8_t rcvPin = PIN_NOT_SET;
static uint16_t pinMask;
static volatile uint16_t *iesReg;
static volatile uint16_t *ifgReg;

/**
 * The ISR of TinyIRRreceiver.
 * It handles the NEC protocol decoding and calls the user callback function on complete.
 * 5 us + 3 us for push + pop for a 16MHz ATmega
 */
void IRPinChangeInterruptHandler(void) {
#if defined(IR_MEASURE_TIMING) && defined(IR_TIMING_TEST_PIN)
    digitalWrite(IR_TIMING_TEST_PIN, HIGH); // 2 clock cycles
#endif
    /*
     * Save IR input level
     * Negative logic, true / HIGH means inactive / IR space, LOW / false means IR mark.
     */
    uint8_t tIRLevel = digitalRead(rcvPin);

    // Toggle triggering edge and clear flag (workaround to MSP432 limitation of not triggering on CHANGE)
    *iesReg ^= pinMask;
    *ifgReg &= ~pinMask;

#if !defined(DO_NOT_USE_FEEDBACK_LED) 
    setRxFeedbackLED(feedbackLEDpin, !tIRLevel);
#endif

    /*
     * compute microseconds after last change
     */
    uint32_t tCurrentMicros = micros();
    uint16_t tMicrosOfMarkOrSpace = tCurrentMicros - TinyIRReceiverControl.LastChangeMicros;
    TinyIRReceiverControl.LastChangeMicros = tCurrentMicros;

    uint8_t tState = TinyIRReceiverControl.IRReceiverState;

#ifdef TRACE
    Serial.print(tState);
    Serial.print(' ');
//    Serial.print(F(" I="));
//    Serial.print(tIRLevel);
//    Serial.print(F(" D="));
//    Serial.print(tDeltaMicros);
//    Serial.println();
#endif

    if (tIRLevel == LOW) {
        /*
         * We have a mark here
         */
        if (tMicrosOfMarkOrSpace > 2 * NEC_HEADER_MARK) {
            // timeout -> must reset state machine
            tState = IR_RECEIVER_STATE_WAITING_FOR_START_MARK;
        }
        if (tState == IR_RECEIVER_STATE_WAITING_FOR_START_MARK) {
            // We are at the beginning of the header mark, check timing at the next transition
            tState = IR_RECEIVER_STATE_WAITING_FOR_START_SPACE;
        }

        else if (tState == IR_RECEIVER_STATE_WAITING_FOR_FIRST_DATA_MARK) {
            if (tMicrosOfMarkOrSpace >= lowerValue25Percent(NEC_HEADER_SPACE)
                    && tMicrosOfMarkOrSpace <= upperValue25Percent(NEC_HEADER_SPACE)) {
                /*
                 * We have a valid data header space here -> initialize data
                 */
                TinyIRReceiverControl.IRRawDataBitCounter = 0;
                TinyIRReceiverControl.IRRawData.ULong = 0;
                TinyIRReceiverControl.IRRawDataMask = 1;
                TinyIRReceiverControl.IRRepeatDetected = false;
                tState = IR_RECEIVER_STATE_WAITING_FOR_DATA_SPACE;
            } else if (tMicrosOfMarkOrSpace >= lowerValue25Percent(NEC_REPEAT_HEADER_SPACE)
                    && tMicrosOfMarkOrSpace <= upperValue25Percent(NEC_REPEAT_HEADER_SPACE)
                    && TinyIRReceiverControl.IRRawDataBitCounter >= NEC_BITS) {
                /*
                 * We have a repeat header here and no broken receive before -> set repeat flag
                 */
                TinyIRReceiverControl.IRRepeatDetected = true;
                tState = IR_RECEIVER_STATE_WAITING_FOR_DATA_SPACE;
            } else {
                // This parts are optimized by the compiler into jumps to one code :-)
                // Wrong length -> reset state
                tState = IR_RECEIVER_STATE_WAITING_FOR_START_MARK;
            }
        }

        else if (tState == IR_RECEIVER_STATE_WAITING_FOR_DATA_MARK) {
            // Check data space length
            if (tMicrosOfMarkOrSpace >= lowerValue(NEC_ZERO_SPACE) && tMicrosOfMarkOrSpace <= upperValue(NEC_ONE_SPACE)) {
                // We have a valid bit here
                tState = IR_RECEIVER_STATE_WAITING_FOR_DATA_SPACE;
                if (tMicrosOfMarkOrSpace >= 2 * NEC_UNIT) {
                    // we received a 1
                    TinyIRReceiverControl.IRRawData.ULong |= TinyIRReceiverControl.IRRawDataMask;
                } else {
                    // we received a 0 - empty code for documentation
                }
                // prepare for next bit
                TinyIRReceiverControl.IRRawDataMask = TinyIRReceiverControl.IRRawDataMask << 1;
                TinyIRReceiverControl.IRRawDataBitCounter++;
            } else {
                // Wrong length -> reset state
                tState = IR_RECEIVER_STATE_WAITING_FOR_START_MARK;
            }
        } else {
            // error wrong state for the received level, e.g. if we missed one change interrupt -> reset state
            tState = IR_RECEIVER_STATE_WAITING_FOR_START_MARK;
        }
    } else {
        /*
         * We have a space here
         */
        if (tState == IR_RECEIVER_STATE_WAITING_FOR_START_SPACE) {
            /*
             * Check length of header mark here
             */
            if (tMicrosOfMarkOrSpace >= lowerValue25Percent(NEC_HEADER_MARK)
                    && tMicrosOfMarkOrSpace <= upperValue25Percent(NEC_HEADER_MARK)) {
                tState = IR_RECEIVER_STATE_WAITING_FOR_FIRST_DATA_MARK;
            } else {
                // Wrong length of header mark -> reset state
                tState = IR_RECEIVER_STATE_WAITING_FOR_START_MARK;
            }
        } else if (tState == IR_RECEIVER_STATE_WAITING_FOR_DATA_SPACE) {
            // Check data mark length
            if (tMicrosOfMarkOrSpace >= lowerValue(NEC_BIT_MARK) && tMicrosOfMarkOrSpace <= upperValue(NEC_BIT_MARK)) {
                /*
                 * We have a valid mark here, check for transmission complete
                 */
                if (TinyIRReceiverControl.IRRawDataBitCounter >= NEC_BITS || TinyIRReceiverControl.IRRepeatDetected) {
                    /*
                     * Code complete -> call callback, no parity check!
                     */
                    // Reset state for new start
                    tState = IR_RECEIVER_STATE_WAITING_FOR_START_MARK;
                    interrupts();
                    /*
                     * Address reduction to 8 bit
                     */
                    if (TinyIRReceiverControl.IRRawData.UByte.LowByte
                            == (uint8_t) (~TinyIRReceiverControl.IRRawData.UByte.MidLowByte)) {
                        // standard 8 bit address NEC protocol
                        TinyIRReceiverControl.IRRawData.UByte.MidLowByte = 0; // Address is the first 8 bit
                    }

#if defined(HANDLE_IR_EVENT)
                    /*
                     * Call user provided callback here
                     */
                    handleReceivedTinyIRData(TinyIRReceiverControl.IRRawData.UWord.LowWord,
                            TinyIRReceiverControl.IRRawData.UByte.MidHighByte, TinyIRReceiverControl.IRRepeatDetected);
#endif
#if !defined(EXCLUDE_REPEATS)
                    TinyIRReceiverControl.newCommandAvailable = TRUE;
#else
                    // Only registers that new data is available if it is not a repeat
                    TinyIRReceiverControl.newCommandAvailable = !(TinyIRReceiverControl.IRRepeatDetected);
#endif
                } else {
                    // not finished yet
                    tState = IR_RECEIVER_STATE_WAITING_FOR_DATA_MARK;
                }
            } else {
                // Wrong length -> reset state
                tState = IR_RECEIVER_STATE_WAITING_FOR_START_MARK;
            }
        } else {
            // error wrong state for the received level, e.g. if we missed one change interrupt -> reset state
            tState = IR_RECEIVER_STATE_WAITING_FOR_START_MARK;
        }
    }

    TinyIRReceiverControl.IRReceiverState = tState;
#ifdef IR_MEASURE_TIMING
    digitalWrite(IR_TIMING_TEST_PIN, LOW); // 2 clock cycles
#endif
#ifdef DEBUG
    uint32_t isrTime = micros();
    IR_DEBUG_PRINT("IR receiver ISR time (us): ");
    IR_DEBUG_PRINTLN((isrTime - tCurrentMicros));
#endif
}

/**
 * Initializes hardware interrupt generation with attachInterrupt() function.
 * @param aRcvPin The Arduino pin number, where a IR receiver is connected.
 * @param aEnableLEDFeedback if true, enable LED feedback when receiving
 * @param aLEDFeedbackPin if 0, then take board specific FEEDBACK_LED_ON() and FEEDBACK_LED_OFF() functions
 * 
 * returns true if successful (should always succeed for valid GPIO pin)
 */
bool initTinyIRReceiver(uint8_t aRcvPin, bool aEnableLEDFeedback, uint8_t aFeedbackLEDPin) {
#if defined(IR_MEASURE_TIMING) && defined(IR_TIMING_TEST_PIN)
    pinMode(IR_TIMING_TEST_PIN, OUTPUT);
#endif

    if (aRcvPin >= GPIOMSP432_config.numberOfPinConfigs) {
        return false;
    }
    PinConfig *config = (PinConfig *) &GPIOMSP432_config.pinConfigs[aRcvPin];
    IR_DEBUG_PRINT("IR receiver port ");
    IR_DEBUG_PRINTLN(config->port);
    IR_DEBUG_PRINT("IR receiver pin mask ");
    IR_DEBUG_PRINTLN(config->pin);
    if (config->port == INVALID_PIN_PORT) {
        return false;
    }

    // if receiver has already been configured for another pin, remove previous interrupt before replacing
    if ((rcvPin != PIN_NOT_SET) & (rcvPin != aRcvPin)) {
        detachInterrupt(rcvPin);
    }
    rcvPin = aRcvPin;
    pinMode(aRcvPin, INPUT_PULLUP);
#if !defined(DO_NOT_USE_FEEDBACK_LED) 
    if (aEnableLEDFeedback) {
        setLEDFeedback(aFeedbackLEDPin, LED_FEEDBACK_ENABLED_FOR_RECEIVE);
    }
    feedbackLEDpin = aFeedbackLEDPin;
#endif

    // store references to registers for quick updates in ISR
    uint32_t baseAddress = GPIO_PORT_TO_BASE[config->port];
    pinMask = config->pin;
    iesReg = (volatile uint16_t *)(baseAddress + OFS_LIB_PAIES);   //HWREG16
    ifgReg = (volatile uint16_t *)(baseAddress + OFS_LIB_PAIFG);

    // attach ISR for falling-edge on IR receiver pin (bug in MSP432 drivers 
    //  prevents triggering on both edges, so have to swap triggering edge in ISR)
    attachInterrupt(aRcvPin, IRPinChangeInterruptHandler, FALLING); 
    return true;
}

/**
 * Fill IRData struct if new command has been received.
 * @param results Reference to IRData struct to be populated with new command data.
 * 
 * returns true if new command available (results populated with new command)
 */
bool decodeIR(IRData *results) {
    
    if (TinyIRReceiverControl.newCommandAvailable) {
        results->protocol = NEC;
        results->address = TinyIRReceiverControl.IRRawData.UWord.LowWord;
        results->command = TinyIRReceiverControl.IRRawData.UByte.MidHighByte;
        results->isRepeat = TinyIRReceiverControl.IRRepeatDetected;
        TinyIRReceiverControl.newCommandAvailable = FALSE;
        
        return TRUE;
    }
    return FALSE;
}

/** @}*/

#endif // TINY_NEC_RX_CPP_H
