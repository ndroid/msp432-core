/**
 * @file IRFeedbackLED.hpp
 *
 * @brief All Feedback LED specific functions are contained in this file.
 *
 *  Modified 2022 chris miller to support MSP432 on Energia
 *  miller4@rose-hulman.edu
 * 
 *  This file adapted from source in Arduino-IRremote https://github.com/Arduino-IRremote/Arduino-IRremote.
 *
 *************************************************************************************
 * MIT License
 *
 * Copyright (c) 2021-2022 Armin Joachimsmeyer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************************
 */
#ifndef _IR_FEEDBACK_LED_HPP
#define _IR_FEEDBACK_LED_HPP

#include "TinyNEC.h"

/** \addtogroup FeedbackLEDFunctions Feedback LED functions
 * @{
 */


/**
 * Enable blinking of feedback LED (IR_FEEDBACK_LED_PIN is taken as default) on IR sending and receiving
 * Cannot disable it here!!! Use disableLEDFeedbackForReceive() or disableLEDFeedbackForSend()
 * @param aFeedbackLEDPin If aFeedbackLEDPin == 0, then take board specific FEEDBACK_LED_ON() and FEEDBACK_LED_OFF() functions
 *                        If FeedbackLEDPin == 0 and no IR_FEEDBACK_LED_PIN defined, disable LED feedback
 * @param aEnableLEDFeedback If LED_FEEDBACK_ENABLED_FOR_RECEIVE or LED_FEEDBACK_ENABLED_FOR_SEND -> enable blinking of Feedback LED
 * 
 * returns pointer to LED Feedback enable status for given pin
 */
uint8_t* setLEDFeedback(uint8_t aFeedbackLEDPin, uint8_t aEnableLEDFeedback) {

    //feedbackLEDs[aFeedbackLEDPin]; // default is 0 -> use IR_FEEDBACK_LED_PIN if available, else disable feedback

    if (aEnableLEDFeedback != DO_NOT_ENABLE_LED_FEEDBACK) {
        feedbackLEDs[aFeedbackLEDPin] |= aEnableLEDFeedback;
        if (aFeedbackLEDPin != USE_DEFAULT_FEEDBACK_LED_PIN) {
            pinMode(aFeedbackLEDPin, OUTPUT);
        } else {
#if defined(IR_FEEDBACK_LED_PIN)
            pinMode(IR_FEEDBACK_LED_PIN, OUTPUT);
#else
            feedbackLEDs[aFeedbackLEDPin] = LED_FEEDBACK_DISABLED_COMPLETELY; // we have no IR_FEEDBACK_LED_PIN available
#endif
        }
    }
}

void enableLEDFeedback(uint8_t aFeedbackLEDPin) {
    feedbackLEDs[aFeedbackLEDPin] |= LED_FEEDBACK_ENABLED_FOR_RECEIVE;
}

void disableLEDFeedback(uint8_t aFeedbackLEDPin) {
    feedbackLEDs[aFeedbackLEDPin] &= ~(LED_FEEDBACK_ENABLED_FOR_RECEIVE);
}

void enableLEDFeedbackForSend(uint8_t aFeedbackLEDPin) {
    feedbackLEDs[aFeedbackLEDPin] |= LED_FEEDBACK_ENABLED_FOR_SEND;
}

void disableLEDFeedbackForSend(uint8_t aFeedbackLEDPin) {
    feedbackLEDs[aFeedbackLEDPin] &= ~(LED_FEEDBACK_ENABLED_FOR_SEND);
}

/**
 * Flash LED while receiving or sending IR data. Does not check if enabled, this must be done by the caller.
 * Handles the 0 value of FeedbackLEDPin and the macro FEEDBACK_LED_IS_ACTIVE_LOW.
 */
void setFeedbackLED(uint8_t aFeedbackLEDPin, bool aSwitchLedOn) {
#if defined(FEEDBACK_LED_IS_ACTIVE_LOW)
    aSwitchLedOn = ~aSwitchLedOn;
#endif
    if (aFeedbackLEDPin != USE_DEFAULT_FEEDBACK_LED_PIN) {
        digitalWrite(aFeedbackLEDPin, aSwitchLedOn); // Turn user defined pin LED on/off
#if defined(IR_FEEDBACK_LED_PIN) // use fast macros here
    } else {
        digitalWrite(IR_FEEDBACK_LED_PIN, aSwitchLedOn); // Turn default pin LED on/off
#endif
    }
}

void setTxFeedbackLED(uint8_t aFeedbackLEDPin, bool aSwitchLedOn) {
    if (feedbackLEDs[aFeedbackLEDPin] & LED_FEEDBACK_ENABLED_FOR_SEND) {
        setFeedbackLED(aFeedbackLEDPin, aSwitchLedOn);
    }
}

void setRxFeedbackLED(uint8_t aFeedbackLEDPin, bool aSwitchLedOn) {
    if (feedbackLEDs[aFeedbackLEDPin] & LED_FEEDBACK_ENABLED_FOR_RECEIVE) {
        setFeedbackLED(aFeedbackLEDPin, aSwitchLedOn);
    }
}

/** @}*/

#endif // _IR_FEEDBACK_LED_HPP
