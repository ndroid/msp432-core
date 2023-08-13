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

#ifndef _SPI_H_INCLUDED
#define _SPI_H_INCLUDED

#include <inttypes.h>

#include <ti/drivers/SPI.h>
#include <ti/sysbios/gates/GateMutex.h>

#define SPI_MODE0 SPI_POL0_PHA0
#define SPI_MODE1 SPI_POL0_PHA1
#define SPI_MODE2 SPI_POL1_PHA0
#define SPI_MODE3 SPI_POL1_PHA1

#ifndef MSBFIRST
#define MSBFIRST 1
#endif
#ifndef LSBFIRST
#define LSBFIRST 0
#endif

#define SPI_CLOCK_MAX 16000000L
#define SPI_CLOCK_DIV1 1
#define SPI_CLOCK_DIV2 2
#define SPI_CLOCK_DIV4 4
#define SPI_CLOCK_DIV8 8
#define SPI_CLOCK_DIV16 16
#define SPI_CLOCK_DIV32 32

#define MAX_USING_INTERRUPTS 16

class SPISettings {
    public:
        SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) {
            init_MightInline(clock, bitOrder, dataMode);
        }
        SPISettings() {
            init_AlwaysInline(4000000, MSBFIRST, SPI_MODE0);
        }
    private:
        void init_MightInline(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) {
            init_AlwaysInline(clock, bitOrder, dataMode);
        }
        void init_AlwaysInline(uint32_t clock, uint8_t bitOrder, uint8_t dataMode)
            __attribute__((__always_inline__)) {
            // Clock settings are defined as follows. Note that this shows SPI2X
            // inverted, so the bits form increasing numbers. Also note that
            // fosc/64 appears twice
            // SPR1 SPR0 ~SPI2X Freq
            //   0    0     0   fosc/2
            //   0    0     1   fosc/4
            //   0    1     0   fosc/8
            //   0    1     1   fosc/16
            //   1    0     0   fosc/32
            //   1    0     1   fosc/64
            //   1    1     0   fosc/64
            //   1    1     1   fosc/128

            // We find the fastest clock that is less than or equal to the
            // given clock rate. The clock divider that results in clock_setting
            // is 2 ^^ (clock_div + 1). If nothing is slow enough, we'll use the
            // slowest (128 == 2 ^^ 7, so clock_div = 6).
            uint8_t clockDiv;

                uint32_t clockSetting = F_CPU / 2;
                clockDiv = 0;
                while (clockDiv < 6 && clock < clockSetting)
                {
                    clockSetting /= 2;
                    clockDiv++;
                }
        }
        uint8_t bitOrder;
        uint8_t dataMode;
        uint8_t clockDivider;
        friend class SPIClass;
};

class SPIClass
{
    private:
        bool begun;
        uint8_t slaveSelect;
        uint8_t spiModule;
        uint8_t bitOrder;
        uint8_t dataMode;
        uint8_t clockDivider;

        uint8_t usingInterruptPins[MAX_USING_INTERRUPTS];
        uint8_t numUsingInterrupts;

        SPI_Handle spi;
        SPI_Params params;
        SPI_Transaction transaction;

        SPI_TransferMode *spiTransferModePtr;
        uint32_t minDmaTransferSize;

        GateMutex_Struct gate;
        void init(unsigned long);
        uint8_t reverseBits(uint8_t);

    public:
        volatile bool transferComplete;

        SPIClass(void);
        SPIClass(unsigned long);
        void begin(); // Default
        void begin(uint8_t);
        void end();
        void end(uint8_t);

        void setBitOrder(uint8_t);
        void setBitOrder(uint8_t, uint8_t);

        void setDataMode(uint8_t);
        void setClockDivider(uint8_t);

        uint8_t transfer(uint8_t);
        uint8_t transfer(uint8_t, uint8_t);
        uint8_t transfer(uint8_t, uint8_t, uint8_t);
        uint8_t *transfer(uint8_t *, size_t);

        void setModule(uint8_t);
        void usingInterrupt(uint8_t);
};

extern SPIClass SPI;
extern SPIClass SPI1;

#endif
