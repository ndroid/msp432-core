/*
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "wiring_private.h"

#include <ti/drivers/GPIO.h>

/* Measures the length (in microseconds) of a pulse on the pin; state is HIGH
 * or LOW, the type of pulse to measure.  Works on pulses from 2-3 microseconds
 * to 3 minutes in length, but must be called at least a few dozen microseconds
 * before the start of the pulse. */

unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout)
{
    uint8_t stateMask;
    uint32_t start, end, result;
    uint32_t numloops;
    uint32_t microsTime;
    static bool firstTime = true;
    static uint32_t loopTime;

    /* measure GPIO_read() time */
    if (firstTime == true) {
        microsTime = micros();
        microsTime = micros();
        microsTime = micros() - microsTime;
        loopTime = micros();
        GPIO_read(pin);
        loopTime = micros() - loopTime - microsTime;
        if (loopTime == 0) loopTime = 1;
        firstTime = false;
    }

    stateMask = state ? 1 : 0;

    /* loopTime is mostly GPIO_read() */
    numloops = timeout / loopTime;

    // wait for any previous pulse to end
    while (GPIO_read(pin) == stateMask) {
        if (numloops-- == 0) {
            return (0);
        }
    }

    // wait for the pulse to start
    while (GPIO_read(pin) != stateMask) {
        if (numloops-- == 0) {
            return (0);
        }
    }

    // wait for the pulse to stop
    start = micros();

    while (GPIO_read(pin) == stateMask) {
        if (numloops-- == 0) {
            return (0);
        }
    }

    end = micros();
    result = end - start;

    return (result);
}
