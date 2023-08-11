/*
 * Energia Robot Library for Texas Instruments' Robot System Learning Kit (RSLK)
 * Bump Switch w/ LEDs Example
 *
 * Summary:
 * This example has the TI Robotic System Learning Kit (TI RSLK) activate 
 * on board LEDs using bump switches.
 *
 * How to run:
 *  Press any bump switch and observe LED states to determine correct detection:
 *      Outer right switch          Front right LED
 *      Middle right switch         Right LEDs
 *      Inner right switch          Back right LED
 *      Inner left switch           Back left LED
 *      Middle left switch          Left LEDs
 *      Outer left switch           Front left LED
 *
 * Learn more about the classes, variables and functions used in this library by going to:
 * https://ndroid.github.io/msp432-core/RSLK/
 *
 * Learn more about the TI RSLK by going to http://www.ti.com/rslk
 *
 * created by chris miller 
 * adapted from example by Franklin Cooper Jr.
 * This example code is in the public domain.
 */

/* Include RSLK library */
#include "SimpleRSLK.h"

// TIP: Use the Energia pin maps to find out what capabilities are attached to the LaunchPad
//      The pin maps show the functions on each pin and the pin number and names used in Energia 

/* The setup() funtion runs one time at the beginning of the Energia program */
void setup() {
  /* Set serial communication to 115200 baud rate for MSP432 */
  Serial.begin(115200);
  delay(500);
  setupRSLK();

  /* Initialize LED pins as outputs */
  pinMode(LED_FR_PIN, OUTPUT); 
  pinMode(LED_FL_PIN, OUTPUT); 
  pinMode(LED_BR_PIN, OUTPUT); 
  pinMode(LED_BL_PIN, OUTPUT);

  Serial.println("Testing Bump Switch");
}

/* The loop() function runs after the setup() function completes in an Energia porgram 
   and will continue to run in a repeating loop until the LaunchPad is reset or powered off */
void loop() {
  /* Push one or more bump switch to see an output */

  /* When switch is closed the switch goes to ground */

  /* If outer right switch is pressed, turn on front right LED */
  if(isBumpSwitchPressed(5)) {
    Serial.println("Bump switch 5 (outer right) was pressed");
    digitalWrite(LED_FR_PIN, HIGH);
  }
  
  /* If middle right switch is pressed, turn on right LEDs */
  if(isBumpSwitchPressed(4)) {
    Serial.println("Bump switch 4 (middle right) was pressed");
    digitalWrite(LED_BR_PIN, HIGH);
    digitalWrite(LED_FR_PIN, HIGH);
  }
  /* If inner right switch is pressed, turn on back right LED */
  if(isBumpSwitchPressed(3)) {
    Serial.println("Bump switch 3 (inner right) was pressed");
    digitalWrite(LED_BR_PIN, HIGH);
  }

  /* If inner left switch is pressed, turn on back left LED */
  if(isBumpSwitchPressed(2)) {
    Serial.println("Bump switch 2 (inner left) was pressed");
    digitalWrite(LED_BL_PIN, HIGH);
  }

  /* If middle left switch is pressed, turn on left LEDs */
  if(isBumpSwitchPressed(1)) {
    Serial.println("Bump switch 1 (middle left) was pressed");
    digitalWrite(LED_BL_PIN, HIGH);
    digitalWrite(LED_FL_PIN, HIGH);
  }
  
  /* If outer left switch is pressed, turn on front left LED */
  if(isBumpSwitchPressed(0)) {
    Serial.println("Bump switch 0 (outer left) was pressed");
    digitalWrite(LED_FL_PIN, HIGH);
  }

  delay(100);
  
  /* Turn off LEDs if nothing pressed */
  digitalWrite(LED_FL_PIN, LOW);
  digitalWrite(LED_FR_PIN, LOW);
  digitalWrite(LED_BL_PIN, LOW);
  digitalWrite(LED_BR_PIN, LOW);
}
