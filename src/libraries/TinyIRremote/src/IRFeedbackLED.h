/**
 * @file IRFeedbackLED.h
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
#ifndef _IR_FEEDBACK_LED_H
#define _IR_FEEDBACK_LED_H

#include <Energia.h>

/** \addtogroup FeedbackLEDFunctions Feedback LED functions
 * @{
 */

#if defined(LED_BUILTIN)
#define USE_DEFAULT_FEEDBACK_LED_PIN    LED_BUILTIN
#else
#define USE_DEFAULT_FEEDBACK_LED_PIN    0
#endif

extern uint8_t  rxFeedbackPin;
extern uint8_t  txFeedbackPin;
extern bool     rxEnableFeedback;
extern bool     txEnableFeedback;

#define RX_FEEDBACK(x)  if(rxEnableFeedback) digitalWrite(rxFeedbackPin, (x));
#define TX_FEEDBACK(x)  if(txEnableFeedback) digitalWrite(txFeedbackPin, (x));


/**
 * Enable blinking of feedback LED (LED_BUILTIN is taken as default) on IR receive. 
 *  If already enabled, LED pin setting will be overwritten to feedbackLEDPin.
 * 
 * @param feedbackLEDPin       pin number of LED for receive feedback
 */
void enableRXLEDFeedback(uint8_t feedbackLEDPin = USE_DEFAULT_FEEDBACK_LED_PIN);

/**
 * Disable blinking of feedback LED for IR receiving.
 */
void disableRXLEDFeedback();

/**
 * Enable blinking of feedback LED (LED_BUILTIN is taken as default) on IR sending. 
 *  If already enabled, LED pin setting will be overwritten to feedbackLEDPin.
 * 
 * @param feedbackLEDPin       pin number of LED for transmit feedback
 */
void enableTXLEDFeedback(uint8_t feedbackLEDPin = USE_DEFAULT_FEEDBACK_LED_PIN);

/**
 * Disable blinking of feedback LED for IR sending.
 */
void disableTXLEDFeedback();


/** @}*/

#endif // _IR_FEEDBACK_LED_H
