#include "GP2Y0A21_Sensor.h"

/* minimum threshold for object detection using 14-bit ADC resolution */
#define MIN_THRESHOLD       1000

GP2Y0A21_Sensor::GP2Y0A21_Sensor()
{
    ir_sensor_pin = 0;
}

bool GP2Y0A21_Sensor::begin(uint8_t pin_num, uint8_t mode)
{
    ir_sensor_pin = pin_num;
    pinMode(ir_sensor_pin, mode);
    return true;
}

uint16_t GP2Y0A21_Sensor::read()
{
    return analogRead(ir_sensor_pin);
}

int16_t GP2Y0A21_Sensor::readMM()
{
    int16_t analogVal;

    analogVal = analogRead(ir_sensor_pin);
    if (analogVal < MIN_THRESHOLD)
        return -1;
    // Equation comes from https://github.com/Resaj/GP2D-GP2Y-function 
    // GP2Y0A21 (5V sensor supply & 3,3V ADC): distance = 31 * ((3000 / (adc_value + 1) - 0,8))
    //  adapted to 14-bit resolution:
    //    31 * 3000 * 16 = 1488000
    //    31 * 0.8 = 24.8 
    return ((long)1488000 / (analogVal + 16)) - 25;
}

float GP2Y0A21_Sensor::readIN()
{
    int distanceMM = readMM();
    if (distanceMM < 0)
        return -1;
    return (float)distanceMM / 25.4;
}
