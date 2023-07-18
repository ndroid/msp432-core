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

#define ARDUINO_MAIN

#include <ti/runtime/wiring/wiring_private.h>
#include "wiring_analog.h"

#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOMSP432.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerMSP432.h>

#include <ti/drivers/PWM.h>
#include <ti/drivers/pwm/PWMTimerMSP432.h>

#include <ti/drivers/ADC.h>
#include <ti/drivers/adc/ADCMSP432.h>

#include <ti/sysbios/family/arm/m3/Hwi.h>
#include <ti/sysbios/family/arm/msp432/Timer.h>

#include <driverlib/rom.h>
#include <driverlib/rom_map.h>
#include <driverlib/timer_a.h>
#include <driverlib/adc14.h>
#include <driverlib/ref_a.h>
#include <driverlib/pmap.h>
#include <driverlib/gpio.h>

#define PWM_NOT_IN_USE  0xffff
#define PWM_IN_USE      0xfffe

#define TIMER2_INDEX    0x02

/*
 * analogWrite() support
 */

extern PWM_Config PWM_config[];
extern PWMTimerMSP432_HWAttrsV2 pwmTimerMSP432HWAttrs[];
extern const GPIOMSP432_Config GPIOMSP432_config;
extern ADCMSP432_HWAttrsV1 adcMSP432HWAttrs[];
extern ADC_Config ADC_config[];

void stopAnalogWriteFxn(uint8_t);
void stopAnalogReadFxn(uint8_t);

StopFunc stopAnalogWriteFxnPtr = NULL;
StopFunc stopAnalogReadFxnPtr = NULL;

uint32_t fixedTimerId = (1 << TIMER2_INDEX);

/* port number to PXMAP translation */
const uint8_t pxmap[] = {
    0,               /* Port numbers start at 1 */
    PMAP_P1MAP,
    PMAP_P2MAP,
    PMAP_P3MAP,
    PMAP_P4MAP,
    PMAP_P5MAP,
    PMAP_P6MAP,
    PMAP_P7MAP,
};

/* Current PWM timer GPIO mappings */
uint16_t used_pwm_port_pins[] = {
    PWM_NOT_IN_USE,   /* Timer 0, CCR 1 */
    PWM_NOT_IN_USE,   /* Timer 0, CCR 2 */
    PWM_NOT_IN_USE,   /* Timer 0, CCR 3 */
    PWM_NOT_IN_USE,   /* Timer 0, CCR 4 */
    PWM_NOT_IN_USE,   /* Timer 1, CCR 1 */
    PWM_NOT_IN_USE,   /* Timer 1, CCR 2 */
    PWM_NOT_IN_USE,   /* Timer 1, CCR 3 */
    PWM_NOT_IN_USE,   /* Timer 1, CCR 4 */
    PWM_NOT_IN_USE,   /* Timer 2, CCR 1 Fixed mapping */
    PWM_NOT_IN_USE,   /* Timer 2, CCR 2 Fixed mapping */
    PWM_NOT_IN_USE,   /* Timer 2, CCR 3 Fixed mapping */
    PWM_NOT_IN_USE,   /* Timer 2, CCR 4 Fixed mapping */
};

uint32_t fixed_map_pwm_pins[] = {
    PWMTimerMSP432_P5_6_TA2CCR1A,
    PWMTimerMSP432_P5_7_TA2CCR2A,
    PWMTimerMSP432_P6_6_TA2CCR3A,
    PWMTimerMSP432_P6_7_TA2CCR4A
};

/* Mappable PWM Timer capture pin encodings */
uint32_t mapped_pwm_pin_ccrs[] = {
    PWMTimerMSP432_TA0CCR1,
    PWMTimerMSP432_TA0CCR2,
    PWMTimerMSP432_TA0CCR3,
    PWMTimerMSP432_TA0CCR4,
    PWMTimerMSP432_TA1CCR1,
    PWMTimerMSP432_TA1CCR2,
    PWMTimerMSP432_TA1CCR3,
    PWMTimerMSP432_TA1CCR4
};

/*
 * Keep track of the number of Capture Compare Registers are used
 * for each of 4 timers. There are 4 CCs available for each timer.
 * When the first CCR is used by PWM, inform Timer module that that
 * timer is in use. When the last CCR of a timer is released, inform
 * the Timer module that the timer is available.
 *
 *  0 -> 1  Timer_setAvailMask(1)
 *  1 -> 0  Timer_setAvailMask(0)
 */
uint8_t timer_ccrs_in_use[] = {
    0, 0, 0, 0
};

/*
 * Do whatever is necessary to prepare the digital input pin to be
 * configured in any other mode
 */
void stopDigitalRead(uint8_t pin)
{
}

/*
 * Do whatever is necessary to prepare the digital output pin to be
 * configured in any other mode
 */
 void stopDigitalWrite(uint8_t pin)
{
}

/*
 * For the MSP432, the timers used for PWM are clocked at 12MHz.
 * The period is set to 2.04ms in the PWM_open() call.
 * The PWM objects are configured for PWM_DUTY_COUNTS mode to minimize
 * the PWM_setDuty() processing overhead.
 * The 2.04ms period yields a period count of 24480.
 * The Arduino analogWrite() API takes a value of 0-255 for the duty cycle.
 * The PWM scale factor is then 24480 / 255 = 96
 */

#define PWM_PERIOD_IN_COUNTS 24480
//#define PWM_SCALE_FACTOR 24480/255

uint16_t pwmMaxDuty = 255;      /* default to 8 bit resolution  */

/*
 * \brief sets the number of bits for duty cycle range of analogWrite()
 */
void analogWriteResolution(uint16_t bits)
{
    if ((bits > 0) && (bits <= 16)) {
        pwmMaxDuty = bit(bits) - 1;
    }
}

void analogWrite(uint8_t pin, int val)
{
    uint16_t pinId, pinNum;
    uint8_t pwmIndex, timerId;
    uint_fast8_t port;
    uint_fast16_t pinMask;
    uint32_t hwiKey;
    uint32_t pwmPin;
    uint8_t timerAvailMask;

    hwiKey = Hwi_disable();

    if (digital_pin_to_pin_function[pin] == PIN_FUNC_ANALOG_OUTPUT) {
        pwmIndex = digital_pin_to_pwm_index[pin];
    }
    else {
        timerAvailMask = Timer_getAvailMask();

        /* re-configure pin if possible */
        PWM_Params pwmParams;
        PWM_Handle pwmHandle;

        /* idempotent */
        PWM_init();

        /*
         * The pwmIndex fetched from the pin_to_pwm_index[] table
         * is either an actual index into the PWM instance table
         * if the pin has already been mapped to a PWM resource,
         * or a mappable port/pin ID, or NOT_MAPPABLE.
         */
        pwmIndex = digital_pin_to_pwm_index[pin];

        if (pwmIndex == PWM_NOT_MAPPABLE) {
            Hwi_restore(hwiKey);
            return; /* can't get there from here */
        }

        pinId = GPIOMSP432_config.pinConfigs[pin] & 0xffff;
        port = pinId >> 8;
        pinMask = pinId & 0xff;
        pinNum = 0;
        while (((1 << pinNum) & pinMask) == 0) pinNum++;

        if (pwmIndex < PWM_AVAILABLE_PWMS) { /* fixed mapping */
            if(!(Timer_getAvailMask() & fixedTimerId) && (!timer_ccrs_in_use[TIMER2_INDEX])) {
                Hwi_restore(hwiKey);
                return; // fixed Timer is already taken by other function
            }
            if (used_pwm_port_pins[pwmIndex] != PWM_NOT_IN_USE) {
                Hwi_restore(hwiKey);
                return; /* PWM port already in use */
            }
            /*
             * encode port/pin to inform stopAnalogWrite() that
             * this is a fixed mapped pin
             */

            used_pwm_port_pins[pwmIndex] = pwmIndex;

            /* plug pwmPin in HwAttrs with corresponding encoded pin identifier */
            pwmTimerMSP432HWAttrs[pwmIndex].pwmPin = fixed_map_pwm_pins[pwmIndex-8];
        }
        else {
            /* find an unused PWM resource and port map it */
            for (pwmIndex = 0; pwmIndex <= PWM_MAX_MAPPABLE_INDEX; pwmIndex++) {
                timerId = pwmIndex >> 2;
                /* Check if the timer is already taken or if already claimed by us */
                if(!(timerAvailMask & (1 << timerId)) && (!timer_ccrs_in_use[timerId])) {
                    pwmIndex += 3;
                    continue;
                }
                if (used_pwm_port_pins[pwmIndex] == PWM_NOT_IN_USE) {
                    /* remember which pinId is being used by this PWM resource */
                    used_pwm_port_pins[pwmIndex] = pinId; /* save port/pin info */
                    /* remember which PWM resource is being used by this pin */
                    digital_pin_to_pwm_index[pin] = pwmIndex; /* save pwm index */
                    break;
                }
            }

            if (pwmIndex > PWM_MAX_MAPPABLE_INDEX) {
                Hwi_restore(hwiKey);
                return; /* no unused PWM ports */
            }

            /* encode pwmPin field with port/pin/TAxCCRyA info */
            pwmPin = port << 4 | pinNum;
            pwmPin = pwmPin | mapped_pwm_pin_ccrs[pwmIndex];
            pwmTimerMSP432HWAttrs[pwmIndex].pwmPin = pwmPin;
        }

        PWM_Params_init(&pwmParams);

        /* Open the PWM port */
        pwmParams.periodUnits = PWM_PERIOD_COUNTS;
        pwmParams.periodValue = PWM_PERIOD_IN_COUNTS; /* arduino period is 2.04ms (490Hz) */
        pwmParams.dutyUnits = PWM_DUTY_COUNTS;

        /* PWM_open() will fail if the timer's CCR is already in use */
        pwmHandle = PWM_open(pwmIndex, &pwmParams);

        /*
         * Assume that the timer was already in use by someone else.
         * mark all associated CCRs busy so we don't keep re-trying this
         * timer's pwmIndexes.
         */
        if (pwmHandle == NULL) {
            pwmIndex &= 0xfc; /* start at base of this timer's PWM indexes */
            used_pwm_port_pins[pwmIndex++] = PWM_IN_USE;
            used_pwm_port_pins[pwmIndex++] = PWM_IN_USE;
            used_pwm_port_pins[pwmIndex++] = PWM_IN_USE;
            used_pwm_port_pins[pwmIndex] = PWM_IN_USE;
            Hwi_restore(hwiKey);
            return;
        }

        /* start the timer */
        PWM_start(pwmHandle);

        /* remove timer from pool of available timers if not in use */
        timerId = pwmIndex >> 2;
        timer_ccrs_in_use[timerId] += 1;
        if (timer_ccrs_in_use[timerId] == 1) {
            Timer_setAvailMask(Timer_getAvailMask() & ~(1 << timerId));
        }

        /*
         * To reduce footprint when analogWrite isn't used,
         * reference stopAnalogWriteFxn only if analogWrite
         * has been called.
         */
        stopAnalogWriteFxnPtr = stopAnalogWriteFxn;

        digital_pin_to_pin_function[pin] = PIN_FUNC_ANALOG_OUTPUT;
    }

    Hwi_restore(hwiKey);

    PWM_setDuty((PWM_Handle)&(PWM_config[pwmIndex]), 
                        ((uint32_t) val * PWM_PERIOD_IN_COUNTS / pwmMaxDuty));
}

/*
 * This internal API is used to de-configure a pin that has been
 * put in analogWrite() mode.
 *
 * It will free up the pin's PWM resource after
 * it is no longer being used to support analogWrite() on a different
 * pin. It is called by pinMap() when a pin's function is being modified.
 */
void stopAnalogWrite(uint8_t pin)
{
    stopAnalogWriteFxnPtr(pin);
}

void stopAnalogWriteFxn(uint8_t pin)
{
    uint16_t pwmIndex = digital_pin_to_pwm_index[pin];
    uint8_t timerId;
    uint_fast8_t port;
    uint_fast16_t pinMask;
    uint16_t pinNum;
    uint32_t hwiKey;

    /* Close PWM port */
    PWM_close((PWM_Handle)&(PWM_config[pwmIndex]));

    hwiKey = Hwi_disable();

    /* if PWM is assigned to a dynamically mapped pin */
    if (used_pwm_port_pins[pwmIndex] >= PWM_AVAILABLE_PWMS) {
        /* undo dynamic port mapping plumbing */
        port = used_pwm_port_pins[pwmIndex] >> 8;
        pinMask = used_pwm_port_pins[pwmIndex] & 0xff;

        /* derive pinNum from pinMask */
        pinNum = 0;
        while (((1 << pinNum) & pinMask) == 0) pinNum++;

        /* the following code was extracted from PMAP_configurePort() */

        //Get write-access to port mapping registers:
        PMAP->KEYID = PMAP_KEYID_VAL;

        //Enable reconfiguration during runtime
        PMAP->CTL = (PMAP->CTL & ~PMAP_CTL_PRECFG) | PMAP_ENABLE_RECONFIGURATION;

        //Undo Port Mapping for this pin:
        HWREG8(PMAP_BASE + pinNum + pxmap[port]) = PM_NONE;

        //Disable write-access to port mapping registers:
        PMAP->KEYID = 0;

        /* restore pin_to_pwm_index table entry to PWM_MAPPABLE */
        digital_pin_to_pwm_index[pin] = PWM_MAPPABLE;
    }

    /* free up pwm resource */
    used_pwm_port_pins[pwmIndex] = PWM_NOT_IN_USE;

    /* put timer back in pool of available timers if not in use */
    timerId = pwmIndex >> 2;

    if (timer_ccrs_in_use[timerId]) {
        timer_ccrs_in_use[timerId] -= 1;
        if (timer_ccrs_in_use[timerId] == 0) {
            Timer_setAvailMask(Timer_getAvailMask() | (1 << timerId));
        }
    }

    Hwi_restore(hwiKey);
}

/*
 * analogRead() support
 */

int8_t analogReadShift = 4; /* 14 - 4 = 10 bits by default */

/*
 * \brief           configure the A/D reference voltage
 * \param mode      DEFAULT, INTERNAL, EXTERNAL, ...
 * \return          void
 */
void analogReference(uint16_t mode)
{
    uint_fast16_t refVoltage;
    uint8_t i;

    switch (mode) {
        default:
        case DEFAULT:  /* Use VCC as reference (3.3V) */
            refVoltage = ADCMSP432_REF_VOLTAGE_VDD;
            break;

        case INTERNAL1V2:
            refVoltage = ADCMSP432_REF_VOLTAGE_INT_1_2V;
            break;

        case INTERNAL1V45:
            refVoltage = ADCMSP432_REF_VOLTAGE_INT_1_45V;
            break;

        case INTERNAL:
        case INTERNAL2V5:
            refVoltage = ADCMSP432_REF_VOLTAGE_INT_2_5V;
            break;

        case EXTERNAL:
            refVoltage = ADCMSP432_REF_VOLTAGE_EXT;
            break;
    }

    /* update all adc HWAttrs accordingly */
    for (i = 0; i < 24; i++) {
        adcMSP432HWAttrs[i].refVoltage = refVoltage;
    }
}

/*
 * This internal API is used to de-configure a pin that has been
 * put in analogRead() mode.
 *
 * It is called by pinMap() when a pin's function is
 * being modified.
 */
void stopAnalogRead(uint8_t pin)
{
    stopAnalogReadFxnPtr(pin);
}

void stopAnalogReadFxn(uint8_t pin)
{
    uint8_t adcIndex = digital_pin_to_adc_index[pin];
    uint_fast8_t port;
    uint_fast16_t pinMask;

    /* Close PWM port */
    ADC_close((ADC_Handle)&(ADC_config[adcIndex]));

    port = (GPIOMSP432_config.pinConfigs[pin] & 0xffff) >> 8;
    pinMask = GPIOMSP432_config.pinConfigs[pin] & 0xff;

    /* Place Pin in NORMAL GPIO mode */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(port, pinMask,
                                                 GPIO_PRIMARY_MODULE_FUNCTION);

    digital_pin_to_pin_function[pin] = PIN_FUNC_UNUSED;
}

/*
 * \brief sets the number of bits to shift the value read by ADCFIFORead()
 */
void analogReadResolution(uint16_t bits)
{
    analogReadShift = 14 - bits;
}

