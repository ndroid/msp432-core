/**
 * @file TinyNECTX.h
 *
 * @brief Minimal IR transmitter for NEC protocol. 
 * 
 * Transmits IR protocol data of NEC protocol by toggling PWM output.
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

#include <ti/drivers/PWM.h>

#include "TinyNEC.h"
#include "IRData.h"
#include "IRFeedbackLED.h"

/** \addtogroup TinyIRSender Minimal IR transmitter for NEC protocol
 * @{
 *  Minimal IR transmitter for NEC protocol. See IRsender class for details.
 */

/** \cond   Just for better readability of code */
#define NO_REPEATS              0
#define SEND_STOP_BIT           true
#define SEND_NO_STOP_BIT        false
#define SEND_REPEAT_COMMAND     true ///< used for e.g. NEC, where a repeat is different from just repeating the data.
/** \endcond */

#if ! defined(IR_SEND_DUTY_CYCLE)
/**
 * Duty cycle in percent for sent signals.
 * 30 saves power and is compatible to the old existing code.
 */
#define IR_SEND_DUTY_CYCLE 30 
#endif

/**
 * Main class for sending IR signals
 * 
 * Transmits IR protocol data of NEC protocol using PWM output. Pin configuration 
 *  is toggled from CCRx output to GPIO output for faster toggle between marks and 
 *  spaces. Up to four IR transmit objects may be declared as type IRsender, but
 *  each must use the same timer (determined by first sender initialized).
 *  
 *  May declare up to _four_ transmitters which use a mappable PWM timer (Ports 
 *      2, 3, and 7 are mappable and may be connected to Timer_A0 or Timer_A1), 
 *      **OR** up to _four_ transmitters which use timer TA2 pins.
 * 
 *  Supported send pins include:
 * 
 *      pin#        port.pin     timer
 *       3           P3.2        mappable
 *       4           P3.3        mappable
 *       11          P3.6        mappable
 *       17          P5.7        TA2.2
 *       18          P3.0        mappable
 *       19          P2.5        TA0.2/mappable
 *       31          P3.7        mappable
 *       32          P3.5        mappable
 *       34          P2.3        TA1.1/mappable
 *       35          P6.7        TA2.4
 *       36          P6.6        TA2.3
 *       37          P5.6        TA2.1
 *       38          P2.4        TA0.1/mappable
 *       39          P2.6        TA0.3/mappable
 *       40          P2.7        TA0.4/mappable
 *      * * * * *  LED pins  * * * * * * * *
 *       75          P2.0        mappable (RED_LED)
 *       76          P2.1        mappable (GREEN_LED)
 *       77          P2.2        mappable (BLUE_LED)
 *      
 *      * * *  MSP432P401R only  * * * * * *
 *       47          P7.3        mappable
 *       48          P7.1        mappable
 *       52          P7.4        mappable
 *       53          P7.6        mappable
 *       64          P7.2        mappable
 *       65          P7.0        mappable
 *       68          P7.5        mappable
 *       69          P7.7        mappable
 * 
 */
class IRsender {
public:
    /** 
     * Default (only) constructor. Must call initIRSender() and verify success
     *  before using object.
     * 
     * @param sendPin   The Arduino pin number, where an IR LED is connected.
     */
    IRsender(uint8_t sendPin);

    /**
     * Initializes hardware for PWM output on sender pin. 
     * 
     * @return true if configuration of pin for PWM output succeeds
     */
    bool initIRSender();

    /**
     * Send IR command defined by irSendData. Assumes NEC protocol.
     * 
     * @param irSendData The values of protocol, address, command and repeat flag are taken for sending.
     * @param numberOfRepeats Number of repeats to send after the initial data.
     * 
     * @return 1 if data transmit succeeds
     */
    size_t write(IRData *irSendData, uint8_t numberOfRepeats = NO_REPEATS);

private:
    /**
     * Prepares IR output. PWM should already be configured and started using 
     *  timerConfigForSend(). To turn the output on and off, we leave the PWM 
     *  running, but connect and disconnect the output pin.
     */
    void enableIROut();

    /**
     * Sends PulseDistance data.
     * The output always ends with a space.
     * 
     * @param oneMarkMicros     Length of IR logic 1 mark pulse in microseconds.
     * @param oneSpaceMicros    Length of IR logic 1 space pulse in microseconds.
     * @param zeroMarkMicros    Length of IR logic 0 mark pulse in microseconds.
     * @param zeroSpaceMicros   Length of IR logic 0 space pulse in microseconds.
     * @param data              Payload data of IR command.
     * @param numberOfBits      Number of bits of data payload.
     * @param sendStopBit       If true, include zero mark stop bit at end.
     */
    void sendPulseDistanceWidthData(unsigned int oneMarkMicros, unsigned int oneSpaceMicros, 
                unsigned int zeroMarkMicros, unsigned int zeroSpaceMicros, uint32_t data, 
                uint8_t numberOfBits, bool sendStopBit);

    /**
     * Sends an IR mark for the specified number of microseconds.
     *  The mark output is modulated at the PWM frequency. The output is 
     *  guaranteed to be OFF / inactive after after the call of the 
     *  function. This function may affect the state of feedback LED.
     * 
     * @param markMicros  Length of IR mark pulse in microseconds.
     */
    void mark(unsigned int markMicros);
    /**
     * Sends an IR space for the specified number of microseconds.
     * A space is "no output", so just wait.
     * 
     * @param spaceMicros  Length of IR space pulse in microseconds.
     */
    static void space(unsigned int spaceMicros);
    /**
     * Just switch the IR sending LED off to send an IR space
     *  A space is "no output", so the PWM output is disabled. 
     *  This function may affect the state of feedback LED.
     */
    void IRLedOff();
    
    /**
     * Set pin configuration to enable PWM output.
     *  Assumes PWM already configured and active for pin.
     */
    void pwmStart();
    /**
     * Set pin configuration to GPIO (disable PWM output).
     *  Faster than using PWM_start() and PWM_stop() toggles.
     *  OUT and DIR registers may need to be set if not set already.
     */
    void pwmStop();
    /** 
     * Configure PWM device if pin and timer are available. IR signal uses 38kHz modulation 
     *   frequency. If not a valid pin, or if configuration fails, pwm handler
     *   will be NULL (resulting in failure of PWM enables). This method mirrors the
     *   analogWrite() method to ensure a dedicated timer is reserved for IR transmit.
     * 
     * @return true if new pwmHandle successfully created and started
     */
    bool timerConfigForSend();

    /**
     * Send repeat command
     * Repeat commands should be sent in a 110 ms raster.
     */
    void sendNECRepeat();

    /**
     * Send IR command in NEC encoding. Repeat commands should be sent 
     * in a 110 ms raster. There is NO delay after the last sent repeat! 
     *  https://www.sbprojects.net/knowledge/ir/nec.php
     * 
     * @param address           IR command address value
     * @param command           IR command command value
     * @param numberOfRepeats   number of IR command repeats to transmit (default is none)
     * @param isRepeat  if true, send only one repeat frame without leading and trailing space
     */
    void sendNEC(uint16_t address, uint8_t command, uint8_t numberOfRepeats = NO_REPEATS, 
                                                        bool isRepeat = false);
    /**
     * Send IR command in NEC encoding. Repeat commands should be sent 
     * in a 110 ms raster. There is NO delay after the last sent repeat! 
     * 
     * @param rawData           NEC IR 4-byte command with address and command value
     * @param numberOfRepeats   number of IR command repeats to transmit (default is none)
     * @param isRepeat  if true, send only one repeat frame without leading and trailing space
     */
    void sendNECRaw(uint32_t rawData, uint8_t numberOfRepeats = NO_REPEATS, bool isRepeat = false);

    uint8_t txPin;
    uint8_t pwmIndex;
    PWM_Handle pwmHandle;
    PinConfig *config;
    uint8_t sel0Mask;
    uint8_t sel1Mask;
    // volatile uint8_t *outReg;
    volatile uint8_t *dirReg;
    volatile uint8_t *sel0Reg;
    volatile uint8_t *sel1Reg;
    volatile uint8_t *outReg;
    volatile uint8_t *renReg;
//#define HWREG8(x)         (*((volatile uint8_t *)(x)))

    /**
     * Custom delay function that circumvents Arduino's delayMicroseconds 16 bit limit
     * and is (mostly) not extended by the duration of interrupt codes like the millis() interrupt
     * 
     * @param aMicroseconds microseconds to delay
     */
    static void customDelayMicroseconds(unsigned long aMicroseconds);
};

/** @}*/

#endif // TINY_NEC_TX_H
