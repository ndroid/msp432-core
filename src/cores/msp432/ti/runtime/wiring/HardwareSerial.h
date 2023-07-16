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

#ifndef HardwareSerial_h
#define HardwareSerial_h

#include <inttypes.h>
#include "Stream.h"

#include <ti/drivers/UART.h>

#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/gates/GateMutex.h>

#define SERIAL_RX_BUFFER_SIZE  128
#define SERIAL_TX_BUFFER_SIZE  128

/*
 * Set to identical values as defined for *config* parameter in Arduino
 *  to maintain compatibility with all Arduino Serial applications
 */
#define SERIAL_PARITY_EVEN   (0x1ul)
#define SERIAL_PARITY_ODD    (0x2ul)
#define SERIAL_PARITY_NONE   (0x3ul)
#define SERIAL_PARITY_MARK   (0x4ul)
#define SERIAL_PARITY_SPACE  (0x5ul)
#define SERIAL_PARITY_MASK   (0xFul)

#define SERIAL_STOP_BIT_1    (0x10ul)
#define SERIAL_STOP_BIT_1_5  (0x20ul)
#define SERIAL_STOP_BIT_2    (0x30ul)
#define SERIAL_STOP_BIT_MASK (0xF0ul)

#define SERIAL_DATA_5        (0x100ul)
#define SERIAL_DATA_6        (0x200ul)
#define SERIAL_DATA_7        (0x300ul)
#define SERIAL_DATA_8        (0x400ul)
#define SERIAL_DATA_MASK     (0xF00ul)

#define SERIAL_5N1           (SERIAL_STOP_BIT_1 | SERIAL_PARITY_NONE  | SERIAL_DATA_5)
#define SERIAL_6N1           (SERIAL_STOP_BIT_1 | SERIAL_PARITY_NONE  | SERIAL_DATA_6)
#define SERIAL_7N1           (SERIAL_STOP_BIT_1 | SERIAL_PARITY_NONE  | SERIAL_DATA_7)
#define SERIAL_8N1           (SERIAL_STOP_BIT_1 | SERIAL_PARITY_NONE  | SERIAL_DATA_8)
#define SERIAL_5N2           (SERIAL_STOP_BIT_2 | SERIAL_PARITY_NONE  | SERIAL_DATA_5)
#define SERIAL_6N2           (SERIAL_STOP_BIT_2 | SERIAL_PARITY_NONE  | SERIAL_DATA_6)
#define SERIAL_7N2           (SERIAL_STOP_BIT_2 | SERIAL_PARITY_NONE  | SERIAL_DATA_7)
#define SERIAL_8N2           (SERIAL_STOP_BIT_2 | SERIAL_PARITY_NONE  | SERIAL_DATA_8)
#define SERIAL_5E1           (SERIAL_STOP_BIT_1 | SERIAL_PARITY_EVEN  | SERIAL_DATA_5)
#define SERIAL_6E1           (SERIAL_STOP_BIT_1 | SERIAL_PARITY_EVEN  | SERIAL_DATA_6)
#define SERIAL_7E1           (SERIAL_STOP_BIT_1 | SERIAL_PARITY_EVEN  | SERIAL_DATA_7)
#define SERIAL_8E1           (SERIAL_STOP_BIT_1 | SERIAL_PARITY_EVEN  | SERIAL_DATA_8)
#define SERIAL_5E2           (SERIAL_STOP_BIT_2 | SERIAL_PARITY_EVEN  | SERIAL_DATA_5)
#define SERIAL_6E2           (SERIAL_STOP_BIT_2 | SERIAL_PARITY_EVEN  | SERIAL_DATA_6)
#define SERIAL_7E2           (SERIAL_STOP_BIT_2 | SERIAL_PARITY_EVEN  | SERIAL_DATA_7)
#define SERIAL_8E2           (SERIAL_STOP_BIT_2 | SERIAL_PARITY_EVEN  | SERIAL_DATA_8)
#define SERIAL_5O1           (SERIAL_STOP_BIT_1 | SERIAL_PARITY_ODD   | SERIAL_DATA_5)
#define SERIAL_6O1           (SERIAL_STOP_BIT_1 | SERIAL_PARITY_ODD   | SERIAL_DATA_6)
#define SERIAL_7O1           (SERIAL_STOP_BIT_1 | SERIAL_PARITY_ODD   | SERIAL_DATA_7)
#define SERIAL_8O1           (SERIAL_STOP_BIT_1 | SERIAL_PARITY_ODD   | SERIAL_DATA_8)
#define SERIAL_5O2           (SERIAL_STOP_BIT_2 | SERIAL_PARITY_ODD   | SERIAL_DATA_5)
#define SERIAL_6O2           (SERIAL_STOP_BIT_2 | SERIAL_PARITY_ODD   | SERIAL_DATA_6)
#define SERIAL_7O2           (SERIAL_STOP_BIT_2 | SERIAL_PARITY_ODD   | SERIAL_DATA_7)
#define SERIAL_8O2           (SERIAL_STOP_BIT_2 | SERIAL_PARITY_ODD   | SERIAL_DATA_8)
#define SERIAL_5M1           (SERIAL_STOP_BIT_1 | SERIAL_PARITY_MARK  | SERIAL_DATA_5)
#define SERIAL_6M1           (SERIAL_STOP_BIT_1 | SERIAL_PARITY_MARK  | SERIAL_DATA_6)
#define SERIAL_7M1           (SERIAL_STOP_BIT_1 | SERIAL_PARITY_MARK  | SERIAL_DATA_7)
#define SERIAL_8M1           (SERIAL_STOP_BIT_1 | SERIAL_PARITY_MARK  | SERIAL_DATA_8)
#define SERIAL_5M2           (SERIAL_STOP_BIT_2 | SERIAL_PARITY_MARK  | SERIAL_DATA_5)
#define SERIAL_6M2           (SERIAL_STOP_BIT_2 | SERIAL_PARITY_MARK  | SERIAL_DATA_6)
#define SERIAL_7M2           (SERIAL_STOP_BIT_2 | SERIAL_PARITY_MARK  | SERIAL_DATA_7)
#define SERIAL_8M2           (SERIAL_STOP_BIT_2 | SERIAL_PARITY_MARK  | SERIAL_DATA_8)
#define SERIAL_5S1           (SERIAL_STOP_BIT_1 | SERIAL_PARITY_SPACE | SERIAL_DATA_5)
#define SERIAL_6S1           (SERIAL_STOP_BIT_1 | SERIAL_PARITY_SPACE | SERIAL_DATA_6)
#define SERIAL_7S1           (SERIAL_STOP_BIT_1 | SERIAL_PARITY_SPACE | SERIAL_DATA_7)
#define SERIAL_8S1           (SERIAL_STOP_BIT_1 | SERIAL_PARITY_SPACE | SERIAL_DATA_8)
#define SERIAL_5S2           (SERIAL_STOP_BIT_2 | SERIAL_PARITY_SPACE | SERIAL_DATA_5)
#define SERIAL_6S2           (SERIAL_STOP_BIT_2 | SERIAL_PARITY_SPACE | SERIAL_DATA_6)
#define SERIAL_7S2           (SERIAL_STOP_BIT_2 | SERIAL_PARITY_SPACE | SERIAL_DATA_7)
#define SERIAL_8S2           (SERIAL_STOP_BIT_2 | SERIAL_PARITY_SPACE | SERIAL_DATA_8)

class HardwareSerial : public Stream
{

    private:
        bool begun;
        bool blockingModeEnabled;
        bool continuousReadMode;
        bool useContinuousRead;
        unsigned char rxBuffer[SERIAL_RX_BUFFER_SIZE];
        volatile unsigned long rxWriteIndex;
        volatile unsigned long rxReadIndex;
        unsigned char txBuffer[SERIAL_TX_BUFFER_SIZE];
        volatile unsigned long txWriteIndex;
        volatile unsigned long txReadIndex;
        volatile bool txActive;
        unsigned long baudRate;
        uint8_t uartModule;
        UART_Handle uart;
        UART_Callback rxCallback;
        UART_Callback txCallback;
        GateMutex_Struct gate;
        void init(unsigned long module, UART_Callback rxCallback, UART_Callback txCallback);
        void flushAll(void);
        void primeTx(void);

    public:
        operator bool();// Arduino compatibility (see StringLength example)
        HardwareSerial(void);
        HardwareSerial(unsigned long);
        HardwareSerial(unsigned long, UART_Callback, UART_Callback);
        HardwareSerial(unsigned long, UART_Callback, UART_Callback, bool);
        void begin(unsigned long);
        void begin(unsigned long, uint16_t);
        void begin(unsigned long, uint16_t, bool);
        void setModule(unsigned long);
        void setPins(unsigned long);
        void acquire(void);  /* acquire serial port for this thread */
        void release(void);  /* release serial port */
        void end(void);
        virtual int available(void);
        virtual int availableForWrite(void);
        virtual int peek(void);
        virtual int read(void);
        virtual void flush(void);
        void readCallback(UART_Handle uart, void *buf, size_t count);
        void writeCallback(UART_Handle uart, void *buf, size_t count);
        virtual size_t write(uint8_t c);
        virtual size_t write(const uint8_t *buffer, size_t size);
        using Print::write; // pull in write(str) from Print
};

extern HardwareSerial Serial;
extern HardwareSerial Serial1;
extern HardwareSerial Serial2;
extern HardwareSerial Serial3;
extern HardwareSerial Serial4;

extern void serialEventRun(void) __attribute__((weak));
extern void serialEventRun1(void) __attribute__((weak));
extern void serialEventRun2(void) __attribute__((weak));
extern void serialEventRun3(void) __attribute__((weak));
extern void serialEventRun4(void) __attribute__((weak));

extern void serialEvent() __attribute__((weak));
extern void serialEvent1() __attribute__((weak));
extern void serialEvent2() __attribute__((weak));
extern void serialEvent3() __attribute__((weak));
extern void serialEvent4() __attribute__((weak));

#endif
