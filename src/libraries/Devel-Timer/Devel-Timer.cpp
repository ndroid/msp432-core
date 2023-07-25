//
// Devel-Timer.cpp
//
// Developer library to assist in understanding timer resource allocation in Energia at runtime.
//
// Created by chris miller
// miller4@rose-hulman.edu
// modified 7/21/23
//
// Copyright © chris miller, 2023
//

#include "Energia.h"
#include "msp432.h"
//#include <Board.h>
//#include <ti/sysbios/BIOS.h>
//#include <ti/sysbios/family/arm/msp432/package/internal/Timer.xdc.h>
#include <ti/drivers/timer.h>
#include <ti/drivers/timer/TimerMSP432.h>
#include <ti/sysbios/family/arm/msp432/Timer.h>
//#include <ti/sysbios/hal/Timer.h>
#include "Print.h"
#include "Devel-Timer.h"
 
#define TIMERA_CNT      4
#define TIMER32_CNT     2
#define CCR_CNT         4

/*
 * Helper macro for getting a macro definition as string
 */
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

Timer_A_Type * TimerA_Base[TIMERA_CNT] = {
    TIMER_A0,
    TIMER_A1,
    TIMER_A2,
    TIMER_A3
};

Timer32_Type * Timer32_Base[TIMER32_CNT] = {
    TIMER32_1,
    TIMER32_2
};

/*#define SysTick_BASE        (SCS_BASE +  0x0010UL)                    /*!< SysTick Base Address *
#define SysTick             ((SysTick_Type   *)     SysTick_BASE  )   /*!< SysTick configuration struct *

typedef struct
{
  __IOM uint32_t CTRL;                   /*!< Offset: 0x000 (R/W)  SysTick Control and Status Register *
  __IOM uint32_t LOAD;                   /*!< Offset: 0x004 (R/W)  SysTick Reload Value Register *
  __IOM uint32_t VAL;                    /*!< Offset: 0x008 (R/W)  SysTick Current Value Register *
  __IM  uint32_t CALIB;                  /*!< Offset: 0x00C (R/ )  SysTick Calibration Register *
} SysTick_Type;
*/

//extern struct ti_sysbios_family_arm_msp432_Timer_Module_State *Timer_module;
extern Timer_Config Timer_config[];
extern const uint_least8_t Timer_count;

//extern struct {    uint32_t bitMask;} timerState;
//extern uint32_t timerState;

uint8_t getTimerAvailMask() {
    return Timer_getAvailMask();
}

void getTimerMSP432Status(Print *stream) {
    stream->println("TimerMSP432 driver status");
    for (int i = 0; i < Timer_count; i++) {
        stream->print("\tMSPTimer id: "); stream->print(i);
        stream->print("\tbase: 0x"); stream->print((uint32_t)(((const TimerMSP432_HWAttrs *)Timer_config[i].hwAttrs)->timerBaseAddress), HEX);  //timerMSP432HWAttrs[i]
        stream->print("\tmode: "); stream->print(((TimerMSP432_Object *)Timer_config[i].object)->mode);  //timerMSP432Objects[i].
        stream->print("\tperiod: "); stream->print(((TimerMSP432_Object *)Timer_config[i].object)->period);  //timerMSP432Objects[i].
        stream->print("\tisRunning: "); stream->println(((TimerMSP432_Object *)Timer_config[i].object)->isRunning);  //timerMSP432Objects[i].
    }
}

void printTimerAConfig(int timerIndex, Print *stream) {
    /* Timer_Ax */
    if ((timerIndex < 0) || (timerIndex >= TIMERA_CNT))  return;
    stream->print("Timer_A"); stream->print(timerIndex); stream->print("\t|\tCTL: 0x"); 
        stream->print((TimerA_Base[timerIndex])->CTL, HEX); stream->print("\tR: "); 
        stream->println((TimerA_Base[timerIndex])->R); 
    stream->print("\t\tCCTL0: 0x"); stream->print((TimerA_Base[timerIndex])->CCTL[0], HEX); 
        stream->print("\tCCR0: "); stream->println((TimerA_Base[timerIndex])->CCR[0]); 
    stream->print("\t\tCCTL1: 0x"); stream->print((TimerA_Base[timerIndex])->CCTL[1], HEX); 
        stream->print("\tCCR1: "); stream->println((TimerA_Base[timerIndex])->CCR[1]); 
    stream->print("\t\tCCTL2: 0x"); stream->print((TimerA_Base[timerIndex])->CCTL[2], HEX); 
        stream->print("\tCCR2: "); stream->println((TimerA_Base[timerIndex])->CCR[2]); 
    stream->print("\t\tCCTL3: 0x"); stream->print((TimerA_Base[timerIndex])->CCTL[3], HEX); 
        stream->print("\tCCR3: "); stream->println((TimerA_Base[timerIndex])->CCR[3]); 
    stream->print("\t\tCCTL4: 0x"); stream->print((TimerA_Base[timerIndex])->CCTL[4], HEX); 
        stream->print("\tCCR4: "); stream->println((TimerA_Base[timerIndex])->CCR[4]); 
}

void printTimer32Config(int timerIndex, Print *stream) {
    /* Timer32_1 */
    if ((timerIndex < 1) || (timerIndex > TIMER32_CNT))  return;
    timerIndex--;
    stream->print("Timer32_"); stream->println(timerIndex); 
    stream->print("\t\tCTL: 0x"); stream->print(Timer32_Base[timerIndex]->CONTROL, HEX);
        stream->print("\tLoad: "); stream->println(Timer32_Base[timerIndex]->LOAD); 
}


void printSysTickConfig(Print *stream) {
    /* SysTick */
    stream->println("SysTick timer"); 
    stream->print("\t\tCTL: 0x"); stream->print(SysTick->CTRL, HEX);
    stream->print("\tLoad: "); stream->println(SysTick->LOAD); 
}


void printClockSysConfig(Print *stream) {
    /* Clock source */
    stream->println("Clock source"); 
    stream->print("\t\tCTL0: 0x"); stream->print(CS->CTL0, HEX);
        stream->print("\tCTL1: 0x"); stream->println(CS->CTL1, HEX);
    stream->print("\t\tCTL2: 0x"); stream->print(CS->CTL2, HEX);
        stream->print("\tCTL3: 0x"); stream->println(CS->CTL3, HEX);
    stream->print("\t\tCLKEN: 0x"); stream->print(CS->CLKEN, HEX);
        stream->print("\tSTAT: 0x"); stream->println(CS->STAT, HEX);
}


uint16_t getTimerAReloadDiff(int timerIndex, int duration, Print *stream) {
    uint16_t  current, max, ccr, tmr;
    unsigned long endTime = millis() + duration;

    if ((timerIndex < 0) || (timerIndex >= TIMERA_CNT))  return 0;
    max = 0;
    while (millis() < endTime) {
        tmr = TimerA_Base[timerIndex]->R;
        ccr = TimerA_Base[timerIndex]->CCR[0];
        current = ccr - tmr;
        if (current > max)  max = current;
    }

    return max;
}

