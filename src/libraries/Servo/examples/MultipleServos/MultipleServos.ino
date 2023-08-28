// Multiple Servos example
// by chris miller <miller4@rose-hulman.edu> 
// Demonstrates instantiation of multiple servos using a single timer.
// This example code is in the public domain.

#include <Servo.h> 

#define SERVO_CNT       8

// a maximum of eight servo objects can be created
Servo myservos[SERVO_CNT]; 
const int servoPins[SERVO_CNT] = {5, 6, 7, 8, 9, 10, 12, 13};

void setup() 
{ 
    int result;

    Serial.begin(57600);
    delay(100);
    // attach servos and test for success
    for(int i = 0; i < SERVO_CNT; i++) {
        result = myservos[i].attach(servoPins[i]);
        if (result == INVALID_SERVO) {
            Serial.print("Servo "); Serial.print(i);
            Serial.println(" failed to attach");
            while (true) { ; }
        }
        myservos[i].write(i*20);
        Serial.print("Servo "); Serial.print(i);
        Serial.print(" on pin "); Serial.print(servoPins[i]);
        Serial.print(" set to angle "); Serial.println(i*20);
        delay(40);
    }
}
 
void loop() 
{ 
} 
