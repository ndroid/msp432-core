#ifndef SERVO_H
#define SERVO_H

// Hardware limitations information

#define MIN_SERVO_PULSE_WIDTH 		510
#define MAX_SERVO_PULSE_WIDTH 		2490
#define DEFAULT_SERVO_PULSE_WIDTH   1500
#define REFRESH_INTERVAL 		    20000

// Other defines

#define SERVOS_PER_TIMER 	8
#define INVALID_SERVO 		255
#define NOT_ATTACHED        -1


class Servo
{
private:
	unsigned int index;
	int min;
	int max;
public:
    /* Instance of Servo class. May create up to 8 instances. */
	Servo();
    /*
     * Initializes instance of Servo and attaches pin.
     * 
     * \param pin   Arduino pin number for servo control signal
     * \param min   minimum pulse width in uSeconds (Default: 510)
     * \param max   maximum pulse width in uSeconds (Default: 2490)
     * 
     * returns index of Servo instance, or 255 if failed
     */
	unsigned int attach(unsigned int pin, int min = MIN_SERVO_PULSE_WIDTH, 
							int max = MAX_SERVO_PULSE_WIDTH);
    /* Detaches pin and disables instance of Servo. 
     * No effect if not previously attached. */
	void detach();
    /*
     * Set pulse width of servo control signal.
     * 
     * \param value   pulse width of servo control signal in microseconds
     */
	void writeMicroseconds(int value);
    /*
     * Read pulse width of servo control signal.
     * 
     * returns   pulse width of servo control signal in microseconds
     */
	int readMicroseconds();
    /*
     * Set angle of servo. Defaults to *writeMicroseconds* if value is above
     *  180 degrees.
     * 
     * \param value   angle of servo (0 to 180)
     */
	void write(int value);
    /*
     * Read current angle of servo based on control signal setting.
     * 
     * returns   angle of servo
     */
	int read();
    /*
     * Check if Servo instance is attached.
     * 
     * returns   true if servo is attached and active
     */
	bool attached();
};

#endif // SERVO_H
