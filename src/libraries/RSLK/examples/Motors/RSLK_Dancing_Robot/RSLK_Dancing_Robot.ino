/*
 * Energia Robot Library for Texas Instruments' Robot System Learning Kit (RSLK)
 * Dancing Robot Example
 *
 * Summary:
 * This example has the TI Robotic System Learning Kit (TI RSLK) driving in a 
 * sequence of patterns until it hits an object (ie a bump switch is triggered) 
 * then it stops.
 *
 * How to run:
 * 1) Push left button on Launchpad to start the demo
 * 2) Robot will drive in sequence of patterns until the bump switches are triggered
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

const uint16_t lowSpeed = 15;
const uint16_t fastSpeed = 30;

bool hit_obstacle = false;

/** 
 * Check for collision with bump switches
 * @param delayMS   delay time in milliseconds 
 * @return  true if collision detected within _delayMS_ time
 */
bool checkCollision(uint32_t delayMS) {
  uint32_t endTime = millis() +  delayMS;

  while (millis() < endTime) {
    /* Check if any bump switch was pressed
    *    Returns: mask of bump switch states (bits 0 to 5)
    *      0 -> if bump switch is not pressed
    *      1 -> if bump switch is pressed
    */
    if (getBumpSwitchPressed() > 0) {
      UART_SERIAL.println("Collision detected");
      disableMotor(BOTH_MOTORS);
      hit_obstacle = true;
      return true;
    }
  }
  
  return false;
}

void setup() {
  /* Set serial communication to 115200 baud rate for MSP432 */
  UART_SERIAL.begin(115200);
  delay(500);

  /* Run setup code */
  setupRSLK();

  /* Left button on Launchpad */
  setupWaitBtn(LP_LEFT_BTN);
  /* Initialize LED pins as outputs */
  setupLed(BLUE_LED);
  setupLed(GREEN_LED);
}

void loop() {
  UART_SERIAL.println("Push left button on Launchpad to start demo.\n");
  /* Wait until button is pressed to start robot */
  waitBtnPressed(LP_LEFT_BTN, GREEN_LED);

  /* Wait one second before starting */
  delay(1000);
  digitalWrite(BLUE_LED, HIGH);
  /* Enables specified motor.
   *  Parameter:
   *   Motor your referencing -> LEFT_MOTOR,  RIGHT_MOTOR,  BOTH_MOTORS
   */
  enableMotor(BOTH_MOTORS);

  /* Set direction of motor rotation.
   *  Parameter:
   *   Motor your referencing -> LEFT_MOTOR,  RIGHT_MOTOR,  BOTH_MOTORS
   *   Direction -> MOTOR_DIR_FORWARD,  MOTOR_DIR_BACKWARD
   */
  setMotorDirection(LEFT_MOTOR, MOTOR_DIR_FORWARD);
  setMotorDirection(RIGHT_MOTOR, MOTOR_DIR_FORWARD);
  
  /* Set speed of motor.
   *  Parameter:
   *   Motor your referencing -> LEFT_MOTOR,  RIGHT_MOTOR,  BOTH_MOTORS
   *   Speed -> 0 - 100
   */
  setMotorSpeed(BOTH_MOTORS, lowSpeed);

  hit_obstacle = false;
  while(!hit_obstacle) {
    /* Left turn */
    setMotorSpeed(LEFT_MOTOR, 0);
    setMotorDirection(RIGHT_MOTOR, MOTOR_DIR_FORWARD);
    if (checkCollision(1000))
      break;

    /* 360 spin left */
    setMotorDirection(LEFT_MOTOR, MOTOR_DIR_BACKWARD);
    setMotorSpeed(BOTH_MOTORS, fastSpeed);
    if (checkCollision(800))
      break;
    setMotorSpeed(BOTH_MOTORS, lowSpeed);

    /* Right turn */
    setMotorDirection(LEFT_MOTOR, MOTOR_DIR_FORWARD);
    setMotorSpeed(RIGHT_MOTOR, 0);
    if (checkCollision(1000))
      break;
    
    /* 360 spin right */
    setMotorDirection(RIGHT_MOTOR, MOTOR_DIR_BACKWARD);
    setMotorSpeed(BOTH_MOTORS, fastSpeed);
    if (checkCollision(800))
      break;
    setMotorSpeed(BOTH_MOTORS, lowSpeed);

    /* Left turn in place */
    setMotorDirection(LEFT_MOTOR, MOTOR_DIR_BACKWARD);
    setMotorDirection(RIGHT_MOTOR, MOTOR_DIR_FORWARD);
    if (checkCollision(1000))
      break;

    /* Drive forward */
    setMotorDirection(BOTH_MOTORS, MOTOR_DIR_FORWARD);
    if (checkCollision(500))
      break;

    /* Right turn in place */
    setMotorDirection(LEFT_MOTOR, MOTOR_DIR_FORWARD);
    setMotorDirection(RIGHT_MOTOR, MOTOR_DIR_BACKWARD);
    if (checkCollision(1000))
      break;

    /* Drive backward */
    setMotorDirection(BOTH_MOTORS, MOTOR_DIR_BACKWARD);
    if (checkCollision(500))
      break;

    /* 360 spin right */
    setMotorDirection(LEFT_MOTOR, MOTOR_DIR_FORWARD);
    setMotorDirection(RIGHT_MOTOR, MOTOR_DIR_BACKWARD);
    setMotorSpeed(BOTH_MOTORS, fastSpeed);
    if (checkCollision(800))
      break;
    setMotorSpeed(BOTH_MOTORS, lowSpeed);

    /* 360 spin left */
    setMotorDirection(LEFT_MOTOR, MOTOR_DIR_BACKWARD);
    setMotorDirection(RIGHT_MOTOR, MOTOR_DIR_FORWARD);
    setMotorSpeed(BOTH_MOTORS, fastSpeed);
    if (checkCollision(800))
      break;
    setMotorSpeed(BOTH_MOTORS, lowSpeed);

  } 
}
