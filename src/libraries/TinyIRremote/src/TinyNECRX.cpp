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

#include <Energia.h>

#include <ti/drivers/gpio/GPIOMSP432.h>
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/gpio.h>

#include "TinyNECRX.h"
//#include "IRFeedbackLED.h"


/** \addtogroup TinyReceiver Minimal receiver for NEC protocol
 * @{
 */
//#define TRACE
/*
 * There are 11 8-bit ports total (1-10 and J)
 * but only 6 ports support interrupts
 */
#define NUM_INTERRUPT_PORTS 6

/**
 * Port number to peripheral register address mapping
 */
//extern const uint32_t GPIO_PORT_TO_BASE[];

extern const GPIOMSP432_Config GPIOMSP432_config;

/*
 * Array of IRreceiver object pointers for attached pins.
 * This is needed to provide reference to member function for interrupt callback. 
 */
IRreceiver* gpio_to_IRreceiver[] = {
    /* port_pin */
    NULL,  /*  0  - dummy */

    /* pins 1-10 */
    NULL,  /*  1  - 3.3V */
    NULL,  /*  2  - P6.0_A15 */
    NULL,  /*  3  - P3.2_URXD */
    NULL,  /*  4  - P3.3_UTXD */
    NULL,  /*  5  - P4.1_IO_A12 */
    NULL,  /*  6  - P4.3_A10 */
    NULL,  /*  7  - P1.5_SPICLK */
    NULL,  /*  8  - P4.6_IO_A7 */
    NULL,  /*  9  - P6.5_I2CSCL */
    NULL,  /*  10 - P6.4_I2CSDA */

    /* pins 11-20 */
    NULL,  /*  11 - P3.6_IO */
    NULL,  /*  12 - P5.2_IO */
    NULL,  /*  13 - P5.0_IO */
    NULL,  /*  14 - P1.7_SPIMISO */
    NULL,  /*  15 - P1.6_SPIMOSI */
    NULL,  /*  16 - RESET */
    NULL,  /*  17 - P5.7_IO */
    NULL,  /*  18 - P3.0_IO */
    NULL,  /*  19 - P2.5_IO_PWM */
    NULL,  /*  20 - GND */

    /* pins 21-30 */
    NULL,  /*  21 - 5V */
    NULL,  /*  22 - GND */
    NULL,  /*  23 - P6.1_A14 */
    NULL,  /*  24 - P4.0_A13 */
    NULL,  /*  25 - P4.2_A11 */
    NULL,  /*  26 - P4.4_A9 */
    NULL,  /*  27 - P4.5_A8 */
    NULL,  /*  28 - P4.7_A6 */
    NULL,  /*  29 - P5.4_IO */
    NULL,  /*  30 - P5.5_IO */

    /* pins 31-40 */
    NULL,  /*  31 - P3.7_IO */
    NULL,  /*  32 - P3.5_IO */
    NULL,  /*  33 - P5.1_IO */
    NULL,  /*  34 - P2.3_IO */
    NULL,  /*  35 - P6.7_IO_CAPT */
    NULL,  /*  36 - P6.6_IO_CAPT */
    NULL,  /*  37 - P5.6_PWM */
    NULL,  /*  38 - P2.4_PWM */
    NULL,  /*  39 - P2.6_PWM */
    NULL,  /*  40 - P2.7_PWM */

    /* pins 41-56 */
    NULL,  /*  41 - P8.5 */
    NULL,  /*  42 - P9.0 */
    NULL,  /*  43 - P8.4 */
    NULL,  /*  44 - P8.2 */
    NULL,  /*  45 - P9.2 */
    NULL,  /*  46 - P6.2 */
    NULL,  /*  47 - P7.3 */
    NULL,  /*  48 - P7.1 */
    NULL,  /*  49 - P9.4 */
    NULL,  /*  40 - P9.6 */
    NULL,  /*  51 - P8.0 */
    NULL,  /*  52 - P7.4 */
    NULL,  /*  53 - P7.6 */
    NULL,  /*  54 - P10.0 */
    NULL,  /*  55 - P10_2 */
    NULL,  /*  56 - P10.4 */

    /* pins 57-72 */
    NULL,  /*  57 - P8.6 */
    NULL,  /*  58 - P8.7 */
    NULL,  /*  59 - P9.1 */
    NULL,  /*  60 - P8.3 */
    NULL,  /*  61 - P5.3 */
    NULL,  /*  62 - P9.3 */
    NULL,  /*  63 - P6.3 */
    NULL,  /*  64 - P7.2 */
    NULL,  /*  65 - P7.0 */
    NULL,  /*  66 - P9.5 */
    NULL,  /*  67 - P9.7 */
    NULL,  /*  68 - P7.5 */
    NULL,  /*  69 - P7.7 */
    NULL,  /*  70 - P10.1 */
    NULL,  /*  71 - P10.3 */
    NULL,  /*  72 - P10.5 */

    /* virtual pins 73-78 */
    NULL,  /*  73 - P1.1 SW1 */
    NULL,  /*  74 - P1.4 SW2 */
    NULL,  /*  75 - P2.0 RED_LED */
    NULL,  /*  76 - P2.1 GREEN_LED */
    NULL,  /*  77 - P2.2 BLUE_LED */
    NULL,  /*  78 - P1.0 LED1 */
};

/**
 * IR receiver is defined as a single instance because of the limitation of the interrupt handler.
 *  The ISR must know the Port/Pin in order to change the interrupt edge and clear the flag, and there 
 *  is no efficient means for passing this to multiple ISR instances. Therefore, only one may be defined.
 */
//TinyIRReceiverStruct TinyIRReceiverControl;

//static uint8_t feedbackLEDpin; 

/**
 * Static ISR to call to receiver object private method.
 * 
 * \param index     pin number of pin which generated interrupt
 */
void IRPinChangeInterruptHandler(uint_least8_t index) {
    IRreceiver* receiver = gpio_to_IRreceiver[index];
    if (receiver == NULL)   return;
    receiver->interruptHandler();
}

void IRreceiver::interruptHandler() {
#if defined(IR_MEASURE_TIMING) && defined(IR_TIMING_TEST_PIN)
    digitalWrite(IR_TIMING_TEST_PIN, HIGH); // 2 clock cycles
#endif
    /*
     * Save IR input level
     * Negative logic, true / HIGH means inactive / IR space, LOW / false means IR mark.
     */
    uint8_t tIRLevel = digitalRead(this->rcvPin);

    // Toggle triggering edge and clear flag (workaround to MSP432 limitation of not triggering on CHANGE)
    *(this->iesReg) ^= this->config->pin;
    *(this->ifgReg) &= ~(this->config->pin);

    RX_FEEDBACK(!tIRLevel);

    /*
     * compute microseconds after last change
     */
    uint32_t tCurrentMicros = micros();
    uint16_t tMicrosOfMarkOrSpace = tCurrentMicros - this->LastChangeMicros;
    this->LastChangeMicros = tCurrentMicros;

    uint8_t tState = this->IRReceiverState;

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
                this->IRRawDataBitCounter = 0;
                this->IRRawData.ULong = 0;
                this->IRRawDataMask = 1;
                this->IRRepeatDetected = false;
                tState = IR_RECEIVER_STATE_WAITING_FOR_DATA_SPACE;
            } else if (tMicrosOfMarkOrSpace >= lowerValue25Percent(NEC_REPEAT_HEADER_SPACE)
                    && tMicrosOfMarkOrSpace <= upperValue25Percent(NEC_REPEAT_HEADER_SPACE)
                    && this->IRRawDataBitCounter >= NEC_BITS) {
                /*
                 * We have a repeat header here and no broken receive before -> set repeat flag
                 */
                this->IRRepeatDetected = true;
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
                    this->IRRawData.ULong |= this->IRRawDataMask;
                } else {
                    // we received a 0 - empty code for documentation
                }
                // prepare for next bit
                this->IRRawDataMask = this->IRRawDataMask << 1;
                this->IRRawDataBitCounter++;
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
                if ((this->IRRawDataBitCounter >= NEC_BITS) || this->IRRepeatDetected) {
                    /*
                     * Code complete -> call callback, no parity check!
                     */
                    // Reset state for new start
                    tState = IR_RECEIVER_STATE_WAITING_FOR_START_MARK;
                    interrupts();
                    /*
                     * Address reduction to 8 bit
                     */
                    if (this->IRRawData.UByte.LowByte
                            == (uint8_t) (~(this->IRRawData.UByte.MidLowByte))) {
                        // standard 8 bit address NEC protocol
                        this->IRRawData.UByte.MidLowByte = 0; // Address is the first 8 bit
                    }

//#if defined(HANDLE_IR_EVENT)
                    if (this->enCallback) {
                        /*
                        * Call user provided callback here
                        */
                        this->callback(this->IRRawData.UWord.LowWord,
                                this->IRRawData.UByte.MidHighByte, this->IRRepeatDetected);
                    } else {
                        this->newCommandAvailable = this->incRepeats || !(this->IRRepeatDetected);
//#endif
//#if !defined(EXCLUDE_REPEATS)
//                    this->newCommandAvailable = TRUE;
//#else
                    // Only registers that new data is available if it is not a repeat
//                    this->newCommandAvailable = !(this->IRRepeatDetected);
//#endif
                    }
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

    this->IRReceiverState = tState;
#if defined(IR_MEASURE_TIMING) && defined(IR_TIMING_TEST_PIN)
    digitalWrite(IR_TIMING_TEST_PIN, LOW); // 2 clock cycles
#endif
#ifdef DEBUG
    uint32_t isrTime = micros();
    IR_DEBUG_PRINT("IR receiver ISR time (us): ");
    IR_DEBUG_PRINTLN((isrTime - tCurrentMicros));
#endif
}

IRreceiver::IRreceiver(uint8_t receivePin) {
    this->rcvPin = receivePin;
    if (receivePin < GPIOMSP432_config.numberOfPinConfigs) {
        this->config = (PinConfig *) &GPIOMSP432_config.pinConfigs[receivePin];
        // store references to registers for quick updates in ISR
        uint32_t baseAddress = GPIO_PORT_TO_BASE[this->config->port];
//        pinMask = config->pin;
        this->iesReg = (volatile uint8_t *)(baseAddress + OFS_LIB_PAIES);   //HWREG16
        this->ifgReg = (volatile uint8_t *)(baseAddress + OFS_LIB_PAIFG);
    } else {
        this->config = NULL;
        this->iesReg = NULL;
        this->ifgReg = NULL;
    }
    this->newCommandAvailable = FALSE;
    this->incRepeats = false;
    this->enCallback = false;
    this->callback = NULL;
    this->initialized = false;
}

bool IRreceiver::initIRReceiver(bool includeRepeats, bool enableCallback, 
                        void (*callbackFunction)(uint16_t , uint8_t , bool)) {
    if (enableCallback) { 
        if (callbackFunction == NULL) {
            return false;
        } else {
            this->callback = (IRData_CallbackFxn)callbackFunction;
        }
    }
    this->incRepeats = includeRepeats;
    this->enCallback = enableCallback;
    
    if(this->initialized) {
        return true;
    }

    if (this->rcvPin >= GPIOMSP432_config.numberOfPinConfigs) {
        return false;
    }
    if (this->config->port == INVALID_PIN_PORT) {
        return false;
    }
    /*
     * Only ports 1-6 are interrupt capable;
     * return if (port > NUM_INTERRUPT_PORTS)
     */
    if (this->config->port > NUM_INTERRUPT_PORTS) {
        return false;
    }
    IR_DEBUG_PRINT("IR receiver port ");
    IR_DEBUG_PRINTLN(this->config->port);
    IR_DEBUG_PRINT("IR receiver pin mask ");
    IR_DEBUG_PRINTLN(this->config->pin);

#if defined(IR_MEASURE_TIMING) && defined(IR_TIMING_TEST_PIN)
    pinMode(IR_TIMING_TEST_PIN, OUTPUT);
#endif

    pinMode(this->rcvPin, INPUT_PULLUP);

    // attach ISR for falling-edge on IR receiver pin (bug in MSP432 drivers 
    //  prevents triggering on both edges, so have to swap triggering edge in ISR)
    attachInterrupt(this->rcvPin, (void (*)())IRPinChangeInterruptHandler, FALLING); 
    gpio_to_IRreceiver[this->rcvPin] = this;
    this->initialized = true;
    return true;
}

bool IRreceiver::newCommandReceived() {
    if (this->newCommandAvailable) {
        this->newCommandAvailable = FALSE;
        return TRUE;
    }
    return FALSE;
}

bool IRreceiver::decodeIR(IRData *results) {
    
    if (this->newCommandAvailable) {
        results->protocol = NEC;
        results->address = this->IRRawData.UWord.LowWord;
        results->command = this->IRRawData.UByte.MidHighByte;
        results->isRepeat = this->IRRepeatDetected;
        this->newCommandAvailable = FALSE;
        
        return TRUE;
    }
    return FALSE;
}

uint16_t IRreceiver::getAddress() {
    return this->IRRawData.UWord.LowWord;
}

uint16_t IRreceiver::getCommand() {
    return this->IRRawData.UByte.MidHighByte;
}

bool IRreceiver::getIsRepeat() {
    return this->IRRepeatDetected;
}

/** @}*/
