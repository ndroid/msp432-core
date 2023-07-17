/*
 *  MinimalReceiver.ino
 *
 *  Small memory footprint and no timer usage!
 *
 *  Receives IR protocol data of NEC protocol using pin change interrupts.
 *  On complete received IR command the function decodeIR(IRData *results) may 
 *  be called to fetch results. Return value indicates if new data is available.
 *  Repeats are included by default unless EXCLUDE_REPEATS is defined. 
 *  
 *  IR input pin must be specified in call to initTinyIRReceiver()
 *      any GPIO pin which supports interrupts may be used
 *
 *
 *  TinyIR is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/gpl.html>.
 *
 */


/*
 * The following optional defines must be defined prior to including TinyIR header
 */
/*
 * Uncomment the following line to change default feedback LED pin
 */
//#define IR_FEEDBACK_LED_PIN    LED1
/*
 * Uncommenting following line saves 12 bytes and reduces ISR handling time
 */
//#define DO_NOT_USE_FEEDBACK_LED 
/*
 * Uncomment the following line to exclude repeats in decodeIR(). 
 *  Otherwise, repeats are included and isRepeat is set. 
 */
//#define EXCLUDE_REPEATS
/*
 * Uncomment the following line in order to define handler for IR event.
 * If enabled, must define 
 *  void handleReceivedTinyIRData(uint16_t aAddress, uint8_t aCommand, bool isRepetition)
 */
//#define HANDLE_IR_EVENT

// Preceding defines (if enabled) must be included prior to including header file
#include <TinyIRremote.h>


/*
 * Helper macro for getting a macro definition as string
 */
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define IR_RCV_PIN      32

/**
 * Struct to hold IR data, defined as:
 * 
 * struct {
 *   decode_type_t protocol;     ///< UNKNOWN, NEC, SONY, RC5, ...
 *   uint16_t address;           ///< Decoded address
 *   uint16_t command;           ///< Decoded command
 *   bool isRepeat;
 * } 
 */
IRData IRresults;

void setup() {
    Serial.begin(57600);
    delay(500); // To be able to connect Serial monitor after reset or power up 
    Serial.println(F("START " __FILE__ " from " __DATE__));
    /*
     * Must be called to initialize and set up IR receiver pin.
     *  bool initTinyIRReceiver(uint8_t aRcvPin, bool aEnableLEDFeedback = false, uint8_t aFeedbackLEDPin = USE_DEFAULT_FEEDBACK_LED_PIN)
     */
    if (initTinyIRReceiver(IR_RCV_PIN, true, GREEN_LED)) {
        Serial.println(F("Ready to receive NEC IR signals at pin " STR(IR_RCV_PIN)));
    } else {
        Serial.println("Initialization of IR receiver failed!");
        while (1) {;}
    }
}

void loop() {
    // decodeIR updates results and returns true if new command is available, 
    //  otherwise results struct is unchanged and returns false
    if (decodeIR(&IRresults)) {
        Serial.print("A=0x");
        Serial.print(IRresults.address, HEX);
        Serial.print(" C=0x");
        Serial.print(IRresults.command, HEX);
        Serial.print(" is repeat: ");
        Serial.print(IRresults.isRepeat ? "TRUE" : "FALSE");
        Serial.println();
    }
}
