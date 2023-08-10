/* TI-RSLK MAX Example 3 */
/* Robot will activate on board LEDs using bump switches */

#include "Energia.h"

/* Defines bump switch functions of robot */
#include "Bump_Switch.h"

/* Defines pin configuration of robot */
#include "RSLK_MAX_Pins.h"

// TIP: Use the Energia pin maps to find out what capabilities are attached to the LaunchPad
//      The pin maps show the functions on each pin and the pin number and names used in Energia 

/* Initialize bump switch objects */
Bump_Switch      bump_sw0;
Bump_Switch      bump_sw1;
Bump_Switch      bump_sw2;
Bump_Switch      bump_sw3;
Bump_Switch      bump_sw4;
Bump_Switch      bump_sw5;

/* The setup() funtion runs one time at the beginning of the Energia program */
void setup() {
  /* Set serial communication to 115200 baud rate for MSP432 */
  Serial.begin(115200);
  delay(500);

  /* Initialize bump switch object pins as inputs */
  bump_sw0.begin(BP_SW_PIN_0, INPUT_PULLUP);
  bump_sw1.begin(BP_SW_PIN_1, INPUT_PULLUP);
  bump_sw2.begin(BP_SW_PIN_2, INPUT_PULLUP);
  bump_sw3.begin(BP_SW_PIN_3, INPUT_PULLUP);
  bump_sw4.begin(BP_SW_PIN_4, INPUT_PULLUP);
  bump_sw5.begin(BP_SW_PIN_5, INPUT_PULLUP);

  /* Initialize LED pins as outputs */
  pinMode(LED_FR_PIN, OUTPUT); 
  pinMode(LED_FL_PIN, OUTPUT); 
  pinMode(LED_BR_PIN, OUTPUT); 
  pinMode(LED_BL_PIN, OUTPUT);
  pinMode(LP_RED_LED_PIN, OUTPUT);
  pinMode(LP_RGB_LED_RED_PIN, OUTPUT);
  pinMode(LP_RGB_LED_BLUE_PIN, OUTPUT); 
  pinMode(LP_RGB_LED_GREEN_PIN, OUTPUT);

  /* Initialize LaunchPad buttons as inputs */
  pinMode(LP_S1_PIN, INPUT_PULLUP);
  pinMode(LP_S2_PIN, INPUT_PULLUP);

  Serial.println("Testing Bump Switch");
}

/* The loop() function runs after the setup() function completes in an Energia porgram 
   and will continue to run in a repeating loop until the LaunchPad is reset or powered off */
void loop() {
  /* Push one or more bump switch to see an output */

  /* When switch is closed the switch goes to ground */

  /* If outer right switch is pressed, turn on front LEDs */
  if(bump_sw0.read() == 0) {
    Serial.print("Bump switch 0");
    Serial.println(" was pressed");
    digitalWrite(LED_FL_PIN, HIGH);
    digitalWrite(LED_FR_PIN, HIGH);
  }
  
  /* If outer left switch is pressed, turn on back LEDs */
  if (bump_sw5.read() == 0) {
    Serial.print("Bump switch 5");
    Serial.println(" was pressed");
    digitalWrite(LED_BL_PIN, HIGH);
    digitalWrite(LED_BR_PIN, HIGH);
  }
  /* If middle right switch is pressed, turn on back right LED */
  if (bump_sw1.read() == 0) {
    Serial.print("Bump switch 1");
    Serial.println(" was pressed");
    digitalWrite(LED_BR_PIN, HIGH);
  }
  /* If inner right switch is pressed, turn on front right LED */
  if (bump_sw2.read() == 0) {
    Serial.print("Bump switch 2");
    Serial.println(" was pressed");
    digitalWrite(LED_FR_PIN, HIGH);
  }
  /* If inner left switch is pressed, turn on front left LED */
  if (bump_sw3.read() == 0) {
    Serial.print("Bump switch 3");
    Serial.println(" was pressed");
    digitalWrite(LED_FL_PIN, HIGH);
  }
  /* If middle left switch is pressed, turn on back left LED */
  if (bump_sw4.read() == 0) {
    Serial.print("Bump switch 4");
    Serial.println(" was pressed");
    digitalWrite(LED_BL_PIN, HIGH);
  }
  
  /* If LaunchPad buttons are pressed, turn on RGB LEDs */
  if (digitalRead(LP_S1_PIN) == 0) digitalWrite(LP_RGB_LED_RED_PIN, HIGH);
  if (digitalRead(LP_S2_PIN) == 0) digitalWrite(LP_RGB_LED_BLUE_PIN, HIGH);

  delay(100);
  
  /* Turn off LEDs if nothing pressed */
  digitalWrite(LED_FL_PIN, LOW);
  digitalWrite(LED_FR_PIN, LOW);
  digitalWrite(LED_BL_PIN, LOW);
  digitalWrite(LED_BR_PIN, LOW);
  digitalWrite(LP_RGB_LED_RED_PIN, LOW);
  digitalWrite(LP_RGB_LED_BLUE_PIN, LOW);
}
