/*
 *  MinimalReceiver.ino
 *
 *  Small memory footprint and no timer usage!
 *
 *  Receives IR protocol data of NEC protocol using pin change interrupts.
 *  On complete received IR command the function decodeIR(IRData *results) may 
 *  be called to fetch results. Return value indicates if new data is available.
 *  Repeats are included by default, but may be disabled in call to init. 
 *  
 *  Multiple receiver objects may be specified with the Class IRreceiver.
 *  IR input pin must be specified in constructor.
 *
 *
 *  TinyIRremote is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/gpl.html>.
 *
 */

#include <TinyIRremote.h>

/*
 * Helper macro for getting a macro definition as string
 */
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define IR_RCV_PIN      33

IRreceiver irRX(IR_RCV_PIN);

/**
 * Struct to hold IR data, defined as (defined in IRData.h):
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
     *  bool initIRReceiver(bool includeRepeats = true, bool enableCallback = false,
                void (*callbackFunction)(uint16_t , uint8_t , bool) = NULL)
     */
    if (irRX.initIRReceiver()) {
        Serial.println(F("Ready to receive NEC IR signals at pin " STR(IR_RCV_PIN)));
    } else {
        Serial.println("Initialization of IR receiver failed!");
        while (1) {;}
    }
    // enable receive feedback and specify LED pin number (defaults to LED_BUILTIN)
    enableRXLEDFeedback(BLUE_LED);
}

void loop() {
    // decodeIR updates results and returns true if new command is available, 
    //  otherwise results struct is unchanged and returns false
    if (irRX.decodeIR(&IRresults)) {
        Serial.print("A=0x");
        Serial.print(IRresults.address, HEX);
        Serial.print(" C=0x");
        Serial.print(IRresults.command, HEX);
        Serial.print(" is repeat: ");
        Serial.print(IRresults.isRepeat ? "TRUE" : "FALSE");
        Serial.println();
    }
}
