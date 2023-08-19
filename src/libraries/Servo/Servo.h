/** 
 * @file Servo.h 
 * 
 * Interrupt driven Servo library for the MSP432 using Timer32 timers
 * 
 * 
 * Copyright (c) 2023 chris miller.  All right reserved.
 * 
 * Adapted from Energia Servo library. Copyright (c) 2009 Michael Margolis.
 * 2014: Modified by Robert Wessels for the CC3200
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */


#ifndef SERVO_H
#define SERVO_H

/** \addtogroup ServoClass  Servo controller class
 * @{
 * Drivers for controlling up to 8 servos using a single Timer32 timer. See Servo 
 * class for details.
 */
// Hardware limitations information

/** Default minimum positive pulse-width (in microseconds) for servo control signal */
#define MIN_SERVO_PULSE_WIDTH 		510
/** Default maximum positive pulse-width (in microseconds) for servo control signal */
#define MAX_SERVO_PULSE_WIDTH 		2490
/** Default positive pulse-width (in microseconds) for servo center position */
#define DEFAULT_SERVO_PULSE_WIDTH   1500
/** Default period (in microseconds) for servo control signal */
#define REFRESH_INTERVAL 		    20000

// Other defines

/** Number of Servo objects supported by single timer */
#define SERVOS_PER_TIMER 	8
/** Index indicating invalid servo (not successfully attached) */
#define INVALID_SERVO 		255
/** Index indicating invalid servo (not successfully attached) */
#define NOT_ATTACHED        -1


/**
 * Servo class
 * 
 * Up to 8 servo objects may be instantiated in a single program.
 * All Servo objects will share a single Timer32 timer for generating 
 * PWM control signals. Uses timer interrupts.
 */
class Servo
{
private:
	unsigned int index;
	int min;
	int max;
public:
    /** 
     * Creat instance of Servo class. May create up to 8 instances. 
     * Instances must be initialized using attach() before use.
     */
	Servo();
    /**
     * Initializes instance of Servo and attaches pin.
     * 
     * \param pin   Arduino pin number for servo control signal
     * \param min   minimum pulse width in uSeconds (Default: 510)
     * \param max   maximum pulse width in uSeconds (Default: 2490)
     * 
     * \returns index of Servo instance, or 255 if failed
     */
	unsigned int attach(unsigned int pin, int min = MIN_SERVO_PULSE_WIDTH, 
							int max = MAX_SERVO_PULSE_WIDTH);
    /** Detaches pin and disables instance of Servo. 
     * No effect if not previously attached. */
	void detach();
    /**
     * Set pulse width of servo control signal.
     * 
     * \param value   pulse width of servo control signal in microseconds
     */
	void writeMicroseconds(int value);
    /**
     * Read pulse width of servo control signal.
     * 
     * \returns   pulse width of servo control signal in microseconds
     */
	int readMicroseconds();
    /**
     * Set angle of servo. Defaults to *writeMicroseconds* if value is above
     *  180 degrees.
     * 
     * \param value   angle of servo (0 to 180)
     */
	void write(int value);
    /**
     * Read current angle of servo based on control signal setting.
     * 
     * \returns   angle of servo
     */
	int read();
    /**
     * Check if Servo instance is attached.
     * 
     * \returns   true if servo is attached and active
     */
	bool attached();
};

/** @}*/

#endif // SERVO_H
