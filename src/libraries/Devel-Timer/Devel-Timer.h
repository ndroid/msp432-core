//
// Developer library to assist in understanding timer resource allocation in Energia at runtime.
//
// Created by chris miller
// miller4@rose-hulman.edu
// modified 7/21/23
//
// Copyright © chris miller, 2023
//

#ifndef DEVEL_TIMER_H
#define DEVEL_TIMER_H


/**
 * Get timer_A availability status mask from TI Timer driver
 * 
 * @param stream    pointer to print object (e.g. Serial) to print output
 */
uint8_t getTimerAvailMask();

/**
 * Print status of timers managed by TI TimerMSP432 driver
 * 
 * @param stream    pointer to print object (e.g. Serial) to print output
 */
void getTimerMSP432Status(Print *stream);

/**
 * Print peripheral register configuration of Timer_A timer 
 * 
 * @param timerIndex    index of Timer_A timer (0 to 3)
 * @param stream        pointer to print object (e.g. Serial) to print output
 */
void printTimerAConfig(int timerIndex, Print *stream);

/**
 * Print peripheral register configuration of Timer32 timer 
 * 
 * @param timerIndex    index of Timer32 timer (1 to 2)
 * @param stream        pointer to print object (e.g. Serial) to print output
 */
void printTimer32Config(int timerIndex, Print *stream);

/**
 * Print peripheral register configuration of SysTick timer 
 * 
 * @param stream        pointer to print object (e.g. Serial) to print output
 */
void printSysTickConfig(Print *stream);

/**
 * Print peripheral register configuration of Clock System 
 * 
 * @param stream        pointer to print object (e.g. Serial) to print output
 */
void printClockSysConfig(Print *stream);

/**
 * Estimate period of Timer_A CCR interrupts. If the timer is in continuous mode,
 *  it is not possible from the configuration to determine the period. This attempts 
 *  to determine the period by comparing the CCR and R register values over an 
 *  interval of time and recording the maximum difference. The period is reported
 *  in timer ticks.
 * 
 * @param timerIndex    index of Timer_A timer (0 to 3)
 * @param duration      time in milliseconds to observe registers
 * @param stream        pointer to print object (e.g. Serial) to print output
 * 
 * @return estimate of CCR interrupt period in timer ticks
 */
uint16_t getTimerAReloadDiff(int timerIndex, int duration, Print *stream);


#endif  // DEVEL_TIMER_H
