/*
 * Energia Robot Library for Texas Instruments' Robot System Learning Kit (RSLK)
 * Line Sensor Display Example
 *
 * Summary:
 * This example has the TI Robotic System Learning Kit (TI RSLK) display values
 * from the line following sensors. This example works on a dark floor with a 
 * white line, but may be changed by modifying the _lineColor_ value. The robot 
 * first needs to be calibrated (on floor without line). Then place the robot
 * over the line to display sensor values. Does not require powering motors.
 *
 * How to run:
 * 1) Place the robot on floor away from line to calibrate to background surface.
 * 2) Push left button on Launchpad to have the robot perform calibration.
 * 3) Place the robot over the line you want it to follow.
 * 4) Observe line sensor values through Serial.
 *
 * Parts Info:
 * o Black eletrical tape or white electrical tape. Masking tape does not work well
 *   with IR sensors.
 *
 * Learn more about the classes, variables and functions used in this library by going to:
 * https://ndroid.github.io/msp432-core/RSLK/
 *
 * Learn more about the TI RSLK by going to http://www.ti.com/rslk
 *
 * created by chris miller 
 * adapted from example by Franklin Cooper Jr.
 *
 * This example code is in the public domain.
 */

/* Include RSLK library */
#include "SimpleRSLK.h"

/* Valid values are either:
 *  DARK_LINE  if your floor is lighter than your line
 *  LIGHT_LINE if your floor is darker than your line
 */
const uint8_t lineColor = LIGHT_LINE;

uint16_t sensorVal[LS_NUM_SENSORS];
uint16_t sensorCalVal[LS_NUM_SENSORS];

void setup()
{
    Serial.begin(115200);

    setupRSLK();
    /* Left button on Launchpad */
    setupWaitBtn(LP_LEFT_BTN);
    /* Red led in rgb led */
    setupLed(RED_LED);
}

void floorCalibration()
{
    /* Place Robot On Floor (no line) */
    delay(2000);
    String btnMsg = "Push left button on Launchpad to begin calibration.\n";
    btnMsg += "Make sure the robot is on the floor away from the line.\n";
    /* Wait until button is pressed to start robot */
    waitBtnPressed(LP_LEFT_BTN, btnMsg, RED_LED);

    delay(500);
    Serial.println("Running calibration on floor");

    /* Must be called prior to using getLinePosition() or readCalLineSensor() */
    calibrateLineSensor(lineColor);

    Serial.println("Reading floor values complete");
    delay(500);
}

bool isCalibrationComplete = false;
void loop()
{
    /* Run this setup only once */
    if (isCalibrationComplete == false) {
        floorCalibration();
        isCalibrationComplete = true;
    }

    /* Get line sensor values */
    readRawLineSensor(sensorVal);
    readCalLineSensor(sensorCalVal);
    uint32_t linePos = getLinePosition();

    Serial.println("Raw sensor values:");
    for (int i = 0; i < LS_NUM_SENSORS; i++) {
        Serial.print("\t"); Serial.print(sensorVal[i]);
    }
    Serial.println();

    Serial.println("Calibrated sensor values:");
    for (int i = 0; i < LS_NUM_SENSORS; i++) {
        Serial.print("\t"); Serial.print(sensorCalVal[i]);
    }
    Serial.println();

    Serial.print("Line position: "); Serial.println(linePos);
    Serial.println();
    delay(1000);
}
