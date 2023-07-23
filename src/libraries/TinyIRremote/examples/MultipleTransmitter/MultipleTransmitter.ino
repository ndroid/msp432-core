/*
 *  MultipleTransmitter.ino
 *
 *  Small memory footprint uses Timer PWM output for 38kHz IR signal.
 *
 *  Transmits IR protocol data of NEC protocol using PWM output. Pin configuration 
 *  is toggled from CCRx output to GPIO output for faster toggle between marks and 
 *  spaces. Up to four IR transmit objects may be declared as type IRsender, but
 *  each must use the same timer (determined by first sender initialized). Each 
 *  IRsender object must be initialized with call to: 
 * 
 *      bool IRsend::initIRSender()
 *  
 *
 *  May declare up to four transmitters which use a mappable PWM timer (TA0/TA1) 
 *    pin, or up to four transmitters which use timer TA2 pins.
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
 *    * * * * *  LED pins  * * * * * * * *
 *      75          P2.0        mappable (RED_LED)
 *      76          P2.1        mappable (GREEN_LED)
 *      77          P2.2        mappable (BLUE_LED)
 * 
 *    * * *  MSP432P401R only  * * * * * *
 *      47          P7.3        mappable
 *      48          P7.1        mappable
 *      52          P7.4        mappable
 *      53          P7.6        mappable
 *      64          P7.2        mappable
 *      65          P7.0        mappable
 *      68          P7.5        mappable
 *      69          P7.7        mappable
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

#define IR_TRX_PIN1      4
#define IR_TRX_PIN2      19

IRsender sendIR1(IR_TRX_PIN1);
IRsender sendIR2(IR_TRX_PIN2);

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
IRData IRmsg1, IRmsg2;

void setup() {
    Serial.begin(57600);
    delay(500); // To be able to connect Serial monitor after reset or power up 
    Serial.println(F("START " __FILE__ " from " __DATE__));
    /*
     * Must be called to initialize and set up IR transmit pin.
     *  bool IRsender::initIRSender( )
     */
    if (sendIR1.initIRSender()) {
        Serial.println(F("Ready to transmit NEC IR signals on pin " STR(IR_TRX_PIN1)));
    } else {
        Serial.println("Initialization of IR transmitter failed!");
        while (1) {;}
    }
    if (sendIR2.initIRSender()) {
        Serial.println(F("Ready to transmit NEC IR signals on pin " STR(IR_TRX_PIN2)));
    } else {
        Serial.println("Initialization of IR transmitter failed!");
        while (1) {;}
    }
    delay(200);
    // enable transmit feedback and specify LED pin number (defaults to LED_BUILTIN)
    enableTXLEDFeedback(GREEN_LED);

    IRmsg1.protocol = NEC;
    IRmsg1.address = 0xAA;
    IRmsg1.command = 0xC3;
    IRmsg1.isRepeat = false;

    IRmsg2.protocol = NEC;
    IRmsg2.address = 0x55;
    IRmsg2.command = 0x4D;
    IRmsg2.isRepeat = false;
}

void loop() {
    // send data specified in IRmsg struct using NEC encoding 
    //  size_t IRsender::write(IRData *aIRSendData, uint8_t aNumberOfRepeats = NO_REPEATS)
    sendIR1.write(&IRmsg1);
    Serial.print('.');
    delay(1000);

    sendIR2.write(&IRmsg2, 2);
    Serial.print('.');
    delay(1000);
}
