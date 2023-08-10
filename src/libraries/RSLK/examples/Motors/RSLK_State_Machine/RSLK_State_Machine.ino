#include "Energia.h"

#include "SimpleRSLK.h"

/* Defines struct for state machine states */
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

/* Variable that will take the state machine to the STOP state */
bool done;

/* Initialize objects */ 


void setup() {

    /* Set serial communication to 115200 baud rate for MSP432 */
    Serial.begin(115200);
    delay(500);
    Serial.println("Initializing.....");

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

    Serial.println("Initializing System Complete.");
}

void loop() {

  // Emergency stop switch S2
  // Switch to state "STOP" if pressed
  if (digitalRead(LP_S2_PIN) == 0) state = STOP;

  //-----------------------------------
  //        Main State Machine
  //-----------------------------------
  switch (state) {

    case START:
            Serial.println("Enter START state");
            state = WAIT;
    break;

    case WAIT:
        Serial.println("Enter WAIT state");
        digitalWrite(LP_RGB_LED_GREEN_PIN, HIGH);
        delay(200);
        digitalWrite(LP_RGB_LED_GREEN_PIN, LOW);
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
         *   Motor your referencing -> LEFT_MOTOR  RIGHT_MOTOR  BOTH_MOTORS
         */
        enableMotor(BOTH_MOTORS);
        setMotorDirection(BOTH_MOTORS, MOTOR_DIR_FORWARD);
        setMotorSpeed(BOTH_MOTORS, 25);
        
        state = GO2;
    break;

    case GO2:
        Serial.println("Enter GO2 state");
        /* Detect a bump and then switch to bump correction state */
        for(int x = 0;x<6;x++)
        {
            if(isBumpSwitchPressed(x) == true) state = BUMPED1a;
        }

        /* Continue to rotate until done condition is met */
        /* Certain distance traveled or other conditions can be set */
        if (getEncoderLeftCnt() > 50000) {
          done = 1; 
        }

        if (done) state = STOP;
    break;

    case BUMPED1a:
        Serial.println("Enter BUMPED1a state");
        /* Stop the motors */
        setMotorSpeed(BOTH_MOTORS,0);
        /* Reverse the robot */
        setMotorDirection(BOTH_MOTORS, MOTOR_DIR_BACKWARD);
        setMotorSpeed(BOTH_MOTORS,25);
        delay(500);
        state = BUMPED1b;
    break;

    case BUMPED1b:
        Serial.println("Enter BUMPED1b state");
        /* Turn robot to avoid obstacle */

        setMotorSpeed(LEFT_MOTOR,0);
        setMotorSpeed(RIGHT_MOTOR,25);
        delay(100);

        state = DRIVE;
    break;

    case DRIVE:
        Serial.println("Enter DRIVE state");
        /* Put motors back to forward direction */
        setMotorDirection(BOTH_MOTORS, MOTOR_DIR_FORWARD);
        setMotorSpeed(BOTH_MOTORS, 25);

        state = GO2;
    break;

    case STOP:
        Serial.println("Enter STOP state");
        Serial.println("Press Reset to begin again");
        /* Stop all motors */
        disableMotor(BOTH_MOTORS);
    break;
  }

  delay(10);
}
