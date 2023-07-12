/*
 * Copyright (c) 2015-2018, Texas Instruments Incorporated
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

#include <ti/runtime/wiring/Energia.h>
#include <ti/runtime/wiring/wiring_private.h>

#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>
#define ti_sysbios_knl_Clock__internalaccess
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/sysbios/family/arm/m3/Hwi.h>

#define ti_sysbios_family_arm_msp432_Timer__internalaccess
#include <ti/sysbios/family/arm/msp432/Timer.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerMSP432.h>

#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPIMSP432DMA.h>

#include <driverlib/rom.h>
#include <driverlib/rom_map.h>
#include <driverlib/wdt_a.h>

static uint32_t timestampFreq = 0;
static uint32_t clockTickTimestamp = 0;

static uint8_t delayMode = 0; /* determines  which tick source is driving Clock_tick */

/*
 *  ======== micros ========
 */
unsigned long micros(void)
{
    uint32_t key;
    Types_FreqHz freq;
    uint64_t micros, expired;

    if (timestampFreq == 0) {
        Timestamp_getFreq(&freq);
        timestampFreq = freq.lo;
    }

    key = Hwi_disable();

    micros = Clock_getTicks() * Clock_tickPeriod;
    expired = Timestamp_get32() - clockTickTimestamp;

    Hwi_restore(key);

    micros += (expired * 1000000) / timestampFreq;

    return (micros);
}

/*
 *  ======== millis ========
 */
unsigned long millis(void)
{
    return (Clock_getTicks());
}

/*
 *  ======== delayMicroseconds ========
 *  Delay for the given number of microseconds.
 */
void delayMicroseconds(unsigned int us)
{
    if (us <7) {
        //The overhead in calling and returning from this function takes about 6us
    }
    else if (us <=20) {
        int time;
        for (time = 5*(us-6); time > 0; time--) {
            asm("   nop");
        }
    }
    else if (us < 70) {
        int time;
        for (time = 5*us; time > 0; time--) {
            asm("   nop");
        }
    }
    else {
        uint32_t t0, deltaT;
        Types_FreqHz freq;

        Timestamp_getFreq(&freq);
        deltaT = us * (freq.lo/1000000);

        t0 = Timestamp_get32();

        while ((Timestamp_get32()-t0) < deltaT) {
            ;
        }
    }
}

extern void (*delayFxn)(uint32_t milliseconds);

/*
 *  ======== delay ========
 */
void delay(uint32_t milliseconds)
{
    delayFxn(milliseconds);
}

/* Timer created statically */
extern Timer_Handle energiaClockTimer; /* initialized in energia.cfg file */

/*
 *  ======== energiaClockTickFxnP401R ========
 */
void energiaClockTickFxnP401R(uintptr_t arg)
{
    /*
     * Bump Clock tick count by 249.
     * Clock_tick() will bump it one more
     */
    if (delayMode == 1) {
        ((ti_sysbios_knl_Clock_Module_State *)(&ti_sysbios_knl_Clock_Module__state__V))->ticks += 249;
    }

    /* snapshot current timestamp for micros() */
    clockTickTimestamp = Timestamp_get32();

    Clock_tick();
}

/*
 *  ======== switchToWatchdogTimer ========
 *
 *  Use 250ms watchdog timer interrupt to drive the Clock tick
 *  Stop the default Timer_A then start the watchdog timer.
 */
static void switchToWatchdogTimer()
{
    static Hwi_Handle wdtHwi = NULL;

    /* Stop Timer_A currrently being used by Clock */
    Timer_stop(energiaClockTimer);

    MAP_WDT_A_holdTimer();

    if (wdtHwi == NULL) {
        /* Create watchdog Timer Hwi */
        wdtHwi = Hwi_create(19, energiaClockTickFxnP401R, NULL, NULL);

        /* set WDT to use 32KHz input, 250ms period */
        MAP_WDT_A_initIntervalTimer(WDT_A_CLOCKSOURCE_BCLK, WDT_A_CLOCKITERATIONS_8192);
    }

    /* remove DEEPSLEEP0 constraint left from TimerA usage */
    Power_releaseConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_0);

    /* don't allow deeper than DEEPSLEEP1 */
    Power_setConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_1);

    /* Start watchdog Timer */
    MAP_WDT_A_clearTimer();
    MAP_WDT_A_startTimer();

    /* hence, Clock_tick() will be called from 250ms watchdog timer interrupt */
}

/*
 *  ======== switchToTimerA ========
 *
 *  Use 1ms Timer_A interrupt to drive the Clock tick
 *  By default, the Timer_A Hwi object has already been
 *  statically created and configured to call Clock_tick().
 *  Simply stop the watchdog timer and restart the Timer_A.
 */
static void switchToTimerA()
{
    /* Stop watchdog Timer */
    MAP_WDT_A_holdTimer();

    /* remove DEEPSLEEP1 constraint left from watchdog usage */
    Power_releaseConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_1);

    /* don't all the power to be cut in deep sleep */
    Power_setConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_0);

    /* Re-start Timer_A */
    Timer_start(energiaClockTimer);

    /* hence, Clock_tick() will be called from 1ms Timer_A interrupt */
}

/*
 *  ======== delay401R ========
 */
void delay401R(uint32_t milliseconds)
{
    if (milliseconds == 0) {
        Task_yield();
        return;
    }

    switch (delayMode) {
        /* using Timer_A, check for opportunity to transition to WDT */
        case 0:
            if ( (milliseconds >= 250) && (milliseconds % 250) == 0) {
                delayMode = 1;
                switchToWatchdogTimer();
            }
            else {
                delayMode = 2;
                switchToTimerA();
            }
            break;
        /* using WDT, check for need to transition to Timer_A */
        case 1:
            if ( (milliseconds >= 250) && (milliseconds % 250) == 0) {
                /* stay in mode 1 */
            }
            else {
                /* switch to Timer_A and never look back */
                delayMode = 2;
                switchToTimerA();
            }
            break;
        /* always using Timer_A */
        case 2:
            break;
    }

    /* timeout is always in milliseconds so that Clock_workFunc() behaves properly */
    Task_sleep(milliseconds);
}

/*
 *  ======== energiaClockTickFxnP4111 ========
 */
void energiaClockTickFxnP4111(uintptr_t arg)
{
    static uint16_t error = 0;

    /*
     * Timer is clocked at 32,768Hz (ACLK).
     *
     * The following algorithm, derived from:
     *   https://e2e.ti.com/support/microcontrollers/msp430/f/166/t/107222?1mSec-tick-for-OS-from-XT1-with-32768-Hz-crystal
     * will ensure that 125 interrupts equals 125ms, to the accuracy of the
     * timebase.
     */

    error += 96;

    if (error >= 125) {
        error -= 125;
        energiaClockTimer->period = 33;
    }
    else {
        energiaClockTimer->period = 32;
    }

    /* snapshot current timestamp for micros() */
    clockTickTimestamp = Timestamp_get32();

    Clock_tick();
}

/*
 *  ======== delay4111 ========
 */
void delay4111(uint32_t milliseconds)
{
    if (milliseconds == 0) {
        Task_yield();
        return;
    }

    /* timeout is always in milliseconds so that Clock_workFunc() behaves properly */
    Task_sleep(milliseconds);
}

/*
 *  ======== setDelayResolution ========
 *
 *  For now, only two resolutions are supported: 1ms and 250ms
 */
void setDelayResolution(uint32_t milliseconds)
{
    if (milliseconds == 250) {
        if ((delayMode == 0) || (delayMode == 2)) {
            switchToWatchdogTimer();
            delayMode = 1;
        }
    }
    else {
        if (delayMode == 1) {
            switchToTimerA();
            delayMode = 2;
        }
    }
}

/*
 *  ======== getSpiInfo ========
 *
 *  A hack to work around spiPolling only being supported
 *  in SPI_MODE_BLOCKING. Remove if/when this is resolved
 *  in the SPI drivers.
 */
void getSpiInfo(void *spi, SpiInfo *spiInfo)
{
    SPIMSP432DMA_Object *obj;
    SPIMSP432DMA_HWAttrsV1 const *hwAttrs;
    SPI_Handle spiHandle = (SPI_Handle)spi;

    obj = (SPIMSP432DMA_Object *)(spiHandle->object);
    hwAttrs = (SPIMSP432DMA_HWAttrsV1 *)(spiHandle->hwAttrs);

    spiInfo->transferModePtr = &obj->transferMode;
    spiInfo->minDmaTransferSize = hwAttrs->minDmaTransferSize;
}
