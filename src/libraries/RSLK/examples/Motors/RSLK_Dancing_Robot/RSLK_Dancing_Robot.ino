#include "Energia.h"

#include "SimpleRSLK.h"

bool hit_obstacle = false;

void waitBtnPressed() {
  while(digitalRead(LP_S2_PIN) == 1){
    digitalWrite(LP_RGB_LED_GREEN_PIN, HIGH);
    delay(500);
    digitalWrite(LP_RGB_LED_GREEN_PIN, LOW);
    delay(500);
  }
}

void checkCollision() {
  for(int x = 0;x<6;x++)
  {
    /* Check if bump switch was pressed
     *  Parameter:
     *    bump switch number -> 0-5
     *    Returns:
     *      true -> if specific switch was pressed
     *      false -> if specific switch was not pressed
     */
    if(isBumpSwitchPressed(x) == true) {
      hit_obstacle = true;
      Serial.println("Collision detected");
      disableMotor(BOTH_MOTORS);
      break;
    }
  }
}

void setup() {
  /* Set serial communication to 115200 baud rate for MSP432 */
  Serial.begin(115200);
  delay(500);

  /* Run setup code */
  setupRSLK();

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
}

void loop() {
  
  Serial.println("Waiting until left button is pushed");
  /* Wait until button is pressed to start robot */
  waitBtnPressed();

  /* Wait two seconds before starting */
  delay(2000);
  digitalWrite(LP_RGB_LED_BLUE_PIN, HIGH);
 /* Enables specified motor.
  *  Parameter:
  *   Motor your referencing -> LEFT_MOTOR  RIGHT_MOTOR  BOTH_MOTORS
  */
  enableMotor(BOTH_MOTORS);

 /* Set direction of motor rotation.
  *  Parameter:
  *   Motor your referencing -> LEFT_MOTOR  RIGHT_MOTOR  BOTH_MOTORS
  *   Direction -> MOTOR_DIR_FORWARD  MOTOR_DIR_BACKWARD
  */
  setMotorDirection(LEFT_MOTOR,MOTOR_DIR_FORWARD);
  setMotorDirection(RIGHT_MOTOR,MOTOR_DIR_FORWARD);
  
 /* Set speed of motor.
  *  Parameter:
  *   Motor your referencing -> LEFT_MOTOR  RIGHT_MOTOR  BOTH_MOTORS
  *   Speed -> 0 - 100
  */
  setMotorSpeed(BOTH_MOTORS,10);

  while(!hit_obstacle) {
    /* Move robot in place */
    
    /* Right turn in place */
    setMotorDirection(LEFT_MOTOR, MOTOR_DIR_BACKWARD);
    setMotorDirection(RIGHT_MOTOR, MOTOR_DIR_FORWARD);
    delay(1000);

    /* Left turn in place */
    setMotorDirection(LEFT_MOTOR, MOTOR_DIR_FORWARD);
    setMotorDirection(RIGHT_MOTOR, MOTOR_DIR_BACKWARD);
    delay(1000);

    
    /* 360 spin right */
    setMotorDirection(LEFT_MOTOR, MOTOR_DIR_FORWARD);
    setMotorDirection(RIGHT_MOTOR, MOTOR_DIR_BACKWARD);
    setMotorSpeed(BOTH_MOTORS,50);
    delay(800);
    setMotorSpeed(BOTH_MOTORS,10);

    /* 360 spin left */
    setMotorDirection(LEFT_MOTOR, MOTOR_DIR_BACKWARD);
    setMotorDirection(RIGHT_MOTOR, MOTOR_DIR_FORWARD);
    setMotorSpeed(BOTH_MOTORS,50);
    delay(800);
    setMotorSpeed(BOTH_MOTORS,10);

    /* Left turn in place */
    setMotorDirection(LEFT_MOTOR, MOTOR_DIR_BACKWARD);
    setMotorDirection(RIGHT_MOTOR, MOTOR_DIR_FORWARD);
    delay(500);

    /* Right turn in place */
    setMotorDirection(LEFT_MOTOR, MOTOR_DIR_FORWARD);
    setMotorDirection(RIGHT_MOTOR, MOTOR_DIR_BACKWARD);
    delay(1000);

    /* Left turn in place */
    setMotorDirection(LEFT_MOTOR, MOTOR_DIR_BACKWARD);
    setMotorDirection(RIGHT_MOTOR, MOTOR_DIR_FORWARD);
    delay(1000);

    /* 360 spin right */
    setMotorDirection(LEFT_MOTOR, MOTOR_DIR_FORWARD);
    setMotorDirection(RIGHT_MOTOR, MOTOR_DIR_BACKWARD);
    setMotorSpeed(BOTH_MOTORS,50);
    delay(800);
    setMotorSpeed(BOTH_MOTORS,10);

    /* 360 spin left */
    setMotorDirection(LEFT_MOTOR, MOTOR_DIR_BACKWARD);
    setMotorDirection(RIGHT_MOTOR, MOTOR_DIR_FORWARD);
    setMotorSpeed(BOTH_MOTORS,50);
    delay(800);
    setMotorSpeed(BOTH_MOTORS,10);

  } 
}
