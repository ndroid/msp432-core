# TinyIRremote
https://github.com/ndroid/TinyIRremote

Adaptation of TinyIRReceiver from Arduino IRRemote library <a href="https://github.com/Arduino-IRremote/">https://github.com/Arduino-IRremote/</a>. Modified to work with MSP432 boards in Energia and add IR transmit.

* IR receiver objects can be declared using the IRreceiver Class. Avoids reserving timers by using pin change interrupts to decode IR signals. Any GPIO pin which supports interrupts may be used.
    * Multiple IRreceiver objects may be declared, however performance may degrade with more receivers.
    * By default, repeats are included and decodeIR() returns all commands. Change default setting in call to initIRReceiver() to ignore repeats in decodeIR() and return only new commands. 
    * A custom user-defined callback function may be specified to handle all received IR commands. If enabled, user must define handler function as type <br>
        ```
        /*
         *  @brief  IRData callback function type
         *
         *  @param  address     Address value from received IR command
         *  @param  command     Commmand value from received IR command
         *  @param  isRepeat    True if the last received IR command is a repeat
         */
        void (*IRData_CallbackFxn)(uint16_t address, uint8_t command, bool isRepeat)
        ```
        * This function is called if a complete command was received, regardless if it is a repeat (not impacted by includeRepeats setting).<br>
!!!!!!!!!!!!!!!!!!!!!!<br>
  Function called in interrupt context - should be running as short as possible, so if you 
  require longer action, save the data (address + command) and handle it in the main loop.<br>
!!!!!!!!!!!!!!!!!!!!!


* For IR transmitter, up to four IRsender objects may be declared and initialized. All four pins must use the same timer, however. Ports 2, 3, and 7 are mappable and may be connected to Timer_A0 or Timer_A1. There are four pins which are mapped to Timer_A2. <br>
  Supported send pins include:
    <pre>      pin#        port.pin     timer
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
        * * * * *  LED pins  * * * * * * * *
          75          P2.0        mappable (RED_LED)
          76          P2.1        mappable (GREEN_LED)
          77          P2.2        mappable (BLUE_LED)
    
        * * *  MSP432P401R only  * * * * * *
          47          P7.3        mappable
          48          P7.1        mappable
          52          P7.4        mappable
          53          P7.6        mappable
          64          P7.2        mappable
          65          P7.0        mappable
          68          P7.5        mappable
          69          P7.7        mappable
    </pre>

* Both IR receive and transmit use IRData struct, defined as
    ```
    struct {
        decode_type_t protocol;     ///< UNKNOWN, NEC, SONY, RC5, ...
        uint16_t address;           ///< Decoded address
        uint16_t command;           ///< Decoded command
        bool isRepeat;
    } 
    ```

## Public methods

### class IRreceiver

* 
        /**
         * Default (only) constructor. Must call initIRReceiver() and verify success
         *  before using object.
         *  
         *  @param receivePin   The Arduino pin number, where an IR receiver is connected.
         */
        IRreceiver::IRreceiver(uint8_t receivePin)

        /**
         * Initializes hardware interrupt generation for receiver pin. If callback
         * is enabled, every new IR command received will result in a callback to 
         * user defined function. Callback function must be defined as:
         *  void handleReceivedTinyIRData(uint16_t aAddress, uint8_t aCommand, bool isRepeat);
         * 
         *  @param includeRepeats       if false, only new IR commands will be reported
         *  @param enableCallback       enables callback to user defined function
         *  @param callbackFunction     reference to callback function handler
         * 
         * @return true if successful (should always succeed for valid GPIO pin)
         */
        bool IRreceiver::initIRReceiver(bool includeRepeats = true, bool enableCallback = false,
                    void (*callbackFunction)(uint16_t , uint8_t , bool) = NULL)

        /**
         * Check for new IR command received. Will never return true if user
         *  callback function is enabled.
         * 
         * @return true if new command available. Future calls will return FALSE 
         *  until new IR command received.
         */
        bool IRreceiver::newCommandReceived()

        /**
         * Fill IRData struct if new command has been received. Will never return 
         *  true if user callback function is enabled. Calls to newCommandReceived() 
         *  will return FALSE after calling this method.
         * 
         * @param results Reference to IRData struct to be populated with new command data.
         * 
         * @return true if new command available (results populated with new command).
         */
        bool IRreceiver::decodeIR(IRData *results)

        /**
         * Get address value of last received IR command.
         * 
         * @return address value of last received IR command
         */
        uint16_t IRreceiver::getAddress()

        /**
         * Get command value of last received IR command.
         * 
         * @return command value of last received IR command
         */
        uint16_t IRreceiver::getCommand()

        /**
         * Get repeat value of last received IR command.
         * 
         * @return true if last received IR command is a repeat
         */
        bool IRreceiver::getIsRepeat()

### class IRsender

* 
        /**
         * Default (only) constructor. Must call initIRSender() and verify success
         *  before using object.
         * 
         * @param sendPin   The Arduino pin number, where an IR LED is connected.
         */
        IRsender::IRsender(uint8_t sendPin);

        /**
         * Initializes hardware for PWM output on sender pin. 
         * 
         * @return true if configuration of pin for PWM output succeeds
         */
        bool IRsender::initIRSender();

        /**
         * Send IR command defined by irSendData. Assumes NEC protocol.
         * 
         * @param irSendData The values of protocol, address, command and repeat flag are taken for sending.
         * @param numberOfRepeats Number of repeats to send after the initial data.
         * 
         * @return 1 if data transmit succeeds
         */
        size_t IRsender::write(IRData *irSendData, uint8_t numberOfRepeats = NO_REPEATS);

### IR feedback LED methods

* 
        /**
         * Enable blinking of feedback LED (LED_BUILTIN is taken as default) on IR receive. 
         *  If already enabled, LED pin setting will be overwritten to feedbackLEDPin.
         * 
         * @param feedbackLEDPin       pin number of LED for receive feedback
         */
        void enableRXLEDFeedback(uint8_t feedbackLEDPin = USE_DEFAULT_FEEDBACK_LED_PIN);

        /**
         * Disable blinking of feedback LED for IR receiving.
         */
        void disableRXLEDFeedback(uint8_t aFeedbackLEDPin);

        /**
         * Enable blinking of feedback LED (LED_BUILTIN is taken as default) on IR sending. 
         *  If already enabled, LED pin setting will be overwritten to feedbackLEDPin.
         * 
         * @param feedbackLEDPin       pin number of LED for transmit feedback
         */
        void enableTXLEDFeedback(uint8_t feedbackLEDPin = USE_DEFAULT_FEEDBACK_LED_PIN);

        /**
         * Disable blinking of feedback LED for IR sending.
         */
        void disableTXLEDFeedback();

