#ifndef GP2Y0A21_Sensor_h
#define GP2Y0A21_Sensor_h

#include "Energia.h"

/// \brief Class for Pololu's Sharp GP2Y0A21YK0F analog distance sensor
///
/// An instance of this class represents a single [Sharp GP2Y0A21YK0F] (https://www.pololu.com/product/136) distance sensor.

class GP2Y0A21_Sensor
{
private:
    uint8_t ir_sensor_pin;
    uint8_t num_pins;
    bool configured;

public:
    GP2Y0A21_Sensor();

    /// \brief Initialize the distance sensor class.
    ///
    /// This function needs to be called before any other function is used.
    ///
    /// \param[in] pin_num on Launchpad connected to distance sensor's analog output.
    /// \param[in] mode for input pin. Options are the same as standard 
    ///  [Energia](https://energia.nu/reference/en/language/functions/digital-io/pinmode/)
    ///  / [Arduino](https://www.arduino.cc/reference/en/language/functions/digital-io/pinmode/) 
    ///  pin mode.
    bool begin(uint8_t pin_num, uint8_t mode = INPUT_PULLDOWN);

    /// \brief Read the value from distance sensor.
    ///
    /// This function returns a value representing the distance an object is
    /// from the sensor.
    ///
    /// \return Return a value of 0 - 4095. The lower the value the closer
    /// an object is.
    uint16_t read();

    /// \brief Read the distance from the Sharp distance sensor in millimeters.
    ///
    /// This function returns a value representing the distance an object is
    /// from the sensor in millimeters. Range is 100 to 800 mm.
    ///
    /// \return the distance in millimeters (-1 if no object detected)
    int16_t readMM();

    /// \brief Read the distance from the Sharp distance sensor in inches.
    ///
    /// This function returns a value representing the distance an object is
    /// from the sensor in inches. Range is ~4 to 31 inches.
    ///
    /// \return the distance in inches (-1 if no object detected)
    float readIN();
};

#endif
