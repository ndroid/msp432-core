/*
 * Copyright (c) 2018, Texas Instruments Incorporated
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

/*
 *  ======== Board_init.c ========
 *  This file is responsible for setting up the board specific items for the
 *  MSP_EXP432P4111 Launch Pad board.
 */

#include <stdbool.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerMSP432.h>

#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/rom.h>
#include <ti/devices/msp432p4xx/driverlib/rom_map.h>
#include <ti/devices/msp432p4xx/driverlib/adc14.h>
#include <ti/devices/msp432p4xx/driverlib/dma.h>
#include <ti/devices/msp432p4xx/driverlib/gpio.h>
#include <ti/devices/msp432p4xx/driverlib/i2c.h>
#include <ti/devices/msp432p4xx/driverlib/interrupt.h>
#include <ti/devices/msp432p4xx/driverlib/pmap.h>
#include <ti/devices/msp432p4xx/driverlib/ref_a.h>
#include <ti/devices/msp432p4xx/driverlib/spi.h>
#include <ti/devices/msp432p4xx/driverlib/timer_a.h>
#include <ti/devices/msp432p4xx/driverlib/timer32.h>
#include <ti/devices/msp432p4xx/driverlib/uart.h>
#include <ti/devices/msp432p4xx/driverlib/wdt_a.h>

#include "Board.h"

/*
 *  =============================== ADC ===============================
 */
#include <ti/drivers/ADC.h>
#include <ti/drivers/adc/ADCMSP432.h>

void ADCMSP432_close(ADC_Handle handle);
int_fast16_t ADCMSP432_control(ADC_Handle handle, uint_fast16_t cmd, void *arg);
int_fast16_t ADCMSP432_convert(ADC_Handle handle, uint16_t *value);
uint32_t ADCMSP432_convertRawToMicroVolts(ADC_Handle handle,
    uint16_t rawAdcValue);
void ADCMSP432_init(ADC_Handle handle);
ADC_Handle ADCMSP432_open(ADC_Handle handle, ADC_Params *params);

/* ADC function table for ADCMSP432 implementation */
const ADC_FxnTable myADCMSP432_fxnTable = {
    ADCMSP432_close,
    NULL, /* ADCMSP432_control, */
    ADCMSP432_convert,
    NULL, /* ADCMSP432_convertRawToMicroVolts, */
    ADCMSP432_init,
    ADCMSP432_open
};

/* ADC objects */
ADCMSP432_Object adcMSP432Objects[Board_ADCCOUNT];

/* ADC configuration structure */
ADCMSP432_HWAttrsV1 adcMSP432HWAttrs[Board_ADCCOUNT] = {
    {
        .adcPin = ADCMSP432_P5_5_A0,
        .refVoltage = ADCMSP432_REF_VOLTAGE_VDD,
        .resolution = ADC_14BIT
    },
    {
        .adcPin = ADCMSP432_P5_4_A1,
        .refVoltage = ADCMSP432_REF_VOLTAGE_VDD,
        .resolution = ADC_14BIT
    },
    {
        .adcPin = ADCMSP432_P5_3_A2,
        .refVoltage = ADCMSP432_REF_VOLTAGE_VDD,
        .resolution = ADC_14BIT
    },
    {
        .adcPin = ADCMSP432_P5_2_A3,
        .refVoltage = ADCMSP432_REF_VOLTAGE_VDD,
        .resolution = ADC_14BIT
    },
    {
        .adcPin = ADCMSP432_P5_1_A4,
        .refVoltage = ADCMSP432_REF_VOLTAGE_VDD,
        .resolution = ADC_14BIT
    },
    {
        .adcPin = ADCMSP432_P5_0_A5,
        .refVoltage = ADCMSP432_REF_VOLTAGE_VDD,
        .resolution = ADC_14BIT
    },
    {
        .adcPin = ADCMSP432_P4_7_A6,
        .refVoltage = ADCMSP432_REF_VOLTAGE_VDD,
        .resolution = ADC_14BIT
    },
    {
        .adcPin = ADCMSP432_P4_6_A7,
        .refVoltage = ADCMSP432_REF_VOLTAGE_VDD,
        .resolution = ADC_14BIT
    },
    {
        .adcPin = ADCMSP432_P4_5_A8,
        .refVoltage = ADCMSP432_REF_VOLTAGE_VDD,
        .resolution = ADC_14BIT
    },
    {
        .adcPin = ADCMSP432_P4_4_A9,
        .refVoltage = ADCMSP432_REF_VOLTAGE_VDD,
        .resolution = ADC_14BIT
    },
    {
        .adcPin = ADCMSP432_P4_3_A10,
        .refVoltage = ADCMSP432_REF_VOLTAGE_VDD,
        .resolution = ADC_14BIT
    },
    {
        .adcPin = ADCMSP432_P4_2_A11,
        .refVoltage = ADCMSP432_REF_VOLTAGE_VDD,
        .resolution = ADC_14BIT
    },
    {
        .adcPin = ADCMSP432_P4_1_A12,
        .refVoltage = ADCMSP432_REF_VOLTAGE_VDD,
        .resolution = ADC_14BIT
    },
    {
        .adcPin = ADCMSP432_P4_0_A13,
        .refVoltage = ADCMSP432_REF_VOLTAGE_VDD,
        .resolution = ADC_14BIT
    },
    {
        .adcPin = ADCMSP432_P6_1_A14,
        .refVoltage = ADCMSP432_REF_VOLTAGE_VDD,
        .resolution = ADC_14BIT
    },
    {
        .adcPin = ADCMSP432_P6_0_A15,
        .refVoltage = ADCMSP432_REF_VOLTAGE_VDD,
        .resolution = ADC_14BIT
    },
    {
        .adcPin = ADCMSP432_P9_1_A16,
        .refVoltage = ADCMSP432_REF_VOLTAGE_VDD,
        .resolution = ADC_14BIT
    },
    {
        .adcPin = ADCMSP432_P9_0_A17,
        .refVoltage = ADCMSP432_REF_VOLTAGE_VDD,
        .resolution = ADC_14BIT
    },
    {
        .adcPin = ADCMSP432_P8_7_A18,
        .refVoltage = ADCMSP432_REF_VOLTAGE_VDD,
        .resolution = ADC_14BIT
    },
    {
        .adcPin = ADCMSP432_P8_6_A19,
        .refVoltage = ADCMSP432_REF_VOLTAGE_VDD,
        .resolution = ADC_14BIT
    },
    {
        .adcPin = ADCMSP432_P8_5_A20,
        .refVoltage = ADCMSP432_REF_VOLTAGE_VDD,
        .resolution = ADC_14BIT
    },
    {
        .adcPin = ADCMSP432_P8_4_A21,
        .refVoltage = ADCMSP432_REF_VOLTAGE_VDD,
        .resolution = ADC_14BIT
    },
    {
        .adcPin = ADCMSP432_P8_3_A22,
        .refVoltage = ADCMSP432_REF_VOLTAGE_VDD,
        .resolution = ADC_14BIT
    },
    {
        .adcPin = ADCMSP432_P8_2_A23,
        .refVoltage = REF_A_VREF1_45V,
        .resolution = ADC_8BIT
    }
};

const ADC_Config ADC_config[Board_ADCCOUNT] = {
    {
        .fxnTablePtr = &myADCMSP432_fxnTable,
        .object = &adcMSP432Objects[0],
        .hwAttrs = &adcMSP432HWAttrs[0]
    },
    {
        .fxnTablePtr = &myADCMSP432_fxnTable,
        .object = &adcMSP432Objects[1],
        .hwAttrs = &adcMSP432HWAttrs[1]
    },
    {
        .fxnTablePtr = &myADCMSP432_fxnTable,
        .object = &adcMSP432Objects[2],
        .hwAttrs = &adcMSP432HWAttrs[2]
    },
    {
        .fxnTablePtr = &myADCMSP432_fxnTable,
        .object = &adcMSP432Objects[3],
        .hwAttrs = &adcMSP432HWAttrs[3]
    },
    {
        .fxnTablePtr = &myADCMSP432_fxnTable,
        .object = &adcMSP432Objects[4],
        .hwAttrs = &adcMSP432HWAttrs[4]
    },
    {
        .fxnTablePtr = &myADCMSP432_fxnTable,
        .object = &adcMSP432Objects[5],
        .hwAttrs = &adcMSP432HWAttrs[5]
    },
    {
        .fxnTablePtr = &myADCMSP432_fxnTable,
        .object = &adcMSP432Objects[6],
        .hwAttrs = &adcMSP432HWAttrs[6]
    },
    {
        .fxnTablePtr = &myADCMSP432_fxnTable,
        .object = &adcMSP432Objects[7],
        .hwAttrs = &adcMSP432HWAttrs[7]
    },
    {
        .fxnTablePtr = &myADCMSP432_fxnTable,
        .object = &adcMSP432Objects[8],
        .hwAttrs = &adcMSP432HWAttrs[8]
    },
    {
        .fxnTablePtr = &myADCMSP432_fxnTable,
        .object = &adcMSP432Objects[9],
        .hwAttrs = &adcMSP432HWAttrs[9]
    },
    {
        .fxnTablePtr = &myADCMSP432_fxnTable,
        .object = &adcMSP432Objects[10],
        .hwAttrs = &adcMSP432HWAttrs[10]
    },
    {
        .fxnTablePtr = &myADCMSP432_fxnTable,
        .object = &adcMSP432Objects[11],
        .hwAttrs = &adcMSP432HWAttrs[11]
    },
    {
        .fxnTablePtr = &myADCMSP432_fxnTable,
        .object = &adcMSP432Objects[12],
        .hwAttrs = &adcMSP432HWAttrs[12]
    },
    {
        .fxnTablePtr = &myADCMSP432_fxnTable,
        .object = &adcMSP432Objects[13],
        .hwAttrs = &adcMSP432HWAttrs[13]
    },
    {
        .fxnTablePtr = &myADCMSP432_fxnTable,
        .object = &adcMSP432Objects[14],
        .hwAttrs = &adcMSP432HWAttrs[14]
    },
    {
        .fxnTablePtr = &myADCMSP432_fxnTable,
        .object = &adcMSP432Objects[15],
        .hwAttrs = &adcMSP432HWAttrs[15]
    },
    {
        .fxnTablePtr = &myADCMSP432_fxnTable,
        .object = &adcMSP432Objects[16],
        .hwAttrs = &adcMSP432HWAttrs[16]
    },
    {
        .fxnTablePtr = &myADCMSP432_fxnTable,
        .object = &adcMSP432Objects[17],
        .hwAttrs = &adcMSP432HWAttrs[17]
    },
    {
        .fxnTablePtr = &myADCMSP432_fxnTable,
        .object = &adcMSP432Objects[18],
        .hwAttrs = &adcMSP432HWAttrs[18]
    },
    {
        .fxnTablePtr = &myADCMSP432_fxnTable,
        .object = &adcMSP432Objects[19],
        .hwAttrs = &adcMSP432HWAttrs[19]
    },
    {
        .fxnTablePtr = &myADCMSP432_fxnTable,
        .object = &adcMSP432Objects[20],
        .hwAttrs = &adcMSP432HWAttrs[20]
    },
    {
        .fxnTablePtr = &myADCMSP432_fxnTable,
        .object = &adcMSP432Objects[21],
        .hwAttrs = &adcMSP432HWAttrs[21]
    },
    {
        .fxnTablePtr = &myADCMSP432_fxnTable,
        .object = &adcMSP432Objects[22],
        .hwAttrs = &adcMSP432HWAttrs[22]
    },
    {
        .fxnTablePtr = &myADCMSP432_fxnTable,
        .object = &adcMSP432Objects[23],
        .hwAttrs = &adcMSP432HWAttrs[23]
    }
};

const uint_least8_t ADC_count = Board_ADCCOUNT;

/*
 *  =============================== DMA ===============================
 */

#include <ti/drivers/dma/UDMAMSP432.h>


#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN(dmaControlTable, 256)
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment=256
#elif defined(__GNUC__)
__attribute__ ((aligned (256)))
#endif
static DMA_ControlTable dmaControlTable[16];

/*
 *  ======== dmaErrorHwi ========
 *  This is the handler for the uDMA error interrupt.
 */
static void dmaErrorHwi(uintptr_t arg)
{
    int status = MAP_DMA_getErrorStatus();
    MAP_DMA_clearErrorStatus();

    /* Suppress unused variable warning */
    (void)status;

    while (1);
}

UDMAMSP432_Object udmaMSP432Object;

const UDMAMSP432_HWAttrs udmaMSP432HWAttrs = {
    .controlBaseAddr = (void *)dmaControlTable,
    .dmaErrorFxn = (UDMAMSP432_ErrorFxn)dmaErrorHwi,
    .intNum = INT_DMA_ERR,
    .intPriority = (~0)
};

const UDMAMSP432_Config UDMAMSP432_config = {
    .object = &udmaMSP432Object,
    .hwAttrs = &udmaMSP432HWAttrs
};

/*
 *  =============================== General ===============================
 */

/*
 *  ======== Board_initGeneral ========
 */
void Board_initGeneral(void)
{
    Power_init();
}

/*
 *  =============================== GPIO ===============================
 */

#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOMSP432.h>

/*
 * Array of Pin configurations
 * NOTE: The order of the pin configurations must coincide with what was
 *       defined in MSP_EXP432P4111.h
 * NOTE: Pins not used for interrupts should be placed at the end of the
 *       array.  Callback entries can be omitted from callbacks array to
 *       reduce memory usage.
 */
GPIO_PinConfig gpioPinConfigs[] = {
    /* port_pin */
    GPIOMSP432_EMPTY_PIN | GPIO_DO_NOT_CONFIG,  /*  0  - dummy */

    /* pins 1-10 */
    GPIOMSP432_EMPTY_PIN | GPIO_DO_NOT_CONFIG,  /*  1  - 3.3V */
    GPIOMSP432_P6_0 | GPIO_DO_NOT_CONFIG,       /*  2  - P6.0_A15 */
    GPIOMSP432_P3_2 | GPIO_DO_NOT_CONFIG,       /*  3  - P3.2_URXD */
    GPIOMSP432_P3_3 | GPIO_DO_NOT_CONFIG,       /*  4  - P3.3_UTXD */
    GPIOMSP432_P4_1 | GPIO_DO_NOT_CONFIG,       /*  5  - P4.1_IO_A12 */
    GPIOMSP432_P4_3 | GPIO_DO_NOT_CONFIG,       /*  6  - P4.3_A10 */
    GPIOMSP432_P1_5 | GPIO_DO_NOT_CONFIG,       /*  7  - P1.5_SPICLK */
    GPIOMSP432_P4_6 | GPIO_DO_NOT_CONFIG,       /*  8  - P4.6_IO_A7 */
    GPIOMSP432_P6_5 | GPIO_DO_NOT_CONFIG,       /*  9  - P6.5_I2CSCL */
    GPIOMSP432_P6_4 | GPIO_DO_NOT_CONFIG,       /*  10 - P6.4_I2CSDA */

    /* pins 11-20 */
    GPIOMSP432_P3_6 | GPIO_DO_NOT_CONFIG,       /*  11 - P3.6_IO */
    GPIOMSP432_P5_2 | GPIO_DO_NOT_CONFIG,       /*  12 - P5.2_IO */
    GPIOMSP432_P5_0 | GPIO_DO_NOT_CONFIG,       /*  13 - P5.0_IO */
    GPIOMSP432_P1_7 | GPIO_DO_NOT_CONFIG,       /*  14 - P1.7_SPIMISO */
    GPIOMSP432_P1_6 | GPIO_DO_NOT_CONFIG,       /*  15 - P1.6_SPIMOSI */
    GPIOMSP432_EMPTY_PIN | GPIO_DO_NOT_CONFIG,  /*  16 - RESET */
    GPIOMSP432_P5_7 | GPIO_DO_NOT_CONFIG,       /*  17 - P5.7_IO */
    GPIOMSP432_P3_0 | GPIO_DO_NOT_CONFIG,       /*  18 - P3.0_IO */
    GPIOMSP432_P2_5 | GPIO_DO_NOT_CONFIG,       /*  19 - P2.5_IO_PWM */
    GPIOMSP432_EMPTY_PIN | GPIO_DO_NOT_CONFIG,  /*  20 - GND */

    /* pins 21-30 */
    GPIOMSP432_EMPTY_PIN | GPIO_DO_NOT_CONFIG,  /*  21 - 5V */
    GPIOMSP432_EMPTY_PIN | GPIO_DO_NOT_CONFIG,  /*  22 - GND */
    GPIOMSP432_P6_1 | GPIO_DO_NOT_CONFIG,       /*  23 - P6.1_A14 */
    GPIOMSP432_P4_0 | GPIO_DO_NOT_CONFIG,       /*  24 - P4.0_A13 */
    GPIOMSP432_P4_2 | GPIO_DO_NOT_CONFIG,       /*  25 - P4.2_A11 */
    GPIOMSP432_P4_4 | GPIO_DO_NOT_CONFIG,       /*  26 - P4.4_A9 */
    GPIOMSP432_P4_5 | GPIO_DO_NOT_CONFIG,       /*  27 - P4.5_A8 */
    GPIOMSP432_P4_7 | GPIO_DO_NOT_CONFIG,       /*  28 - P4.7_A6 */
    GPIOMSP432_P5_4 | GPIO_DO_NOT_CONFIG,       /*  29 - P5.4_IO */
    GPIOMSP432_P5_5 | GPIO_DO_NOT_CONFIG,       /*  30 - P5.5_IO */

    /* pins 31-40 */
    GPIOMSP432_P3_7 | GPIO_DO_NOT_CONFIG,       /*  31 - P3.7_IO */
    GPIOMSP432_P3_5 | GPIO_DO_NOT_CONFIG,       /*  32 - P3.5_IO */
    GPIOMSP432_P5_1 | GPIO_DO_NOT_CONFIG,       /*  33 - P5.1_IO */
    GPIOMSP432_P2_3 | GPIO_DO_NOT_CONFIG,       /*  34 - P2.3_IO */
    GPIOMSP432_P6_7 | GPIO_DO_NOT_CONFIG,       /*  35 - P6.7_IO_CAPT */
    GPIOMSP432_P6_6 | GPIO_DO_NOT_CONFIG,       /*  36 - P6.6_IO_CAPT */
    GPIOMSP432_P5_6 | GPIO_DO_NOT_CONFIG,       /*  37 - P5.6_PWM */
    GPIOMSP432_P2_4 | GPIO_DO_NOT_CONFIG,       /*  38 - P2.4_PWM */
    GPIOMSP432_P2_6 | GPIO_DO_NOT_CONFIG,       /*  39 - P2.6_PWM */
    GPIOMSP432_P2_7 | GPIO_DO_NOT_CONFIG,       /*  40 - P2.7_PWM */

    /* bottom row pins 41-56 */
    GPIOMSP432_P8_5 | GPIO_DO_NOT_CONFIG,       /*  41 - P8.5 */
    GPIOMSP432_P9_0 | GPIO_DO_NOT_CONFIG,       /*  42 - P9.0 */
    GPIOMSP432_P8_4 | GPIO_DO_NOT_CONFIG,       /*  43 - P8.4 */
    GPIOMSP432_P8_2 | GPIO_DO_NOT_CONFIG,       /*  44 - P8.2 */
    GPIOMSP432_P9_2 | GPIO_DO_NOT_CONFIG,       /*  45 - P9.2 */
    GPIOMSP432_P6_2 | GPIO_DO_NOT_CONFIG,       /*  46 - P6.2 */
    GPIOMSP432_P7_3 | GPIO_DO_NOT_CONFIG,       /*  47 - P7.3 */
    GPIOMSP432_P7_1 | GPIO_DO_NOT_CONFIG,       /*  48 - P7.1 */
    GPIOMSP432_P9_4 | GPIO_DO_NOT_CONFIG,       /*  49 - P9.4 */
    GPIOMSP432_P9_6 | GPIO_DO_NOT_CONFIG,       /*  40 - P9.6 */
    GPIOMSP432_P8_0 | GPIO_DO_NOT_CONFIG,       /*  51 - P8.0 */
    GPIOMSP432_P7_4 | GPIO_DO_NOT_CONFIG,       /*  52 - P7.4 */
    GPIOMSP432_P7_6 | GPIO_DO_NOT_CONFIG,       /*  53 - P7.6 */
    GPIOMSP432_P10_0 | GPIO_DO_NOT_CONFIG,      /*  54 - P10.0 */
    GPIOMSP432_P10_2 | GPIO_DO_NOT_CONFIG,      /*  55 - P10_2 */
    GPIOMSP432_P10_4 | GPIO_DO_NOT_CONFIG,      /*  56 - P10.4 */

    /* bottom row pins 57-72 */
    GPIOMSP432_P8_6 | GPIO_DO_NOT_CONFIG,       /*  57 - P8.6 */
    GPIOMSP432_P8_7 | GPIO_DO_NOT_CONFIG,       /*  58 - P8.7 */
    GPIOMSP432_P9_1 | GPIO_DO_NOT_CONFIG,       /*  59 - P9.1 */
    GPIOMSP432_P8_3 | GPIO_DO_NOT_CONFIG,       /*  60 - P8.3 */
    GPIOMSP432_P5_3 | GPIO_DO_NOT_CONFIG,       /*  61 - P5.3 */
    GPIOMSP432_P9_3 | GPIO_DO_NOT_CONFIG,       /*  62 - P9.3 */
    GPIOMSP432_P6_3 | GPIO_DO_NOT_CONFIG,       /*  63 - P6.3 */
    GPIOMSP432_P7_2 | GPIO_DO_NOT_CONFIG,       /*  64 - P7.2 */
    GPIOMSP432_P7_0 | GPIO_DO_NOT_CONFIG,       /*  65 - P7.0 */
    GPIOMSP432_P9_5 | GPIO_DO_NOT_CONFIG,       /*  66 - P9.5 */
    GPIOMSP432_P9_7 | GPIO_DO_NOT_CONFIG,       /*  67 - P9.7 */
    GPIOMSP432_P7_5 | GPIO_DO_NOT_CONFIG,       /*  68 - P7.5 */
    GPIOMSP432_P7_7 | GPIO_DO_NOT_CONFIG,       /*  69 - P7.7 */
    GPIOMSP432_P10_1 | GPIO_DO_NOT_CONFIG,      /*  70 - P10.1 */
    GPIOMSP432_P10_3 | GPIO_DO_NOT_CONFIG,      /*  71 - P10.3 */
    GPIOMSP432_P10_5 | GPIO_DO_NOT_CONFIG,      /*  72 - P10.5 */

    /* virtual pins 73-78 */
    GPIOMSP432_P1_1 | GPIO_DO_NOT_CONFIG,       /*  73 - P1.1 SW1 */
    GPIOMSP432_P1_4 | GPIO_DO_NOT_CONFIG,       /*  74 - P1.4 SW2 */
    GPIOMSP432_P2_0 | GPIO_DO_NOT_CONFIG,       /*  75 - P2.0 RED_LED */
    GPIOMSP432_P2_1 | GPIO_DO_NOT_CONFIG,       /*  76 - P2.1 GREEN_LED */
    GPIOMSP432_P2_2 | GPIO_DO_NOT_CONFIG,       /*  77 - P2.2 BLUE_LED */
    GPIOMSP432_P1_0 | GPIO_DO_NOT_CONFIG,       /*  78 - P1.0 LED1 */
};

/*
 * Array of callback function pointers
 * NOTE: The order of the pin configurations must coincide with what was
 *       defined in MSP_EXP432P4111.h
 * NOTE: Pins not used for interrupts can be omitted from callbacks array to
 *       reduce memory usage (if placed at end of gpioPinConfigs array).
 */
GPIO_CallbackFxn gpioCallbackFunctions[] = {
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

/* The device-specific GPIO_config structure */
const GPIOMSP432_Config GPIOMSP432_config = {
    .pinConfigs = (GPIO_PinConfig *)gpioPinConfigs,
    .callbacks = (GPIO_CallbackFxn *)gpioCallbackFunctions,
    .numberOfPinConfigs = sizeof(gpioPinConfigs)/sizeof(GPIO_PinConfig),
    .numberOfCallbacks = sizeof(gpioCallbackFunctions)/sizeof(GPIO_CallbackFxn),
    .intPriority = (~0)
};

/* Not sure why the SVSLOFF bit is undefined in the latest driverlib msp432p401r.h file */

#define SVSLOFF 0x00000100

/*
 *  ======== Board_initGPIO ========
 */
void Board_initGPIO(void)
{
    /* Terminate all IO pins on the device */
    P1DIR |= 0xFF; P1OUT = 0;
    P2DIR |= 0xFF; P2OUT = 0;
    P3DIR |= 0xFF; P3OUT = 0;
    P4DIR |= 0xFF; P4OUT = 0;
    P5DIR |= 0xFF; P5OUT = 0;
    P6DIR |= 0xFF; P6OUT = 0;
    P7DIR |= 0xFF; P7OUT = 0;
    P8DIR |= 0xFF; P8OUT = 0;
    P9DIR |= 0xFF; P9OUT = 0;
    P10DIR |= 0xFF; P10OUT = 0;

    /* Configure Port PJ.2 and PJ.3 as GPIO and write 0 */
    PJDIR |= (BIT2 | BIT3); PJOUT &= ~(BIT2 | BIT3);

    /* PJ.0 & PJ.1 configured for LFXT IN/OUT */
    PJSEL0 |= BIT0 | BIT1;
    PJSEL1 &= ~(BIT0 | BIT1);

    /* Turn off PSS high-side & low-side supervisors */
    PSS->KEY = PSS_KEY_KEY_VAL;
    PSS->CTL0 |= PSS_CTL0_SVSMHOFF | SVSLOFF;         /* 823 (14) uA -> 809 (3) uA */
    PSS->KEY = 0;

    /* Configure Port PJ.4 and PJ.5 */
    ; /* do nothing (the reset default is to support JTAG) */

    /* set up initial TI-RTOS GPIO pin configurations */
    GPIO_init();
}

/*
 *  =============================== I2C ===============================
 */

#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CMSP432.h>

extern void I2CMSP432_cancel(I2C_Handle handle);
extern void I2CMSP432_close(I2C_Handle handle);
extern int_fast16_t I2CMSP432_control(I2C_Handle handle, uint_fast16_t cmd, void *arg);
extern void I2CMSP432_init(I2C_Handle handle);
extern I2C_Handle I2CMSP432_open(I2C_Handle handle, I2C_Params *params);
extern bool I2CMSP432_transfer(I2C_Handle handle, I2C_Transaction *transaction);

const I2C_FxnTable myI2CMSP432_fxnTable = {
    NULL, /* I2CMSP432_cancel, */
    I2CMSP432_close,
    NULL, /* I2CMSP432_control, */
    I2CMSP432_init,
    I2CMSP432_open,
    I2CMSP432_transfer
};

/* I2C objects */
I2CMSP432_Object i2cMSP432Objects[Board_I2CCOUNT];

/* I2C configuration structure */
const I2CMSP432_HWAttrsV1 i2cMSP432HWAttrs[Board_I2CCOUNT] = {
    {
        .baseAddr = EUSCI_B1_BASE,
        .intNum = INT_EUSCIB1,
        .intPriority = (~0),
        .clockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK,
        .dataPin = I2CMSP432_P6_4_UCB1SDA,
        .clkPin = I2CMSP432_P6_5_UCB1SCL
    },
    {
        .baseAddr = EUSCI_B0_BASE,
        .intNum = INT_EUSCIB0,
        .intPriority = (~0),
        .clockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK,
        .dataPin = I2CMSP432_P1_6_UCB0SDA,
        .clkPin = I2CMSP432_P1_7_UCB0SCL
    }
};

const I2C_Config I2C_config[Board_I2CCOUNT] = {
    {
        .fxnTablePtr = &myI2CMSP432_fxnTable,
        .object = &i2cMSP432Objects[0],
        .hwAttrs = &i2cMSP432HWAttrs[0]
    },
    {
        .fxnTablePtr = &myI2CMSP432_fxnTable,
        .object = &i2cMSP432Objects[1],
        .hwAttrs = &i2cMSP432HWAttrs[1]
    }
};

const uint_least8_t I2C_count = Board_I2CCOUNT;

/*
 *  =============================== NVS ===============================
 *  Non-Volatile Storage configuration.
 */
#include <ti/drivers/NVS.h>
#include <ti/drivers/nvs/NVSMSP432.h>

NVSMSP432_Object nvsMSP432Objects[1];

extern uint8_t __NVS_BASE__;
extern uint8_t __NVS_SIZE__;

NVSMSP432_HWAttrs nvsMSP432HWAttrs[] = {
    {
        .regionBase = (void *)&__NVS_BASE__,   /* base of unused flash aligned on 4k boundary */
        .regionSize = (size_t)(&__NVS_SIZE__)
    },
};

const NVS_Config NVS_config[] = {
    {
        .fxnTablePtr = &NVSMSP432_fxnTable,
        .object = &nvsMSP432Objects[0],
        .hwAttrs = &nvsMSP432HWAttrs[0]
    },
};

int NVS_count = 1;

/*
 *  =============================== Power ===============================
 */

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerMSP432.h>

const PowerMSP432_ConfigV1 PowerMSP432_config = {
    .policyInitFxn = PowerMSP432_initPolicy,
    .policyFxn = PowerMSP432_deepSleepPolicy,
    .initialPerfLevel = 2,
    .enablePolicy = true,
    .enablePerf = true,
    .enableParking = true
};

/*
 *  ======== Board_initPower ========
 */
void Board_initPower(void)
{
    Power_init();
    Power_setConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_0);
}

/*
 *  =============================== PWM ===============================
 */
/* Place into subsections to allow the TI linker to remove items properly */

#include <ti/drivers/PWM.h>
#include <ti/drivers/pwm/PWMTimerMSP432.h>

void PWMTimerMSP432_close(PWM_Handle handle);
int_fast16_t PWMTimerMSP432_control(PWM_Handle handle, uint_fast16_t cmd, void *arg);
void PWMTimerMSP432_init(PWM_Handle handle);
PWM_Handle PWMTimerMSP432_open(PWM_Handle handle, PWM_Params *params);
int_fast16_t PWMTimerMSP432_setDuty(PWM_Handle handle, uint32_t dutyValue);
int_fast16_t PWMTimerMSP432_setPeriod(PWM_Handle handle, uint32_t periodValue);
void PWMTimerMSP432_start(PWM_Handle handle);
void PWMTimerMSP432_stop(PWM_Handle handle);

/* PWM function table for PWMTimerMSP432 implementation */
const PWM_FxnTable myPWMTimerMSP432_fxnTable = {
    PWMTimerMSP432_close,
    NULL, /* PWMTimerMSP432_control, */
    PWMTimerMSP432_init,
    PWMTimerMSP432_open,
    PWMTimerMSP432_setDuty,
    NULL, /* PWMTimerMSP432_setPeriod, */
    NULL, /* PWMTimerMSP432_setDutyAndPeriod, */
    PWMTimerMSP432_start,
    NULL /* PWMTimerMSP432_stop */
};

PWMTimerMSP432_Object pwmTimerMSP432Objects[Board_PWMCOUNT];

/* PWM configuration structure */
PWMTimerMSP432_HWAttrsV2 pwmTimerMSP432HWAttrs[Board_PWMCOUNT] = {
    /* pin mappable PWM channels */
    {
        .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
        .pwmPin = PWMTimerMSP432_P2_0_TA0CCR1A
    },
    {
        .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
        .pwmPin = PWMTimerMSP432_P2_1_TA0CCR2A
    },
    {
        .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
        .pwmPin = PWMTimerMSP432_P2_2_TA0CCR3A
    },
    {
        .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
        .pwmPin = PWMTimerMSP432_P2_3_TA0CCR4A
    },
    {
        .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
        .pwmPin = PWMTimerMSP432_P2_4_TA1CCR1A
    },
    {
        .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
        .pwmPin = PWMTimerMSP432_P2_5_TA1CCR2A
    },
    {
        .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
        .pwmPin = PWMTimerMSP432_P2_6_TA1CCR3A
    },
    {
        .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
        .pwmPin = PWMTimerMSP432_P2_7_TA1CCR4A
    },

    /* fixed pin mapped PWM channels */
    {
        .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
        .pwmPin = PWMTimerMSP432_P2_1_TA0CCR1A
    },
    {
        .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
        .pwmPin = PWMTimerMSP432_P2_2_TA0CCR2A
    },
    {
        .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
        .pwmPin = PWMTimerMSP432_P2_3_TA0CCR3A
    },
    {
        .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
        .pwmPin = PWMTimerMSP432_P2_4_TA0CCR4A
    },
};

const PWM_Config PWM_config[] = {
    {
        .fxnTablePtr = &myPWMTimerMSP432_fxnTable,
        .object = &pwmTimerMSP432Objects[0],
        .hwAttrs = &pwmTimerMSP432HWAttrs[0]
    },
    {
        .fxnTablePtr = &myPWMTimerMSP432_fxnTable,
        .object = &pwmTimerMSP432Objects[1],
        .hwAttrs = &pwmTimerMSP432HWAttrs[1]
    },
    {
        .fxnTablePtr = &myPWMTimerMSP432_fxnTable,
        .object = &pwmTimerMSP432Objects[2],
        .hwAttrs = &pwmTimerMSP432HWAttrs[2]
    },
    {
        .fxnTablePtr = &myPWMTimerMSP432_fxnTable,
        .object = &pwmTimerMSP432Objects[3],
        .hwAttrs = &pwmTimerMSP432HWAttrs[3]
    },
    {
        .fxnTablePtr = &myPWMTimerMSP432_fxnTable,
        .object = &pwmTimerMSP432Objects[4],
        .hwAttrs = &pwmTimerMSP432HWAttrs[4]
    },
    {
        .fxnTablePtr = &myPWMTimerMSP432_fxnTable,
        .object = &pwmTimerMSP432Objects[5],
        .hwAttrs = &pwmTimerMSP432HWAttrs[5]
    },
    {
        .fxnTablePtr = &myPWMTimerMSP432_fxnTable,
        .object = &pwmTimerMSP432Objects[6],
        .hwAttrs = &pwmTimerMSP432HWAttrs[6]
    },
    {
        .fxnTablePtr = &myPWMTimerMSP432_fxnTable,
        .object = &pwmTimerMSP432Objects[7],
        .hwAttrs = &pwmTimerMSP432HWAttrs[7]
    },
    {
        .fxnTablePtr = &myPWMTimerMSP432_fxnTable,
        .object = &pwmTimerMSP432Objects[8],
        .hwAttrs = &pwmTimerMSP432HWAttrs[8]
    },
    {
        .fxnTablePtr = &myPWMTimerMSP432_fxnTable,
        .object = &pwmTimerMSP432Objects[9],
        .hwAttrs = &pwmTimerMSP432HWAttrs[9]
    },
    {
        .fxnTablePtr = &myPWMTimerMSP432_fxnTable,
        .object = &pwmTimerMSP432Objects[10],
        .hwAttrs = &pwmTimerMSP432HWAttrs[10]
    },
    {
        .fxnTablePtr = &myPWMTimerMSP432_fxnTable,
        .object = &pwmTimerMSP432Objects[11],
        .hwAttrs = &pwmTimerMSP432HWAttrs[11]
    },
};

const uint_least8_t PWM_count = Board_PWMCOUNT;

/*
 *  =============================== SPI ===============================
 */

#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPIMSP432DMA.h>

/* SPIMSP432DMA functions */
extern void SPIMSP432DMA_close(SPI_Handle handle);
extern int_fast16_t SPIMSP432DMA_control(SPI_Handle handle, uint_fast16_t cmd, void *arg);
extern void SPIMSP432DMA_init(SPI_Handle handle);
extern SPI_Handle SPIMSP432DMA_open(SPI_Handle handle, SPI_Params *params);
extern bool SPIMSP432DMA_transfer(SPI_Handle handle, SPI_Transaction *transaction);
extern void SPIMSP432DMA_transferCancel(SPI_Handle handle);

/* SPI function table for SPIMSP432DMA implementation */
const SPI_FxnTable mySPIMSP432DMA_fxnTable = {
    SPIMSP432DMA_close,
    NULL, /* SPIMSP432DMA_control, */
    SPIMSP432DMA_init,
    SPIMSP432DMA_open,
    SPIMSP432DMA_transfer,
    NULL /* SPIMSP432DMA_transferCancel */
};

/* SPI objects */
SPIMSP432DMA_Object spiMSP432DMAObjects[Board_SPICOUNT];

/* SPI configuration structure, describing which pins are to be used */
const SPIMSP432DMA_HWAttrsV1 spiMSP432DMAHWAttrs[Board_SPICOUNT] = {
    {
        .baseAddr = EUSCI_B0_BASE,
        .bitOrder = EUSCI_B_SPI_MSB_FIRST,
        .clockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK,
        .defaultTxBufValue = 0xFF,
        .dmaIntNum = INT_DMA_INT1,
        .intPriority = 0xC0,       /* make SPI interrupt one priority higher than default */
        .rxDMAChannelIndex = DMA_CH1_EUSCIB0RX0,
        .txDMAChannelIndex = DMA_CH0_EUSCIB0TX0,
        .clkPin  = SPIMSP432DMA_P1_5_UCB0CLK,
        .simoPin = SPIMSP432DMA_P1_6_UCB0SIMO,
        .somiPin = SPIMSP432DMA_P1_7_UCB0SOMI,
        .stePin  = SPIMSP432DMA_P1_4_UCB0STE,
        .pinMode  = EUSCI_SPI_3PIN,
        .minDmaTransferSize = 16
    },
    {
        .baseAddr = EUSCI_B2_BASE,
        .bitOrder = EUSCI_B_SPI_MSB_FIRST,
        .clockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK,
        .defaultTxBufValue = 0xFF,
        .dmaIntNum = INT_DMA_INT2,
        .intPriority = 0xC0,       /* make SPI interrupt one priority higher than default */
        .rxDMAChannelIndex = DMA_CH5_EUSCIB2RX0,
        .txDMAChannelIndex = DMA_CH4_EUSCIB2TX0,
        .clkPin  = SPIMSP432DMA_P3_5_UCB2CLK,
        .simoPin = SPIMSP432DMA_P3_6_UCB2SIMO,
        .somiPin = SPIMSP432DMA_P3_7_UCB2SOMI,
        .stePin  = SPIMSP432DMA_P3_4_UCB2STE,
        .pinMode  = EUSCI_SPI_3PIN,
        .minDmaTransferSize = 16
    }
};

const SPI_Config SPI_config[] = {
    {
        .fxnTablePtr = &mySPIMSP432DMA_fxnTable,
        .object = &spiMSP432DMAObjects[0],
        .hwAttrs = &spiMSP432DMAHWAttrs[0]
    },
    {
        .fxnTablePtr = &mySPIMSP432DMA_fxnTable,
        .object = &spiMSP432DMAObjects[1],
        .hwAttrs = &spiMSP432DMAHWAttrs[1]
    },
};

const uint_least8_t SPI_count = Board_SPICOUNT;

/*
 *  =============================== Timer ===============================
 */
#include <ti/drivers/Timer.h>
#include <ti/drivers/timer/TimerMSP432.h>

TimerMSP432_Object timerMSP432Objects[Board_TIMERCOUNT];

const TimerMSP432_HWAttrs timerMSP432HWAttrs[Board_TIMERCOUNT] = {
    /* Timer32_0 */
    {
        .timerBaseAddress = TIMER32_0_BASE,
        .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
        .intNum = INT_T32_INT1,
        .intPriority = ~0
    },
    {
        .timerBaseAddress = TIMER32_1_BASE,
        .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
        .intNum = INT_T32_INT2,
        .intPriority = ~0
    },
    /* Timer_A1 */
    {
        .timerBaseAddress = TIMER_A1_BASE,
        .clockSource = TIMER_A_CLOCKSOURCE_ACLK,
        .intNum = INT_TA1_0,
        .intPriority = ~0
    },
    /* Timer_A2 */
    {
        .timerBaseAddress = TIMER_A2_BASE,
        .clockSource = TIMER_A_CLOCKSOURCE_ACLK,
        .intNum = INT_TA2_0,
        .intPriority = ~0
    },
    /* Timer_A3 */
    {
        .timerBaseAddress = TIMER_A3_BASE,
        .clockSource = TIMER_A_CLOCKSOURCE_ACLK,
        .intNum = INT_TA3_0,
        .intPriority = ~0
    }
};

const Timer_Config Timer_config[Board_TIMERCOUNT] = {
    {
        .fxnTablePtr = &TimerMSP432_Timer32_fxnTable,
        .object = &timerMSP432Objects[Board_TIMER_T32_0],
        .hwAttrs = &timerMSP432HWAttrs[Board_TIMER_T32_0]
    },
    {
        .fxnTablePtr = &TimerMSP432_Timer32_fxnTable,
        .object = &timerMSP432Objects[Board_TIMER_T32_1],
        .hwAttrs = &timerMSP432HWAttrs[Board_TIMER_T32_1]
    },
    {
        .fxnTablePtr = &TimerMSP432_Timer_A_fxnTable,
        .object = &timerMSP432Objects[Board_TIMER_TA_1],
        .hwAttrs = &timerMSP432HWAttrs[Board_TIMER_TA_1]
    },
    {
        .fxnTablePtr = &TimerMSP432_Timer_A_fxnTable,
        .object = &timerMSP432Objects[Board_TIMER_TA_2],
        .hwAttrs = &timerMSP432HWAttrs[Board_TIMER_TA_2]
    },
    {
        .fxnTablePtr = &TimerMSP432_Timer_A_fxnTable,
        .object = &timerMSP432Objects[Board_TIMER_TA_3],
        .hwAttrs = &timerMSP432HWAttrs[Board_TIMER_TA_3]
    }
};

const uint_least8_t Timer_count = Board_TIMERCOUNT;

/*
 *  =============================== UART ===============================
 */

#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTMSP432.h>

extern void UARTMSP432_close(UART_Handle handle);
extern int_fast16_t UARTMSP432_control(UART_Handle handle, uint_fast16_t cmd,
        void *arg);
extern void UARTMSP432_init(UART_Handle handle);
extern UART_Handle UARTMSP432_open(UART_Handle handle, UART_Params *params);
extern int_fast32_t UARTMSP432_read(UART_Handle handle, void *buffer, size_t size);
extern void UARTMSP432_readCancel(UART_Handle handle);
extern int_fast32_t UARTMSP432_readPolling(UART_Handle handle, void *buffer,
        size_t size);
extern int_fast32_t UARTMSP432_write(UART_Handle handle, const void *buffer,
        size_t size);
extern void UARTMSP432_writeCancel(UART_Handle handle);
extern int_fast32_t UARTMSP432_writePolling(UART_Handle handle, const void *buffer,
        size_t size);

/* UART function table for UARTMP432 implementation */
const UART_FxnTable myUARTMSP432_fxnTable = {
    UARTMSP432_close,
    UARTMSP432_control,
    UARTMSP432_init,
    UARTMSP432_open,
    UARTMSP432_read,
    NULL, /* UARTMSP432_readPolling, */
    NULL, /* UARTMSP432_readCancel, */
    UARTMSP432_write,
    NULL, /* UARTMSP432_writePolling, */
    NULL /* UARTMSP432_writeCancel, */
};

/* UART objects */
UARTMSP432_Object uartMSP432Objects[Board_UARTCOUNT];
unsigned char uartMSP432RingBuffer0[128];
unsigned char uartMSP432RingBuffer1[128];


/*
 * The baudrate dividers were determined by using the MSP430 baudrate
 * calculator
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
const UARTMSP432_BaudrateConfig uartMSP432Baudrates[] = {
    /* {baudrate, input clock, prescalar, UCBRFx, UCBRSx, oversampling} */
    {
        .outputBaudrate = 115200,
        .inputClockFreq = 24000000,
        .prescalar = 13,
        .hwRegUCBRFx = 0,
        .hwRegUCBRSx = 37,
        .oversampling = 1
    },
    {57600,  24000000,  26,  0, 111, 1},
    {38400,  24000000,  39,  1,   0, 1},
    {19200,  24000000,  78,  2,   0, 1},
    {9600,   24000000, 156,  4,   0, 1},
    {4800,   24000000, 312,  8,   0, 1},

    {115200, 12000000,   6,  8,  32, 1},
    {57600,  12000000,  13,  0,  37, 1},
    {38400,  12000000,  19,  8,  85, 1},
    {19200,  12000000,  39,  1,   0, 1},
    {9600,   12000000,  78,  2,   0, 1},
    {4800,   12000000, 156,  4,   0, 1},

    {115200, 6000000,    3,  4,   2, 1},
    {57600,  6000000,    6,  8,  32, 1},
    {38400,  6000000,    9, 12,  34, 1},
    {19200,  6000000,   19,  8,  85, 1},
    {9600,   6000000,   39,  1,   0, 1},
    {4800,   6000000,   78,  2,   0, 1},

    {115200, 3000000,    1, 10,   0, 1},
    {57600,  3000000,    3,  4,   2, 1},
    {38400,  3000000,    4, 14,   8, 1},
    {19200,  3000000,    9, 12,  34, 1},
    {9600,   3000000,   19,  8,  85, 1},
    {4800,   3000000,   39,  1,   0, 1},

    /* only 3 baud rates supported with */
    /* 32.678KHz input clock            */
    {19200,  32768,      1,  0, 183, 0},
    {9600,   32768,      3,  0, 146, 0},
    {4800,   32768,      6,  0, 238, 0},
};

/* UART configuration structure */
const UARTMSP432_HWAttrsV1 uartMSP432HWAttrs[Board_UARTCOUNT] = {
    {
        .baseAddr = EUSCI_A0_BASE,
        .intNum = INT_EUSCIA0,
        .intPriority = (0xc0),
        .clockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK,
        .bitOrder = EUSCI_A_UART_LSB_FIRST,
        .numBaudrateEntries = sizeof(uartMSP432Baudrates) /
                              sizeof(UARTMSP432_BaudrateConfig),
        .baudrateLUT = uartMSP432Baudrates,
        .ringBufPtr  = uartMSP432RingBuffer0,
        .ringBufSize = sizeof(uartMSP432RingBuffer0),
        .rxPin = UARTMSP432_P1_2_UCA0RXD,
        .txPin = UARTMSP432_P1_3_UCA0TXD,
        .errorFxn = NULL
    },
    {
        .baseAddr = EUSCI_A2_BASE,
        .intNum = INT_EUSCIA2,
        .intPriority = (0xc0),
        .clockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK,
        .bitOrder = EUSCI_A_UART_LSB_FIRST,
        .numBaudrateEntries = sizeof(uartMSP432Baudrates) /
            sizeof(UARTMSP432_BaudrateConfig),
        .baudrateLUT = uartMSP432Baudrates,
        .ringBufPtr  = uartMSP432RingBuffer1,
        .ringBufSize = sizeof(uartMSP432RingBuffer1),
        .rxPin = UARTMSP432_P3_2_UCA2RXD,
        .txPin = UARTMSP432_P3_3_UCA2TXD,
        .errorFxn = NULL
    }
};

const UART_Config UART_config[] = {
    {
        .fxnTablePtr = &myUARTMSP432_fxnTable,
        .object = &uartMSP432Objects[0],
        .hwAttrs = &uartMSP432HWAttrs[0]
    },
    {
        .fxnTablePtr = &myUARTMSP432_fxnTable,
        .object = &uartMSP432Objects[1],
        .hwAttrs = &uartMSP432HWAttrs[1]
    },
};

const uint_least8_t UART_count = Board_UARTCOUNT;

/*
 *  =============================== Watchdog ===============================
 */
/* Place into subsections to allow the TI linker to remove items properly */

#include <ti/drivers/Watchdog.h>
#include <ti/drivers/watchdog/WatchdogMSP432.h>

/* Watchdog objects */
WatchdogMSP432_Object watchdogMSP432Objects[Board_WATCHDOGCOUNT];

/* Watchdog configuration structure */
const WatchdogMSP432_HWAttrs watchdogMSP432HWAttrs[Board_WATCHDOGCOUNT] = {
    {
        .baseAddr = WDT_A_BASE,
        .intNum = INT_WDT_A,
        .intPriority = (~0),
        .clockSource = WDT_A_CLOCKSOURCE_SMCLK,
        .clockDivider = WDT_A_CLOCKDIVIDER_8192K
    }
};

const Watchdog_Config Watchdog_config[] = {
    {
        .fxnTablePtr = &WatchdogMSP432_fxnTable,
        .object = &watchdogMSP432Objects[0],
        .hwAttrs = &watchdogMSP432HWAttrs[0]
    },
};

const uint_least8_t Watchdog_count = Board_WATCHDOGCOUNT;

/*
 *  ======== Board_initWatchdog ========
 */
void Board_initWatchdog(void)
{
    /* Initialize the Watchdog driver */
    Watchdog_init();
}

/*
 *  ======== Board_init ========
 */
void Board_init(void) {
    Board_initGPIO();
    Board_initPower();
}

extern void delay4111(uint32_t milliseconds);

void (*delayFxn)(uint32_t milliseconds) = delay4111;
