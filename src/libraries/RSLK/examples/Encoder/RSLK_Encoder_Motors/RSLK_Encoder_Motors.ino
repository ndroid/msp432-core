/*
 * Energia Robot Library for Texas Instruments' Robot System Learning Kit (RSLK)
 * Encoder Example
 *
 * Summary:
 * This example has the TI Robotic System Learning Kit (TI RSLK) driving forward
 * by a predefined distance utilizing wheel encoders.
 *
 * How to run:
 * 1) Push left button on Launchpad to start the demo
 * 2) Robot will drive forward by a predefined distance
 * 3) Once distance has been reached the robot will stop
 * 4) Push left button again to start demo again
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

/* Diameter of Romi wheels in inches 
 *  2.7559055" = 7.0 cm (International System of Units) 
 */
const float wheelDiameter = 2.7559055; 

/* Number of encoder (rising) pulses every time the wheel turns completely */
const int cntPerRevolution = 360;

/* How far in inches for the robot to travel */
const int inchesToTravel = 6;

const int wheelSpeed = 15; // Default raw pwm speed for motor.

/* The distance the wheel turns per revolution is equal to the diameter * PI.
 * The distance the wheel turns per encoder pulse is equal to the above divided
 * by the number of pulses per revolution.
 */
float distanceTraveled(float wheel_diam, uint16_t cnt_per_rev, uint8_t current_cnt)
{
    float temp = (wheel_diam * PI * current_cnt) / cnt_per_rev;
    return temp;
}

uint32_t countForDistance(float wheel_diam, uint16_t cnt_per_rev, uint32_t distance)
{
    float temp = (wheel_diam * PI) / cnt_per_rev;
    temp = distance / temp;
    return int(temp);
}

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
    uint16_t totalCount = 0;        // Total amount of encoder pulses received
    uint16_t leftCount, rightCount;

    UART_SERIAL.print("Driving forward distance (inches): ");
    UART_SERIAL.println(inchesToTravel);

    /* Amount of encoder pulses needed to achieve distance */
    uint16_t target = countForDistance(wheelDiameter, cntPerRevolution, inchesToTravel);
    String btnMsg = "Expected count: ";
    btnMsg += target;

    /* Wait until button is pressed to start robot */
    btnMsg += "\nPush left button on Launchpad to start demo.\n";
    UART_SERIAL.println(btnMsg);
    waitBtnPressed(LP_LEFT_BTN, RED_LED);

    delay(2000);

    /* Set the encoder pulses count back to zero */
    resetLeftEncoderCnt();
    resetRightEncoderCnt();

    /* Cause the robot to drive forward */
    setMotorDirection(BOTH_MOTORS, MOTOR_DIR_FORWARD);

    /* "Turn on" the motor */
    enableMotor(BOTH_MOTORS);

    /* Set motor speed */
    setMotorSpeed(BOTH_MOTORS, wheelSpeed);

    /* Drive motor until it has received x pulses */
    while (totalCount < target) {
        leftCount = getEncoderLeftCnt();
        rightCount = getEncoderRightCnt();
        totalCount = (leftCount + rightCount) / 2;
        UART_SERIAL.print("\t"); UART_SERIAL.print(leftCount);
        UART_SERIAL.print("\t"); UART_SERIAL.println(rightCount);
    }

    /* Halt motors */
    disableMotor(BOTH_MOTORS);

    /* Calculate traveled distance from encoder ticks */
    float traveled = distanceTraveled(wheelDiameter, cntPerRevolution, totalCount);
    UART_SERIAL.print("Distance traveled (inches): ");
    UART_SERIAL.println(traveled);
    UART_SERIAL.println();
}
