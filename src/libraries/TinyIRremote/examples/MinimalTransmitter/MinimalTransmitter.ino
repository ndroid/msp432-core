/*
 *  MinimalTransmitter.ino
 *
 *  Small memory footprint uses Timer PWM output for 38kHz IR signal.
 *
 *  Transmits IR protocol data of NEC protocol using PWM output. Pin configuration 
 *  is toggled from CCRx output to GPIO output for faster toggle between marks and 
 *  spaces. Multiple IR transmit objects may be declared as type IRsend. Each 
 *  IRsend object must be initialized with call to 
 * 
 *  bool IRsend::begin(uint8_t aSendPin, bool aEnableLEDFeedback, uint8_t aLEDFeedbackPin)
 *  
 *
 *  Supported send pins include:
 *     pin#        port.pin     timer
 *      3           P3.2        mappable
 *      4           P3.3        mappable
 *      11          P3.6        mappable
 *      17          P5.7        TA2.2
 *      18          P3.0        mappable
 *      19          P2.5        TA0.2/mappable
 *      31          P3.7        mappable
 *      32          P3.5        mappable
 *      34          P2.3        TA1.1/mappable
 *      35          P6.7        TA2.4
 *      36          P6.6        TA2.3
 *      37          P5.6        TA2.1
 *      38          P2.4        TA0.1/mappable
 *      39          P2.6        TA0.3/mappable
 *      40          P2.7        TA0.4/mappable
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

// Preceding defines (if enabled) must be included prior to including header file
#include <TinyIRremote.h>


/*
 * Helper macro for getting a macro definition as string
 */
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define IR_TRX_PIN      32

IRsend sendIR;

/**
 * Struct for IR data to transmit, defined as:
 * 
 * struct {
 *   decode_type_t protocol;     ///< UNKNOWN, NEC, SONY, RC5, ...
 *   uint16_t address;           ///< Decoded address
 *   uint16_t command;           ///< Decoded command
 *   bool isRepeat;
 * } 
 */
IRData IRmsg;

void setup() {
    Serial.begin(57600);
    delay(500); // To be able to connect Serial monitor after reset or power up 
    Serial.println(F("START " __FILE__ " from " __DATE__));
    /*
     * Must be called to initialize and set up IR transmit pin.
     *  bool IRsend::begin(uint8_t aSendPin, bool aEnableLEDFeedback = false, uint8_t aFeedbackLEDPin = USE_DEFAULT_FEEDBACK_LED_PIN)
     */
    if (sendIR.begin(IR_TRX_PIN, true, GREEN_LED)) {
        Serial.println(F("Ready to transmit NEC IR signals on pin " STR(IR_TRX_PIN)));
    } else {
        Serial.println("Initialization of IR transmitter failed!");
        while (1) {;}
    }
    delay(200);

    IRmsg.protocol = NEC;
    IRmsg.address = 0xA5;
    IRmsg.command = 0xC3;
    IRmsg.isRepeat = false;
}

void loop() {
    // send data specified in IRmsg struct using NEC encoding 
    //  size_t IRsend::write(IRData *aIRSendData, uint8_t aNumberOfRepeats = NO_REPEATS)
    sendIR.write(&IRmsg);
    Serial.print('.');
    delay(500);
}
