/*
 *  TinyNECTX.cpp
 *
 *  Transmits IR protocol data of NEC protocol by toggling PWM output.
 *  NEC is the protocol of most cheap remote controls for Arduino.
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
 *  This file adapted from sources in Arduino-IRremote https://github.com/Arduino-IRremote/Arduino-IRremote.
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

#ifndef TINY_NEC_TX_CPP_H
#define TINY_NEC_TX_CPP_H

#include <Energia.h>
#include "wiring_analog.h"

#include <ti/drivers/PWM.h>
#include <ti/drivers/pwm/PWMTimerMSP432.h>

#include "TinyNEC.h"


/** \addtogroup TinyReceiver Minimal receiver for NEC protocol
 * @{
 */

// Values defined  for timers and pins of MSP432 boards
#define NON_PWM_PIN         0
#define MAPPABLE_TIMERS     8
#define MAPPABLE_INDEXES    (PWM_MAX_MAPPABLE_INDEX + 1)

// Values defined to interpret pwmpin values - as described in PWMTimerMSP432.h
#define CCR_OFFSET          20
#define TMRID_OFFSET        16
#define VALUE_OFFSET        10
#define PORT_OFFSET         4
#define PIN_OFFSET          0
#define PIN_MASK            0x0f
#define PORT_MASK           0xf0
#define VALUE_MASK          0x3f

#define MIN_TIMER_INDEX     PMAP_TA0CCR1A

/* Pin and clock attributes for PWM timer objects.
    Pin values do not seem to be set by default for appropriate pwm
    index, so this array is referenced to update pin values.

    Refer to documentation in ti/drivers/pwm/PWMTimerMSP432.h for
     description of pwmpin values.
 */
extern PWMTimerMSP432_HWAttrsV2 pwmTimerMSP432HWAttrs[];


/* Pin number to pwmPin translation, as defined in PWMTimerMSP432.h 
 *  All pins mapped to their default timer CCR output. Mappable pins
 *  which are not the default CCR output are mapped to TA1.1 out
 *  by default. Mappable pins may be reassigned if the initial 
 *  mapped CCR output is unavailable.
 *  See IRsend::timerConfigForSend()
 */
const uint32_t pwmPinMap[] = {
    NON_PWM_PIN, /* Pin numbers start at 1 */
    NON_PWM_PIN, NON_PWM_PIN, PWMTimerMSP432_P3_2_TA1CCR1A, /* pin 3 - P3.2 (TA1.1) */
    PWMTimerMSP432_P3_3_TA1CCR1A, /* pin 4 - P3.3 (TA1.1) */
    NON_PWM_PIN, /* pins 5 */
    NON_PWM_PIN, NON_PWM_PIN, NON_PWM_PIN, NON_PWM_PIN, NON_PWM_PIN, /* pins 6-10 */
    PWMTimerMSP432_P3_6_TA1CCR1A, /* pin 11 - P3.6 (TA1.1) */
    NON_PWM_PIN, NON_PWM_PIN, NON_PWM_PIN, NON_PWM_PIN, /* pins 12-15 */
    NON_PWM_PIN, PWMTimerMSP432_P5_7_TA2CCR2A, /* pin 17 - P5.7 (TA2.2) */
    PWMTimerMSP432_P3_0_TA1CCR1A, /* pin 18 - P3.0 (TA1.1) */
    PWMTimerMSP432_P2_5_TA0CCR2A, /* pin 19 - P2.5 (TA0.2) */
    NON_PWM_PIN, NON_PWM_PIN, NON_PWM_PIN, NON_PWM_PIN, NON_PWM_PIN, /* pins 20-24 */
    NON_PWM_PIN, NON_PWM_PIN, NON_PWM_PIN, NON_PWM_PIN, NON_PWM_PIN, /* pins 25-29 */
    NON_PWM_PIN, PWMTimerMSP432_P3_7_TA1CCR1A, /* pin 31 - P3.7 (TA1.1) */
    PWMTimerMSP432_P3_5_TA1CCR1A, /* pin 32 - P3.5 (TA1.1) */
    NON_PWM_PIN, PWMTimerMSP432_P2_3_TA1CCR1A, /* pin 34 - P2.3 (TA1.1) */
    PWMTimerMSP432_P6_7_TA2CCR4A, /* pin 35 - P6.7 (TA2.4) */
    PWMTimerMSP432_P6_6_TA2CCR3A, /* pin 36 - P6.6 (TA2.3) */
    PWMTimerMSP432_P5_6_TA2CCR1A, /* pin 37 - P5.6 (TA2.1) */
    PWMTimerMSP432_P2_4_TA0CCR1A, /* pin 38 - P2.4 (TA0.1) */
    PWMTimerMSP432_P2_6_TA0CCR3A, /* pin 39 - P2.6 (TA0.3) */
    PWMTimerMSP432_P2_7_TA0CCR4A /* pin 40 - P2.7 (TA0.4) */
};

/* Only need to track use of mappable indexes (0-7), indexes 8-11 are fixed.
    Not sure how to correlate this with usage of indexes from other drivers,
    there does not appear to be a status getter for TI pwm driver.
 */
static uint32_t indexAssignedMask = 0x0;

// Track mappable timers used when allocating pwm to mappable pins. 
static uint32_t usedMappableTimers[MAPPABLE_TIMERS][2] = {
    {PWMTimerMSP432_TA0CCR1, false},  // PMAP_TA0CCR1A    20
    {PWMTimerMSP432_TA0CCR2, false},  // PMAP_TA0CCR2A    21
    {PWMTimerMSP432_TA0CCR3, false},  // PMAP_TA0CCR3A    22
    {PWMTimerMSP432_TA0CCR4, false},  // PMAP_TA0CCR4A    23
    {PWMTimerMSP432_TA1CCR1, false},  // PMAP_TA1CCR1A    24
    {PWMTimerMSP432_TA1CCR2, false},  // PMAP_TA1CCR2A    25
    {PWMTimerMSP432_TA1CCR3, false},  // PMAP_TA1CCR3A    26
    {PWMTimerMSP432_TA1CCR4, false}   // PMAP_TA1CCR4A    27
};

/* Default (only) constructor 
 *  Initializes register values based on LED_BUILTIN to avoid seg fault if begin() is not called.
 */
IRsend::IRsend() {
    this->pwmHandle = NULL;
    this->timerIndex = MAPPABLE_TIMERS;
    setRegisters(LED_BUILTIN);
}

/**
 * Initializes the send and feedback pin
 * @param aSendPin The Arduino pin number, where a IR sender diode is connected.
 * @param aEnableLEDFeedback if true, enable LED to feedback when transmitting
 * @param aLEDFeedbackPin if 0, then take board specific FEEDBACK_LED_ON() and FEEDBACK_LED_OFF() functions
 * 
 * returns true if configuration of pin for PWM succeeds
 */
bool IRsend::begin(uint8_t aSendPin, bool aEnableLEDFeedback, uint8_t aLEDFeedbackPin) {
    IR_DEBUG_PRINTLN("Initializing the send and feedback pin");
    if (aSendPin > MAX_SUPPORTED_PIN) {
        return false;
    }
    this->sendPin = aSendPin;
    pinMode(aSendPin, OUTPUT);
    if (aEnableLEDFeedback) {
        setLEDFeedback(aLEDFeedbackPin, LED_FEEDBACK_ENABLED_FOR_SEND);
    }
    this->feedbackLEDpin = aLEDFeedbackPin;
    setRegisters(aSendPin);

    return timerConfigForSend(IR_FREQUENCY_KHZ);
}

/**
 * Initializes peripheral register addresses for pin to support faster configuration changes
 * @param aSendPin The Arduino pin number, where a IR sender diode is connected.
 */
void IRsend::setRegisters(uint8_t aSendPin) {
    PinConfig *config = (PinConfig *) &GPIOMSP432_config.pinConfigs[aSendPin];
    uint32_t baseAddress = GPIO_PORT_TO_BASE[config->port];

    this->pinMask = config->pin;
    this->dirReg = (volatile uint16_t *)(baseAddress + OFS_LIB_PADIR);   //HWREG16
    this->sel0Reg = (volatile uint16_t *)(baseAddress + OFS_LIB_PASEL0);
    this->sel1Reg = (volatile uint16_t *)(baseAddress + OFS_LIB_PASEL1);
    this->outReg = (volatile uint16_t *)(baseAddress + OFS_LIB_PAOUT);
    this->renReg = (volatile uint16_t *)(baseAddress + OFS_LIB_PAREN);
}

/**
 * Send IR command defined by aIRSendData. Assumes NEC protocol.
 * @param aIRSendData The values of protocol, address, command and repeat flag are taken for sending.
 * @param aNumberOfRepeats Number of repeats to send after the initial data.
 */
size_t IRsend::write(IRData *aIRSendData, uint8_t aNumberOfRepeats) {

    if (aIRSendData->protocol == NEC) {
        IR_DEBUG_PRINTLN("sending NEC signal . . .");
        sendNEC(aIRSendData->address, aIRSendData->command, aNumberOfRepeats, aIRSendData->isRepeat);
        IR_DEBUG_PRINTLN("NEC signal sent");
        return 1;
    }
    IR_DEBUG_PRINTLN("IRsend::write - didn't recognize protocol");
    return 0;
}

/**
 * Set pin configuration to enable PWM output.
 *  Assumes PWM already configured and active for pin.
 */
void IRsend::pwmStart() {
    if(this->pwmHandle != NULL) {
        *(this->sel0Reg) |= this->sel0Mask;
        *(this->sel1Reg) |= this->sel1Mask;
    }
}

/**
 * Set pin configuration to GPIO (disable PWM output).
 *  Faster than using PWM_start() and PWM_stop() toggles.
 *  OUT and DIR registers may need to be set if not set already.
 */
void IRsend::pwmStop() {
    IR_DEBUG_PRINTLN("IRsend::pwmStop - begin");
    if(this->pwmHandle != NULL) {
        *(this->sel0Reg) &= ~(this->pinMask);
        *(this->sel1Reg) &= ~(this->pinMask);
//        *(this->outReg)  &= ~(this->pinMask);
        IR_DEBUG_PRINTLN("IRsend::pwmStop - stopped");
    }
}

/** 
 * Configure PWM device if pin and timer are available. The kHz value controls the modulation 
 *   frequency in kilohertz. If not a valid pin, or if configuration fails, pwm handler
 *   will be NULL (resulting in failure of PWM enables)
 * @param aFrequencyKHz PWM modulation frequency in kilohertz.
 * 
 * returns true if new pwmHandle successfully created and started
 */
bool IRsend::timerConfigForSend(uint8_t aFrequencyKHz) {
    IR_DEBUG_PRINTLN("IRsend::timerConfigForSend - begin");

    if (this->pwmHandle == NULL) {
        IR_DEBUG_PRINTLN("IRsend::timerConfigForSend - create handle");
        PWM_Params pwmParams;
        uint32_t pwmPin;
        this->pwmIndex = digital_pin_to_pwm_index[this->sendPin];

        PWM_init();
        PWM_Params_init(&pwmParams);
        // periodUnits default is Hz and dutyUnits default is % of PWM_DUTY_FRACTION_MAX
        pwmParams.periodValue = (uint32_t)aFrequencyKHz * 1000;
        pwmParams.dutyValue = (uint32_t)(((uint64_t)PWM_DUTY_FRACTION_MAX * IR_SEND_DUTY_CYCLE) / 100); // 30%

        if (this->sendPin <= MAX_SUPPORTED_PIN) {
            IR_DEBUG_PRINTLN("IRsend::timerConfigForSend - sendPin set");
            pwmPin = pwmPinMap[this->sendPin];
        } else {
            IR_DEBUG_PRINTLN("IRsend::timerConfigForSend - sendPin invalid");
            pwmPin = NON_PWM_PIN;
        }
        if (pwmPin == NON_PWM_PIN) {
            IR_DEBUG_PRINTLN("IRsend::timerConfigForSend - handle not set");
            return false;
        }
        IR_DEBUG_PRINT("IRsend::timerConfigForSend - original index: ");
        IR_DEBUG_PRINTLN(this->pwmIndex);

        // For non-mappable pins, digital_pin_to_pwm_index[] returns a fixed index.  For mappable pins, 
        //  pwm_index may vary (digital_pin_to_pwm_index[] returns MAPPABLE_PIN). Find an available index 
        //  (currently only identifies conflicts within library, no known support for identifying usage across libraries) 
        if (this->pwmIndex >= PWM_AVAILABLE_PWMS) {
            this->pwmIndex = MAPPABLE_INDEXES;

            // Look for a free pwm index.
            for (int i = 0; i < MAPPABLE_INDEXES; i++) {
                if (((indexAssignedMask >> i) & 1) == 0) {
                    
                    this->pwmIndex = i;     // Save the index for this instance of IR TX
                    
                    indexAssignedMask |= (1 << i);  // Mark the spot in the mask
                    
                    break;                  // Stop searching for free slots
                }
            }
            IR_DEBUG_PRINT("IRsend::timerConfigForSend - index set to ");
            IR_DEBUG_PRINTLN(this->pwmIndex);
            if (this->pwmIndex == MAPPABLE_INDEXES) {
                return false;
            }
            // For mappable pins, store default Timer/CCR. May need to change if already in use.
            this->timerIndex = ((pwmPin >> VALUE_OFFSET) & VALUE_MASK) - MIN_TIMER_INDEX;
        }

        IR_DEBUG_PRINT("IRsend::timerConfigForSend - index pin: ");
        IR_DEBUG_PRINTLN(pwmTimerMSP432HWAttrs[this->pwmIndex].pwmPin, HEX);

        // set pin for selected pwmIndex to sendPin with default Timer/CCR
        pwmTimerMSP432HWAttrs[this->pwmIndex].pwmPin = pwmPin;

        IR_DEBUG_PRINT("IRsend::timerConfigForSend - index pin updated: ");
        IR_DEBUG_PRINTLN(pwmTimerMSP432HWAttrs[this->pwmIndex].pwmPin, HEX);

        if (this->pwmIndex < MAPPABLE_INDEXES) {
            // For mappable pins, verify Timer/CCR is available. If not, search for available
            //  Timer/CCR and attempt to open until success or no other timers available
            while (this->timerIndex < MAPPABLE_TIMERS) {
                // If Timer/CCR is not is use, try to create PWM handle
                if (usedMappableTimers[this->timerIndex][1] == false) {
                    this->pwmHandle = PWM_open(this->pwmIndex, &pwmParams);
                    // whether PWM_open succeeds or fails, Timer/CCR is now known to be used
                    usedMappableTimers[this->timerIndex][1] = true; 
                }
                // If PWM handle not successfully created, try to find another Timer/CCR option
                if (this->pwmHandle == NULL) {
                    this->timerIndex = MAPPABLE_TIMERS;
                    for (int i = 0; i < MAPPABLE_TIMERS; i++) {
                        if (usedMappableTimers[i][1] == false) {
                            this->timerIndex = i;
                            // change only the Timer/CCR bits of pwmPin value
                            pwmPin = (pwmPin & (PIN_MASK | PORT_MASK)) | usedMappableTimers[i][0];
                            pwmTimerMSP432HWAttrs[this->pwmIndex].pwmPin = pwmPin;

                            IR_DEBUG_PRINT("IRsend::timerConfigForSend - index pin updated: ");
                            IR_DEBUG_PRINTLN(pwmTimerMSP432HWAttrs[this->pwmIndex].pwmPin, HEX);
                            break;
                        }
                    }
                } else {
                    break;
                }
            }
        } else {
            // For non-mappable pins, there is only one Timer/CCR option. Fingers-crossed that it succeeds
            this->pwmHandle = PWM_open(this->pwmIndex, &pwmParams);
        }
        if (this->pwmHandle == NULL) {
            IR_DEBUG_PRINTLN("IRsend::timerConfigForSend - handle is invalid");
            return false;
        } else {
            // Start the PWM module and store the SELx mask. Pin will be immediately changed back to GPIO LOW 
            //  output in IRLedOff. This is done because it is faster to change pin config than to toggle 
            //  PWM functionality using PWM_start/PWM_stop
            PWM_start(this->pwmHandle);
            this->sel0Mask = (*(this->sel0Reg) & this->pinMask);
            this->sel1Mask = (*(this->sel1Reg) & this->pinMask);
            IR_DEBUG_PRINT("IRsend::timerConfigForSend - new value for SEL0: ");
            IR_DEBUG_PRINTLN(*(this->sel0Reg), HEX);
            IR_DEBUG_PRINT("IRsend::timerConfigForSend - new value for SEL1: ");
            IR_DEBUG_PRINTLN(*(this->sel1Reg), HEX);
        }

        IR_DEBUG_PRINT("IRsend::timerConfigForSend - pin: ");
        IR_DEBUG_PRINTLN(pwmPin, HEX);
        IR_DEBUG_PRINT("IRsend::timerConfigForSend - index: ");
        IR_DEBUG_PRINTLN(this->pwmIndex);
        IR_DEBUG_PRINT("IRsend::timerConfigForSend - index pin: ");
        IR_DEBUG_PRINTLN(pwmTimerMSP432HWAttrs[this->pwmIndex].pwmPin, HEX);
        IR_DEBUG_PRINT("IRsend::timerConfigForSend - index clock: ");
        IR_DEBUG_PRINTLN(pwmTimerMSP432HWAttrs[this->pwmIndex].clockSource, HEX);
        IRLedOff();
        return true;
    } 
    return false;
}

/**
 * Sends an IR mark for the specified number of microseconds.
 *  The mark output is modulated at the PWM frequency. The output is guaranteed to be OFF / inactive
 *  after after the call of the function. This function may affect the state of feedback LED.
 * 
 * @param aMarkMicros  Length of IR mark pulse in microseconds.
 */
void IRsend::mark(unsigned int aMarkMicros) {
    setTxFeedbackLED(this->feedbackLEDpin, true);

    pwmStart(); // Enable PWM output on pin
    customDelayMicroseconds(aMarkMicros);
    IRLedOff(); // Turn off IR output
}

/**
 * Just switch the IR sending LED off to send an IR space
 *  A space is "no output", so the PWM output is disabled.
 *  This function may affect the state of feedback LED.
 */
void IRsend::IRLedOff() {
    IR_DEBUG_PRINTLN("IRsend::IRLedOff - begin");
    pwmStop(); // Disable PWM output
    *(this->outReg)  &= ~(this->pinMask);   // set output LOW

    IR_DEBUG_PRINTLN("IRsend::IRLedOff - pwm stopped");
    setTxFeedbackLED(this->feedbackLEDpin, false);
}

/**
 * Sends an IR space for the specified number of microseconds.
 * A space is "no output", so just wait.
 * @param aSpaceMicros  Length of IR space pulse in microseconds.
 */
void IRsend::space(unsigned int aSpaceMicros) {
    customDelayMicroseconds(aSpaceMicros);
}

/**
 * Custom delay function that circumvents Arduino's delayMicroseconds 16 bit limit
 * and is (mostly) not extended by the duration of interrupt codes like the millis() interrupt
 */
void IRsend::customDelayMicroseconds(unsigned long aMicroseconds) {
    unsigned long start = micros();
    // overflow invariant comparison :-)
    while ((micros() - start) < aMicroseconds) {
    }
}

/**
 * Prepares IR output. PWM should already be configured and started using timerConfigForSend(). 
 * To turn the output on and off, we leave the PWM running, but connect and disconnect the output pin.
 */
void IRsend::enableIROut() {
    IR_DEBUG_PRINTLN("IRsend::enableIROut - begin");
//    TIMER_DISABLE_RECEIVE_INTR;
    IRLedOff(); // When not sending, we want it low/inactive
    *(this->dirReg)  |= (this->pinMask);    // set pin as output
}


//+=============================================================================
/*
 * Send repeat
 * Repeat commands should be sent in a 110 ms raster.
 */
void IRsend::sendNECRepeat() {
    IR_DEBUG_PRINTLN("IRsend::sendNECRepeat - begin");
    enableIROut();
    mark(NEC_HEADER_MARK);
    space(NEC_REPEAT_HEADER_SPACE);
    mark(NEC_BIT_MARK);
//    ledOff(); // Always end with the LED off
}

/*
 * Repeat commands should be sent in a 110 ms raster.
 * There is NO delay after the last sent repeat!
 * https://www.sbprojects.net/knowledge/ir/nec.php
 * @param aIsRepeat if true, send only one repeat frame without leading and trailing space
 */
void IRsend::sendNEC(uint16_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats, bool aIsRepeat) {

    LongUnion tRawData;

    IR_DEBUG_PRINTLN("IRsend::sendNEC - begin send NEC");
    // Address 16 bit LSB first
    if ((aAddress & 0xFF00) == 0) {
        IR_DEBUG_PRINTLN("IRsend::sendNEC - 8-bit address");
        // assume 8 bit address -> send 8 address bits and then 8 inverted address bits LSB first
        tRawData.UByte.LowByte = aAddress;
        tRawData.UByte.MidLowByte = ~tRawData.UByte.LowByte;
    } else {
        IR_DEBUG_PRINTLN("IRsend::sendNEC - 16-bit address");
        tRawData.UWord.LowWord = aAddress;
    }

    // send 8 command bits and then 8 inverted command bits LSB first
    tRawData.UByte.MidHighByte = aCommand;
    tRawData.UByte.HighByte = ~aCommand;

    sendNECRaw(tRawData.ULong, aNumberOfRepeats, aIsRepeat);
}

void IRsend::sendNECRaw(uint32_t aRawData, uint8_t aNumberOfRepeats, bool aIsRepeat) {
    IR_DEBUG_PRINTLN("IRsend::sendNECRaw - begin");
    if (aIsRepeat) {
        sendNECRepeat();
        return;
    }
    // Set IR carrier frequency
    enableIROut();
    IR_DEBUG_PRINTLN("IRsend::sendNECRaw - IR enabled");

    // Header
    mark(NEC_HEADER_MARK);
    space(NEC_HEADER_SPACE);
    IR_DEBUG_PRINTLN("IRsend::sendNECRaw - header sent");

    // LSB first + stop bit
    sendPulseDistanceWidthData(NEC_BIT_MARK, NEC_ONE_SPACE, NEC_BIT_MARK, NEC_ZERO_SPACE, aRawData, NEC_BITS, 
    SEND_STOP_BIT);
    IR_DEBUG_PRINTLN("IRsend::sendNECRaw - data sent");

    for (uint8_t i = 0; i < aNumberOfRepeats; ++i) {
        IR_DEBUG_PRINTLN("IRsend::sendNECRaw - sending repeat");
        // send repeat in a 110 ms raster
        if (i == 0) {
            delay(NEC_REPEAT_SPACE / 1000);
        } else {
            delay((NEC_REPEAT_PERIOD - NEC_REPEAT_DURATION) / 1000);
        }
        // send repeat
        sendNECRepeat();
    }
}

/**
 * Sends PulseDistance data
 * The output always ends with a space
 */
void IRsend::sendPulseDistanceWidthData(unsigned int aOneMarkMicros, unsigned int aOneSpaceMicros, unsigned int aZeroMarkMicros,
        unsigned int aZeroSpaceMicros, uint32_t aData, uint8_t aNumberOfBits, bool aSendStopBit) {

    IR_DEBUG_PRINTLN("IRsend::sendPulseDistanceWidthData - IR enabled");
    // Send the Least Significant Bit (LSB) first / MSB last.
    for (uint8_t bit = 0; bit < aNumberOfBits; bit++, aData >>= 1) {
        if (aData & 1) {  // Send a 1
            TRACE_PRINT('1');
            mark(aOneMarkMicros);
            space(aOneSpaceMicros);
        } else {  // Send a 0
            TRACE_PRINT('0');
            mark(aZeroMarkMicros);
            space(aZeroSpaceMicros);
        }
    }
    if (aSendStopBit) {
        TRACE_PRINT('S');
        mark(aZeroMarkMicros); // seems like this is used for stop bits
    }
    TRACE_PRINTLN("");
}


/** @}*/

#endif // TINY_NEC_TX_CPP_H
