# TinyIRremote
https://github.com/ndroid/TinyIRremote

Adaptation of TinyIRReceiver from Arduino IRRemote library <a href="https://github.com/Arduino-IRremote/">https://github.com/Arduino-IRremote/</a>
Modified to work with MSP432 boards in Energia and add IR transmit.


*Define IR_FEEDBACK_LED_PIN to change default feedback LED pin

*Define DO_NOT_USE_FEEDBACK_LED to save 12 bytes and reduce ISR handling time

*By default, repeats are included and decodeIR() returns all commands.
Define EXCLUDE_REPEATS to ignore repeats in decodeIR() and return only new commands. 

*Define HANDLE_IR_EVENT in order to provide user-defined handler for IR event.
If enabled, must define <br>
<code>  void handleReceivedTinyIRData(uint16_t aAddress, uint8_t aCommand, bool isRepetition)</code><br>
This function is called if a complete command was received, regardless if it is 
a repeat (not impacted by EXCLUDE_REPEATS constant).<br>
!!!!!!!!!!!!!!!!!!!!!!<br>
  Function called in interrupt context - should be running as short as possible, so if you 
  require longer action, save the data (address + command) and handle it in the main loop.<br>
!!!!!!!!!!!!!!!!!!!!!

*For IR receiver, IR input pin must be specified in call to initTinyIRReceiver() - returns true on success.
Any GPIO pin which supports interrupts may be used.<br>
<code>  bool initTinyIRReceiver(uint8_t aRcvPin, bool aEnableLEDFeedback = false, uint8_t aFeedbackLEDPin = USE_DEFAULT_FEEDBACK_LED_PIN)</code>

*For IR transmitter, declare an IRsend object and initialize with <br>
<code>  bool IRsend::begin(uint8_t aSendPin, bool aEnableLEDFeedback = false, uint8_t aFeedbackLEDPin = USE_DEFAULT_FEEDBACK_LED_PIN)</code><br>
  Supported send pins include:
<pre>     pin#        port.pin     timer
      3           P3.2        mappable
      4           P3.3        mappable
      11          P3.6        mappable
      17          P5.7        TA2.2
      18          P3.0        mappable
      19          P2.5        TA0.2/mappable
      31          P3.7        mappable
      32          P3.5        mappable
      34          P2.3        TA1.1/mappable
      35          P6.7        TA2.4
      36          P6.6        TA2.3
      37          P5.6        TA2.1
      38          P2.4        TA0.1/mappable
      39          P2.6        TA0.3/mappable
      40          P2.7        TA0.4/mappable
</pre>

*Both IR receive and transmit use IRData struct, defined as
<pre><code>  struct {
    decode_type_t protocol;     ///< UNKNOWN, NEC, SONY, RC5, ...
    uint16_t address;           ///< Decoded address
    uint16_t command;           ///< Decoded command
    bool isRepeat;
  } 
</code></pre>
