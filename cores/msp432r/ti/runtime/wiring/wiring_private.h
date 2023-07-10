/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
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

#ifndef WiringPrivate_h
#define WiringPrivate_h

#include "Energia.h"
#ifdef __cplusplus
extern "C" {
#endif

#define PIN_FUNC_UNUSED             0
#define PIN_FUNC_DIGITAL_OUTPUT     1
#define PIN_FUNC_DIGITAL_INPUT      2
#define PIN_FUNC_ANALOG_OUTPUT      3
#define PIN_FUNC_ANALOG_INPUT       4
#define PIN_FUNC_INVALID            5

#define NOT_ON_ADC      0xff

typedef void (*StopFunc)(uint8_t);

extern void stopAnalogWrite(uint8_t pin);
extern void stopAnalogRead(uint8_t pin);
extern void stopDigitalWrite(uint8_t pin);
extern void stopDigitalRead(uint8_t pin);

extern void stopAnalogWriteFxn(uint8_t);
extern void stopAnalogReadFxn(uint8_t);

extern StopFunc stopAnalogWriteFxnPtr;
extern StopFunc stopAnalogReadFxnPtr;

typedef struct SpiInfo {
    uint16_t minDmaTransferSize;
    void *transferModePtr;
} SpiInfo;

extern void getSpiInfo(void *spi, SpiInfo *spiInfo);

extern int8_t analogReadShift;

extern uint8_t digital_pin_to_pin_function[];
extern const uint8_t digital_pin_to_adc_index[];


#ifdef __cplusplus
} // extern "C"
#endif


#endif
