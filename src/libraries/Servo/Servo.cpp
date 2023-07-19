/*
 Servo.cpp - Interrupt driven Servo library for the MSP432 using Timer32 timers
 Copyright (c) 2023 chris miller.  All right reserved.

 Adapted from Energia Servo library. Copyright (c) 2009 Michael Margolis.
 2014: Modified by Robert Wessels for the CC3200

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <Energia.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/power/PowerMSP432.h>
#include <ti/drivers/timer/TimerMSP432.h>

#include "Servo.h"
#include <stdlib.h>

/** variables and functions common to all Servo instances **/
static int_fast16_t servoPerfChangeNotify(uint_fast16_t eventType,
    uintptr_t eventArg, uintptr_t clientArg);
static void ServoIntHandler(Timer_Handle handle);

//volatile unsigned long ticksPerMicrosecond;  // Holds the calculated value
uint16_t servoAssignedMask;
uint32_t remainderPulseWidth;
uint32_t currentMclkFreq;
int servosAttached = 0;
int currentServo = 0;
bool servoInitialized = false;

typedef struct
{
    unsigned int pin;
    uint32_t pulseWidthUS;
    uint32_t pulseWidthTicks;
    bool enabled;
} servo_t;
static servo_t servos[SERVOS_PER_TIMER];


//extern Timer_Config Timer_config[];    // Timer_Handle is (Timer_Config *)
Timer_Handle timerHandle = NULL;


/*
 * When a new servo is created:
 * Initialize the servo module if it has not been initialized already.
 * Add the servo to the assigned servos mask with a new index.
 */
Servo::Servo()
{
    this->index = INVALID_SERVO;

    // Look for a free servo index.
    for (int i = 0; i < SERVOS_PER_TIMER; i++) {
        if (((servoAssignedMask >> i) & 1) == 0) {
            // Save the index for this instance of Servo.
            this->index = i;

            // Mark the spot in the mask.
            servoAssignedMask |= (1 << i);

            // Stop searching for free slots.
            break;
        }
    }
}

//! Write a pulse width of the given number of microseconds to the Servo's pin
void Servo::writeMicroseconds(int value)
{
    if (!this->attached())    return;
    if(value < this->min) value = this->min;
    if(value > this->max) value = this->max;

    /*
     * For Timer_PERIOD_US, rawPeriod refers to time in US, and period refers
     *     to count in ticks. Unfortunately, period does not get updated with
     *  changes to rawPeriod, so need to calculate period in tick count for
     *     each cycle. Assuming MCLK unchanged and Timer32 prescale remains 1:1.
     */
    servos[this->index].pulseWidthUS = value;
    servos[this->index].pulseWidthTicks = (uint32_t)((value / 1000000.0f) 
                                                        * currentMclkFreq);
    calculatePeriodRemainder();
}

//! Write a pulse width of the given degrees (if in the appropriate range to be degrees)
//! or of the specified number of microseconds (if in the appropriate range to be microseconds)
void Servo::write(int value)
{
    if (!this->attached())    return;
    // treat values less than the min pulse width as angles in degrees 
    //    (valid values in microseconds are handled as microseconds)
    if (value <= 180) {
        if(value < 0) value = 0;

        value = map(value, 0, 180, this->min,  this->max);
    }
    this->writeMicroseconds(value);
}

//! Returns the current state of Servo object
bool Servo::attached()
{
    if (this->index < SERVOS_PER_TIMER) {
        return servos[this->index].enabled;
    }
    return false;
}

//! Returns the current pulse width of the Servo's signal, in microseconds
int Servo::readMicroseconds()
{
    if (!this->attached())    return NOT_ATTACHED;
    return servos[this->index].pulseWidthUS;
}

//! Returns the current position of the Servo, in degrees
int Servo::read() // return the value as degrees
{
    if (!this->attached())    return NOT_ATTACHED;
    return  map( this->readMicroseconds()+1, this->min, this->max, 0, 180);
}

//! Attach the Servo to the given pin (and, if specified, with the given range of legal pulse widths)
unsigned int Servo::attach(unsigned int pin, int min, int max)
{
    if (this->index >= SERVOS_PER_TIMER) {
        return INVALID_SERVO;
    }
    // If the module has not been initialized
    if (!servoInitialized) {
        // Initialize it.
        if (!initServo() ) {
            return INVALID_SERVO;
        }
        Timer_start(timerHandle);
    }

    this->min = min;
    this->max = max;

    servos[this->index].pin = pin;

    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);

    servos[this->index].enabled = true;
    servosAttached++;

    return this->index;
}

//! Detach the Servo from its pin
void Servo::detach()
{
    if (!this->attached())    return;
    // Disable, clean up
    servos[this->index].enabled = false;
    servos[this->index].pulseWidthUS = DEFAULT_SERVO_PULSE_WIDTH;
    servos[this->index].pulseWidthTicks = (uint32_t)((DEFAULT_SERVO_PULSE_WIDTH 
                                                / 1000000.0f) * currentMclkFreq);
    calculatePeriodRemainder();

    // Clear the bit in the assigned mask.
    servoAssignedMask &= ~(1 << this->index);

    digitalWrite(servos[this->index].pin, LOW);

    servosAttached--;
    if (servosAttached == 0) {
        releaseServo();
    }
}

/** static functions **/
//! Reserve and initialize a Timer32 resource for the Servo 
static bool initServo(void) {
    Timer_Params timerParams;

    Timer_init();
    Timer_Params_init(&timerParams);
    timerParams.period = DEFAULT_SERVO_PULSE_WIDTH;
    timerParams.timerCallback = ServoIntHandler;
    timerParams.timerMode = Timer_ONESHOT_CALLBACK;    //.runMode = Timer_RunMode_ONESHOT;    
    timerParams.periodUnits = Timer_PERIOD_US;    //.periodType = Timer_PeriodType_MICROSECS;    
//    timerHandle = Timer_create(Timer_ANY, ServoIntHandler, &timerParams, &eb);

    // Use Timer32 timers to avoid conflict with PWMs
//    timerHandle = (Timer_Handle)(&Timer_config[Board_TIMER_T32_1]);
//    timerHandle = timerHandle->fxnTablePtr->openFxn(timerHandle, &timerParams);
    timerHandle = Timer_open(1, &timerParams);//Board_TIMER_T32_1

    if (NULL == timerHandle) {
        timerHandle = Timer_open(0, &timerParams);//Board_TIMER_T32_0

        if (NULL == timerHandle) {
            return false;
        }        
    }

    servoAssignedMask = 0;
    remainderPulseWidth = 0;
    currentServo = 0;
    servosAttached = 0;

    PowerMSP432_Freqs powerFreqs;
    PowerMSP432_getFreqs(Power_getPerformanceLevel(), &powerFreqs);
    currentMclkFreq = powerFreqs.MCLK;

    for (int i = 0; i < SERVOS_PER_TIMER; i++) {
        servos[i].pin = 0;
        servos[i].pulseWidthUS = DEFAULT_SERVO_PULSE_WIDTH;
        servos[i].pulseWidthTicks = (uint32_t)((DEFAULT_SERVO_PULSE_WIDTH 
                                        / 1000000.0f) * currentMclkFreq);
        servos[i].enabled = false;
    }

    calculatePeriodRemainder();

    // It has been initialized, prevent further calls to initServo().
    servoInitialized = true;
    return true;
}

// Release Timer32 resources when all servos detached
static void releaseServo()
{
    Timer_close(timerHandle);
    servoInitialized = false;
    timerHandle = NULL;
}

// Calculate the new period remainder in Timer32 ticks
static void calculatePeriodRemainder()
{
    uint32_t servoPeriodSum = 0;
    for (int i = 0; i < SERVOS_PER_TIMER; i++) {
        servoPeriodSum += servos[i].pulseWidthUS;
    }
    if (servoPeriodSum < REFRESH_INTERVAL) {
        remainderPulseWidth = (uint32_t)(((REFRESH_INTERVAL - servoPeriodSum) 
                                        / 1000000.0f) * currentMclkFreq);
    } else {
        remainderPulseWidth = (uint32_t)((DEFAULT_SERVO_PULSE_WIDTH 
                                        / 1000000.0f) * currentMclkFreq);
    }
}

//! ISR for generating the pulse widths
static void ServoIntHandler(Timer_Handle handle)
{
    // Get the pulse width value for the current servo from the array
    // and reload the timer with the new pulse width count value
    // if we have already serviced all servos (currentServo = MAX_SERVO_NO)
    // then this value should be the 20ms period value

    TimerMSP432_Object *object = (TimerMSP432_Object *)handle->object;
    if (currentServo < SERVOS_PER_TIMER) {
        object->period = servos[currentServo].pulseWidthTicks;
    } else {
        object->period = remainderPulseWidth;
    }

    // End the servo pulse set previously (if any)
    if (currentServo > 0) {     // If not the 1st Servo....
        if (servos[currentServo - 1].enabled) {
            digitalWrite(servos[currentServo - 1].pin, LOW);
        }
    }

    // Set the current servo pin HIGH
    if (currentServo < SERVOS_PER_TIMER) {
        if (servos[currentServo].enabled) {
            digitalWrite(servos[currentServo].pin, HIGH);
        }
        currentServo++;  // Advance to next servo for processing next time
    } else {
        currentServo = 0; // Start all over again
    }

    Timer_start(timerHandle);
}
