/*
 * Energia Robot Library for Texas Instruments' Robot System Learning Kit (RSLK)
 * State Machine Example
 *
 * Summary:
 * This example has the TI Robotic System Learning Kit (TI RSLK) drive until an
 * obstacle is hit (ie a bump switch is triggered), then it backs up, turns, and 
 * repeats until a specified distance is traveled. It uses a state machine to 
 * accomplish this task.
 *
 * How to run:
 * 1) Push right button on Launchpad to proceed from WAIT state to GO state
 * 2) Robot will drive forward until the bump switches are triggered
 * 3) Once switches are triggered the robot will back up, turn, and drive forward
 * 4) Robot will repeat (2)(3) until a specified distance is traveled, then stop
 *
 * Learn more about the classes, variables and functions used in this library by going to:
 * https://ndroid.github.io/msp432-core/RSLK/
 *
 * Learn more about the TI RSLK by going to http://www.ti.com/rslk
 *
 * created by Franklin Cooper Jr.
 * modified by chris miller 
 *
 * This example code is in the public domain.
 */

/* Include RSLK library */
#include "SimpleRSLK.h"

const uint16_t motorSpeed = 25;
const uint32_t targetTicks = 50000;

/* Defines constants for state machine states */
typedef enum
{
    START = 0,
    WAIT,
    GO,
    GO2,
    BUMPED1a,
    BUMPED1b,
    DRIVE,
    STOP
} my_state_t;

/* Initialize state machine in START state */
my_state_t state = START;

void setup() {
    /* Set serial communication to 115200 baud rate for MSP432 */
    Serial.begin(115200);
    delay(500);
    Serial.println("Initializing.....");

    /* Run setup code */
    setupRSLK();

    /* Initialize LED pins as outputs */
    setupLed(GREEN_LED);
  
    /* Initialize LaunchPad buttons as inputs */
    pinMode(LP_S1_PIN, INPUT_PULLUP);

    Serial.println("Initializing System Complete.");
}

void loop() {
  //-----------------------------------
  //        Main State Machine
  //-----------------------------------
  switch (state) {

    case START:
        Serial.println("Enter START state");
        toggleCount = 0;
        state = WAIT;
        break;

    case WAIT:
        Serial.println("Enter WAIT state");
        digitalWrite(GREEN_LED, HIGH);
        delay(200);
        if (digitalRead(LP_S1_PIN) == 0) {
            state = GO;
        }
        digitalWrite(GREEN_LED, LOW);
        delay(200);
        if (digitalRead(LP_S1_PIN) == 0) {
            state = GO;
        }
        break;

    case GO:
        Serial.println("Enter GO state");
        /* Start running the motors */
        /* Enables specified motor.
         *  Parameter:
         *   Motor your referencing -> LEFT_MOTOR,  RIGHT_MOTOR,  BOTH_MOTORS
         */
        enableMotor(BOTH_MOTORS);
        setMotorDirection(BOTH_MOTORS, MOTOR_DIR_FORWARD);
        setMotorSpeed(BOTH_MOTORS, motorSpeed);
        
        state = GO2;
        break;

    case GO2:
        Serial.println("Enter GO2 state");
        /* Detect a bump and then switch to bump correction state */
        if(getBumpSwitchPressed() > 0) {
            state = BUMPED1a;
        }

        /* Continue to rotate until done condition is met */
        /* Certain distance traveled or other conditions can be set */
        if (getEncoderLeftCnt() > targetTicks) {
            state = STOP;
        }
        break;

    case BUMPED1a:
        Serial.println("Enter BUMPED1a state");
        /* Stop the motors */
        pauseMotor(BOTH_MOTORS);
        /* Reverse the robot */
        setMotorDirection(BOTH_MOTORS, MOTOR_DIR_BACKWARD);
        resumeMotor(BOTH_MOTORS);
        delay(500);
        state = BUMPED1b;
        break;

    case BUMPED1b:
        Serial.println("Enter BUMPED1b state");
        /* Turn robot to avoid obstacle */
        setMotorSpeed(LEFT_MOTOR, 0);
        setMotorSpeed(RIGHT_MOTOR, motorSpeed);
        delay(100);

        state = DRIVE;
        break;

    case DRIVE:
        Serial.println("Enter DRIVE state");
        /* Put motors back to forward direction */
        setMotorDirection(BOTH_MOTORS, MOTOR_DIR_FORWARD);
        setMotorSpeed(BOTH_MOTORS, motorSpeed);

        state = GO2;
    break;

    case STOP:
        Serial.println("Enter STOP state");
        Serial.println("Press Reset to begin again");
        /* Stop all motors */
        disableMotor(BOTH_MOTORS);
    break;
  }

  delay(20);
}
