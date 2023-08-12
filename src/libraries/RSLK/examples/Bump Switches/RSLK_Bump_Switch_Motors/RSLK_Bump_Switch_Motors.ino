/*
 * Energia Robot Library for Texas Instruments' Robot System Learning Kit (RSLK)
 * Bump Switch w/ Motors Example
 *
 * Summary:
 * This example has the TI Robotic System Learning Kit (TI RSLK) driving forward
 * until it hits an object (ie a bump switch is triggered) then it stops.
 *
 * How to run:
 * 1) Push left button on Launchpad to start the demo
 * 2) Robot will drive forward until the bump switches are triggered
 * 3) Once switches are triggered the robot will halt
 * 4) Push left button again to start demo
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

/* Modify the following line to use an alternate UART interface (i.e. Serial1/2/3) */
#define UART_SERIAL     Serial

const int motorSpeed = 15;

void setup()
{
    UART_SERIAL.begin(115200);
    setupRSLK();
    /* Left button on Launchpad */
    setupWaitBtn(LP_LEFT_BTN);
    /* Red led in rgb led */
    setupLed(RED_LED);
}

void loop()
{
    bool hitObstacle = false;

    UART_SERIAL.println("Push left button on Launchpad to start demo.\n");
    /* Wait until button is pressed to start robot */
    waitBtnPressed(LP_LEFT_BTN, RED_LED);

    /* Wait two seconds before starting */
    delay(2000);

    /* Enable both motors, set their direction and provide a default speed */
    enableMotor(BOTH_MOTORS);
    setMotorDirection(BOTH_MOTORS, MOTOR_DIR_FORWARD);
    setMotorSpeed(BOTH_MOTORS, motorSpeed);

    /* Keep checking if the robot has hit an object */
    while (!hitObstacle) {
        /* Check if any bump switch was pressed */
        if (getBumpSwitchPressed() > 0) {
            hitObstacle = true;
            break;
        }
    }

    UART_SERIAL.println("Collision detected\n");
    disableMotor(BOTH_MOTORS);
}
