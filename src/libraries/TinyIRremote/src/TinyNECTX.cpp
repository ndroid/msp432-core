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

#include <Energia.h>
#include <ti/runtime/wiring/wiring_private.h>
#include "wiring_analog.h"

#include <ti/drivers/PWM.h>
#include <ti/drivers/pwm/PWMTimerMSP432.h>
#include <ti/drivers/gpio/GPIOMSP432.h>

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/gpio.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>
#include <ti/sysbios/family/arm/msp432/Timer.h>

#include "TinyNECTX.h"
//#include "IRFeedbackLED.h"
#include "LongUnion.h"


/** \addtogroup TinyReceiver Minimal receiver for NEC protocol
 * @{
 */

// Values defined  for timers and pins of MSP432 boards
/*
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
*/
//#define MIN_TIMER_INDEX     PMAP_TA0CCR1A
#define TIMER_CCR_COUNT         4
#define TIMER2_INDEX            2
#define INVALID_TIMER_INDEX     0xff

// default Timer_A frequency in Energia is 12MHz and IR frequency is 38 kHz
#define PWM_PERIOD_IN_COUNTS    (12000000 / 38000)
// duty cycle set to 30% for reduced power consumption
#define PWM_DUTY_IN_COUNTS      ((PWM_PERIOD_IN_COUNTS * 3 / 10) + 1)

/* Pin and clock attributes for PWM timer objects.
    Pin values do not seem to be set by default for appropriate pwm
    index, so this array is referenced to update pin values.

    Refer to documentation in ti/drivers/pwm/PWMTimerMSP432.h for
     description of pwmpin values.
 */
extern PWMTimerMSP432_HWAttrsV2 pwmTimerMSP432HWAttrs[];

/**
 * Port number to peripheral register address mapping
 */
//extern const uint32_t GPIO_PORT_TO_BASE[];

extern uint32_t fixed_map_pwm_pins[];
extern uint32_t mapped_pwm_pin_ccrs[];

extern const GPIOMSP432_Config GPIOMSP432_config;

static uint8_t timerIndex = INVALID_TIMER_INDEX;
static uint8_t senderCnt = 0;

/* Pin number to pwmPin translation, as defined in PWMTimerMSP432.h 
 *  All pins mapped to their default timer CCR output. Mappable pins
 *  which are not the default CCR output are mapped to TA1.1 out
 *  by default. Mappable pins may be reassigned if the initial 
 *  mapped CCR output is unavailable.
 *  See IRsend::timerConfigForSend()
 */

// const uint32_t pwmPinMap[] = {
//     NON_PWM_PIN, /* Pin numbers start at 1 */
//     NON_PWM_PIN, NON_PWM_PIN, PWMTimerMSP432_P3_2_TA1CCR1A, /* pin 3 - P3.2 (TA1.1) */
//     PWMTimerMSP432_P3_3_TA1CCR1A, /* pin 4 - P3.3 (TA1.1) */
//     NON_PWM_PIN, /* pins 5 */
//     NON_PWM_PIN, NON_PWM_PIN, NON_PWM_PIN, NON_PWM_PIN, NON_PWM_PIN, /* pins 6-10 */
//     PWMTimerMSP432_P3_6_TA1CCR1A, /* pin 11 - P3.6 (TA1.1) */
//     NON_PWM_PIN, NON_PWM_PIN, NON_PWM_PIN, NON_PWM_PIN, /* pins 12-15 */
//     NON_PWM_PIN, PWMTimerMSP432_P5_7_TA2CCR2A, /* pin 17 - P5.7 (TA2.2) */
//     PWMTimerMSP432_P3_0_TA1CCR1A, /* pin 18 - P3.0 (TA1.1) */
//     PWMTimerMSP432_P2_5_TA0CCR2A, /* pin 19 - P2.5 (TA0.2) */
//     NON_PWM_PIN, NON_PWM_PIN, NON_PWM_PIN, NON_PWM_PIN, NON_PWM_PIN, /* pins 20-24 */
//     NON_PWM_PIN, NON_PWM_PIN, NON_PWM_PIN, NON_PWM_PIN, NON_PWM_PIN, /* pins 25-29 */
//     NON_PWM_PIN, PWMTimerMSP432_P3_7_TA1CCR1A, /* pin 31 - P3.7 (TA1.1) */
//     PWMTimerMSP432_P3_5_TA1CCR1A, /* pin 32 - P3.5 (TA1.1) */
//     NON_PWM_PIN, PWMTimerMSP432_P2_3_TA1CCR1A, /* pin 34 - P2.3 (TA1.1) */
//     PWMTimerMSP432_P6_7_TA2CCR4A, /* pin 35 - P6.7 (TA2.4) */
//     PWMTimerMSP432_P6_6_TA2CCR3A, /* pin 36 - P6.6 (TA2.3) */
//     PWMTimerMSP432_P5_6_TA2CCR1A, /* pin 37 - P5.6 (TA2.1) */
//     PWMTimerMSP432_P2_4_TA0CCR1A, /* pin 38 - P2.4 (TA0.1) */
//     PWMTimerMSP432_P2_6_TA0CCR3A, /* pin 39 - P2.6 (TA0.3) */
//     PWMTimerMSP432_P2_7_TA0CCR4A /* pin 40 - P2.7 (TA0.4) */
// };

/* Only need to track use of mappable indexes (0-7), indexes 8-11 are fixed.
    Not sure how to correlate this with usage of indexes from other drivers,
    there does not appear to be a status getter for TI pwm driver.
 */
//static uint32_t indexAssignedMask = 0x0;

// Track mappable timers used when allocating pwm to mappable pins. 
/*
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
*/
IRsender::IRsender(uint8_t sendPin) {
    this->txPin = sendPin;
    if (sendPin < GPIOMSP432_config.numberOfPinConfigs) {
        this->config = (PinConfig *) &GPIOMSP432_config.pinConfigs[sendPin];
        // store references to registers for quick updates when transmitting
        uint32_t baseAddress = GPIO_PORT_TO_BASE[this->config->port];
        this->dirReg = (volatile uint8_t *)(baseAddress + OFS_LIB_PADIR);   //HWREG16
        this->sel0Reg = (volatile uint8_t *)(baseAddress + OFS_LIB_PASEL0);
        this->sel1Reg = (volatile uint8_t *)(baseAddress + OFS_LIB_PASEL1);
        this->outReg = (volatile uint8_t *)(baseAddress + OFS_LIB_PAOUT);
        this->renReg = (volatile uint8_t *)(baseAddress + OFS_LIB_PAREN);
    } else {
        this->config = NULL;
    }
    this->pwmHandle = NULL;
}

bool IRsender::initIRSender() {
    IR_DEBUG_PRINTLN("Initializing the send and feedback pin");
    if (pwmHandle != NULL) {    // can only initialize once
        return true;
    }
    if (this->config == NULL) { // invalid pin
        return false;
    }
    if (this->config->port == INVALID_PIN_PORT) {   // invalid port
        return false;
    }
    if (senderCnt >= TIMER_CCR_COUNT) { // already using all four CCRs of timer
        return false;
    }

    return timerConfigForSend();
}

size_t IRsender::write(IRData *irSendData, uint8_t numberOfRepeats) {
    if(this->pwmHandle == NULL) {
        return 0;
    }

    if (irSendData->protocol == NEC) {
        IR_DEBUG_PRINTLN("sending NEC signal . . .");
        sendNEC(irSendData->address, irSendData->command, numberOfRepeats, 
                                        irSendData->isRepeat);
        IR_DEBUG_PRINTLN("NEC signal sent");
        return 1;
    }
    IR_DEBUG_PRINTLN("IRsend::write - didn't recognize protocol");
    return 0;
}

void IRsender::pwmStart() {
    *(this->sel0Reg) |= this->sel0Mask;
    *(this->sel1Reg) |= this->sel1Mask;
}

void IRsender::pwmStop() {
    IR_DEBUG_PRINTLN("IRsend::pwmStop - begin");
    *(this->sel0Reg) &= ~(this->config->pin);
    *(this->sel1Reg) &= ~(this->config->pin);
//        *(this->outReg)  &= ~(this->pinMask);
    IR_DEBUG_PRINTLN("IRsend::pwmStop - stopped");
}

bool IRsender::timerConfigForSend() {
    uint16_t pinId, pinNum;
    uint_fast8_t port;
    uint_fast16_t pinMask;
    uint32_t hwiKey;
    uint32_t pwmPin;
    uint8_t timerAvailMask;
    IR_DEBUG_PRINTLN("IRsend::timerConfigForSend - begin");

    hwiKey = Hwi_disable();
    timerAvailMask = Timer_getAvailMask();

    /* idempotent */
    PWM_init();

    /*
     * The pwmIndex fetched from the pin_to_pwm_index[] table is either an 
     * actual index into the PWM instance table if the pin has already been 
     * mapped to a PWM resource, or a mappable port/pin ID, or NOT_MAPPABLE.
     */
    this->pwmIndex = digital_pin_to_pwm_index[this->txPin];

    IR_DEBUG_PRINT("IRsend::timerConfigForSend - original index: ");
    IR_DEBUG_PRINTLN(this->pwmIndex);
    if (this->pwmIndex == PWM_NOT_MAPPABLE) {
        Hwi_restore(hwiKey);
        return false; /* can't get there from here */
    }

    IR_DEBUG_PRINTLN("IRsend::timerConfigForSend - find available Timer");
    // find available Timer for PWM
    if (timerIndex == INVALID_TIMER_INDEX) {
        if (this->pwmIndex < PWM_AVAILABLE_PWMS) { /* fixed mapping */
            if (timerAvailMask & (1 << TIMER2_INDEX)) {
                timerIndex = TIMER2_INDEX;
            }
        } else {
            // find an unused mappable PWM resource and port map it 
            for (int tmr = 0; tmr < TIMER2_INDEX; tmr++) {
                if (timerAvailMask & (1 << tmr)) {
                    timerIndex = tmr;
                    break;
                }
            }
        }
        if (timerIndex == INVALID_TIMER_INDEX) {
            Hwi_restore(hwiKey);
            return false; // no available timer matching pin
        }
    }

    IR_DEBUG_PRINTLN("IRsend::timerConfigForSend - find valid pwmIndex");
    // find valid pwmIndex and HwAttrs configuration value for pin
    if (this->pwmIndex < PWM_AVAILABLE_PWMS) { /* fixed mapping */
        if (timerIndex != TIMER2_INDEX) {
            Hwi_restore(hwiKey);
            return false; // another Timer already reserved for IR transmit
        }

        // plug pwmPin in HwAttrs with corresponding encoded pin identifier 
        pwmPin = fixed_map_pwm_pins[this->pwmIndex - (PWM_MAX_MAPPABLE_INDEX + 1)];
    } else {
        if (timerIndex >= TIMER2_INDEX) {
            Hwi_restore(hwiKey);
            return false; // another Timer already reserved for IR transmit
        }

        this->pwmIndex = (TIMER_CCR_COUNT * timerIndex) + senderCnt;

        pinId = *(uint32_t *)(this->config) & 0xffff;
        port = pinId >> 8;
        pinMask = pinId & 0xff;
        pinNum = 0;
        while (((1 << pinNum) & pinMask) == 0)
            pinNum++;

        // encode pwmPin field with port/pin/TAxCCRyA info 
        pwmPin = port << 4 | pinNum;
        pwmPin = pwmPin | mapped_pwm_pin_ccrs[this->pwmIndex];
    }
    IR_DEBUG_PRINT("IRsend::timerConfigForSend - index set to ");
    IR_DEBUG_PRINTLN(this->pwmIndex);

    pwmTimerMSP432HWAttrs[this->pwmIndex].pwmPin = pwmPin;

    // re-configure pin if possible 
    PWM_Params pwmParams;

    PWM_Params_init(&pwmParams);

    // Open the PWM port 
    pwmParams.periodUnits = PWM_PERIOD_COUNTS;
    pwmParams.periodValue = PWM_PERIOD_IN_COUNTS; // 38kHz 
    pwmParams.dutyUnits = PWM_DUTY_COUNTS;
    pwmParams.dutyValue = PWM_DUTY_IN_COUNTS; // 30%

    // PWM_open() will fail if the timer's CCR is already in use 
    this->pwmHandle = PWM_open(this->pwmIndex, &pwmParams);

    // Assume that the timer was already in use by someone else.
    if (this->pwmHandle == NULL) {
        // leave timer unassigned if PWM allocation fails
        if (senderCnt == 0) {
            timerIndex = INVALID_TIMER_INDEX;
        }
        Hwi_restore(hwiKey);
        return false;
    }

    /* 
     * Start the PWM module and store the SELx mask. Pin will be immediately 
     *  changed back to GPIO LOW output in IRLedOff. This is done because it 
     *  is faster to change pin config than to toggle PWM functionality using 
     *  PWM_start/PWM_stop
     */
    PWM_start(this->pwmHandle);
    this->sel0Mask = (*(this->sel0Reg) & this->config->pin);
    this->sel1Mask = (*(this->sel1Reg) & this->config->pin);
    IR_DEBUG_PRINT("IRsend::timerConfigForSend - new value for SEL0: ");
    IR_DEBUG_PRINTLN(*(this->sel0Reg), HEX);
    IR_DEBUG_PRINT("IRsend::timerConfigForSend - new value for SEL1: ");
    IR_DEBUG_PRINTLN(*(this->sel1Reg), HEX);

    /* remove timer from pool of available timers if not in use */
    if (senderCnt == 0) {
        Timer_setAvailMask(Timer_getAvailMask() & ~(1 << timerIndex));
    }
    senderCnt++;

    digital_pin_to_pin_function[this->txPin] = PIN_FUNC_ANALOG_OUTPUT;

    Hwi_restore(hwiKey);

//    PWM_setDuty((PWM_Handle)&(PWM_config[this->pwmIndex]), 
//                        ((uint32_t) val * PWM_PERIOD_IN_COUNTS / pwmMaxDuty));

    IRLedOff();
    return true;
}

void IRsender::mark(unsigned int markMicros) {
    TX_FEEDBACK(HIGH);

    pwmStart(); // Enable PWM output on pin
    customDelayMicroseconds(markMicros);
    IRLedOff(); // Turn off IR output
}

void IRsender::IRLedOff() {
    IR_DEBUG_PRINTLN("IRsend::IRLedOff - begin");
    pwmStop(); // Disable PWM output
    *(this->outReg)  &= ~(this->config->pin);   // set output LOW

    IR_DEBUG_PRINTLN("IRsend::IRLedOff - pwm stopped");
    TX_FEEDBACK(LOW);
}

void IRsender::space(unsigned int spaceMicros) {
    customDelayMicroseconds(spaceMicros);
}

void IRsender::customDelayMicroseconds(unsigned long aMicroseconds) {
    unsigned long start = micros();
    // overflow invariant comparison :-)
    while ((micros() - start) < aMicroseconds) {
    }
}

void IRsender::enableIROut() {
    IR_DEBUG_PRINTLN("IRsend::enableIROut - begin");
    IRLedOff();         // When not sending, we want it low/inactive
    *(this->dirReg) |= (this->config->pin);     // set pin as output
}


//+=============================================================================
void IRsender::sendNECRepeat() {
    IR_DEBUG_PRINTLN("IRsend::sendNECRepeat - begin");
    enableIROut();
    mark(NEC_HEADER_MARK);
    space(NEC_REPEAT_HEADER_SPACE);
    mark(NEC_BIT_MARK);
//    ledOff(); // Always end with the LED off
}

void IRsender::sendNEC(uint16_t address, uint8_t command, uint8_t numberOfRepeats, bool isRepeat) {

    LongUnion tRawData;

    IR_DEBUG_PRINTLN("IRsend::sendNEC - begin send NEC");
    // Address 16 bit LSB first
    if ((address & 0xFF00) == 0) {
        IR_DEBUG_PRINTLN("IRsend::sendNEC - 8-bit address");
        // assume 8 bit address -> send 8 address bits and then 8 inverted address bits LSB first
        tRawData.UByte.LowByte = address;
        tRawData.UByte.MidLowByte = ~tRawData.UByte.LowByte;
    } else {
        IR_DEBUG_PRINTLN("IRsend::sendNEC - 16-bit address");
        tRawData.UWord.LowWord = address;
    }

    // send 8 command bits and then 8 inverted command bits LSB first
    tRawData.UByte.MidHighByte = command;
    tRawData.UByte.HighByte = ~command;

    sendNECRaw(tRawData.ULong, numberOfRepeats, isRepeat);
}

void IRsender::sendNECRaw(uint32_t rawData, uint8_t numberOfRepeats, bool isRepeat) {
    IR_DEBUG_PRINTLN("IRsend::sendNECRaw - begin");
    if (isRepeat) {
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
    sendPulseDistanceWidthData(NEC_BIT_MARK, NEC_ONE_SPACE, NEC_BIT_MARK, 
                            NEC_ZERO_SPACE, rawData, NEC_BITS, SEND_STOP_BIT);
    IR_DEBUG_PRINTLN("IRsend::sendNECRaw - data sent");

    for (uint8_t i = 0; i < numberOfRepeats; ++i) {
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

void IRsender::sendPulseDistanceWidthData(unsigned int oneMarkMicros, 
                unsigned int oneSpaceMicros, unsigned int zeroMarkMicros, 
                unsigned int zeroSpaceMicros, uint32_t data, 
                uint8_t numberOfBits, bool sendStopBit) {

    IR_DEBUG_PRINTLN("IRsend::sendPulseDistanceWidthData - IR enabled");
    // Send the Least Significant Bit (LSB) first / MSB last
    for (uint8_t bit = 0; bit < numberOfBits; bit++, data >>= 1) {
        if (data & 1) {  // Send a 1
            TRACE_PRINT('1');
            mark(oneMarkMicros);
            space(oneSpaceMicros);
        } else {  // Send a 0
            TRACE_PRINT('0');
            mark(zeroMarkMicros);
            space(zeroSpaceMicros);
        }
    }
    if (sendStopBit) {
        TRACE_PRINT('S');
        mark(zeroMarkMicros); // seems like this is used for stop bits
    }
    TRACE_PRINTLN("");
}


/** @}*/
