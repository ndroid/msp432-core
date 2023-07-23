/**
 * @file IRFeedbackLED.cpp
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

#include <Energia.h>
#include "IRFeedbackLED.h"

/** \addtogroup FeedbackLEDFunctions Feedback LED functions
 * @{
 */

uint8_t  rxFeedbackPin = 0;
uint8_t  txFeedbackPin = 0;
bool     rxEnableFeedback = false;
bool     txEnableFeedback = false;

void enableRXLEDFeedback(uint8_t feedbackLEDPin) {
    if (feedbackLEDPin == 0)    return;
    rxFeedbackPin = feedbackLEDPin;
    pinMode(feedbackLEDPin, OUTPUT);
    rxEnableFeedback = true;
}

void disableRXLEDFeedback(uint8_t aFeedbackLEDPin) {
    rxEnableFeedback = false;
}

void enableTXLEDFeedback(uint8_t feedbackLEDPin) {
    if (feedbackLEDPin == 0)    return;
    txFeedbackPin = feedbackLEDPin;
    pinMode(feedbackLEDPin, OUTPUT);
    txEnableFeedback = true;
}

void disableTXLEDFeedback() {
    txEnableFeedback = false;
}

