#include "SimpleRSLK.h"

GP2Y0A21_Sensor dst_sensor[DST_NUM_SENSORS];
Romi_Motor_Power motor[NUM_MOTORS];
Bump_Switch bump_sw[TOTAL_BP_SW];

uint16_t sensorRawValues[LS_NUM_SENSORS];
uint16_t calibrationValues[LS_NUM_SENSORS];
uint16_t sensorMinValues[LS_NUM_SENSORS];
uint16_t sensorMaxValues[LS_NUM_SENSORS];
uint8_t lineMode;

QTRSensors qtr;

void setupRSLK()
{
    motor[LEFT_MOTOR].begin(MOTOR_L_SLP_PIN, MOTOR_L_DIR_PIN, MOTOR_L_PWM_PIN);
    motor[RIGHT_MOTOR].begin(MOTOR_R_SLP_PIN, MOTOR_R_DIR_PIN, MOTOR_R_PWM_PIN);

    setupEncoder(ENCODER_ELA_PIN, ENCODER_ELB_PIN, ENCODER_ERA_PIN, ENCODER_ERB_PIN);

    bump_sw[0].begin(BP_SW_PIN_0, INPUT_PULLUP);
    bump_sw[1].begin(BP_SW_PIN_1, INPUT_PULLUP);
    bump_sw[2].begin(BP_SW_PIN_2, INPUT_PULLUP);
    bump_sw[3].begin(BP_SW_PIN_3, INPUT_PULLUP);
    bump_sw[4].begin(BP_SW_PIN_4, INPUT_PULLUP);
    bump_sw[5].begin(BP_SW_PIN_5, INPUT_PULLUP);

    dst_sensor[0].begin(SHRP_DIST_L_PIN, INPUT_PULLDOWN);
    dst_sensor[1].begin(SHRP_DIST_C_PIN, INPUT_PULLDOWN);
    dst_sensor[2].begin(SHRP_DIST_R_PIN, INPUT_PULLDOWN);

    qtr.setTypeRC();
    qtr.setSensorPins((const uint8_t[]){QTR_7, QTR_6, QTR_5, QTR_4, QTR_3,
                                        QTR_2, QTR_1, QTR_0},
                      LS_NUM_SENSORS);
    qtr.setEmitterPins(QTR_EMITTER_PIN_ODD, QTR_EMITTER_PIN_EVEN);
    disableMotor(BOTH_MOTORS);

    clearMinMax(sensorMinValues, sensorMaxValues);
    lineMode = DARK_LINE;
}

uint16_t readSharpDist(uint8_t num)
{
    if (num < 0 || num >= DST_NUM_SENSORS)
        return 0;

    return dst_sensor[num].read();
}

bool isBumpSwitchPressed(uint8_t num)
{
    if (num < 0 || num >= TOTAL_BP_SW)
        return false;

    if (bump_sw[num].read() == 0) {
        return true;
    } 
    return false;
}

void enableMotor(uint8_t motorNum)
{
    if (motorNum == LEFT_MOTOR || motorNum == BOTH_MOTORS) {
        motor[LEFT_MOTOR].enableMotor();
    }

    if (motorNum == RIGHT_MOTOR || motorNum == BOTH_MOTORS) {
        motor[RIGHT_MOTOR].enableMotor();
    }
}

void disableMotor(uint8_t motorNum)
{
    if (motorNum == LEFT_MOTOR || motorNum == BOTH_MOTORS) {
        motor[LEFT_MOTOR].disableMotor();
    }

    if (motorNum == RIGHT_MOTOR || motorNum == BOTH_MOTORS) {
        motor[RIGHT_MOTOR].disableMotor();
    }
}

void pauseMotor(uint8_t motorNum)
{
    if (motorNum == LEFT_MOTOR || motorNum == BOTH_MOTORS) {
        motor[LEFT_MOTOR].pauseMotor();
    }

    if (motorNum == RIGHT_MOTOR || motorNum == BOTH_MOTORS) {
        motor[RIGHT_MOTOR].pauseMotor();
    }
}

void resumeMotor(uint8_t motorNum)
{
    if (motorNum == LEFT_MOTOR || motorNum == BOTH_MOTORS) {
        motor[LEFT_MOTOR].resumeMotor();
    }

    if (motorNum == RIGHT_MOTOR || motorNum == BOTH_MOTORS) {
        motor[RIGHT_MOTOR].resumeMotor();
    }
}

void setMotorDirection(uint8_t motorNum, uint8_t direction)
{
    if (motorNum == LEFT_MOTOR || motorNum == BOTH_MOTORS) {
        if (direction == 0) {
            motor[LEFT_MOTOR].directionForward();
        } else if (direction == 1) {
            motor[LEFT_MOTOR].directionBackward();
        }
    }

    if (motorNum == RIGHT_MOTOR || motorNum == BOTH_MOTORS) {
        if (direction == 0) {
            motor[RIGHT_MOTOR].directionForward();
        } else if (direction == 1) {
            motor[RIGHT_MOTOR].directionBackward();
        }
    }
}

void setMotorSpeed(uint8_t motorNum, uint8_t speed)
{
    if (motorNum == LEFT_MOTOR || motorNum == BOTH_MOTORS) {
        motor[LEFT_MOTOR].setSpeed(speed);
    }

    if (motorNum == RIGHT_MOTOR || motorNum == BOTH_MOTORS) {
        motor[RIGHT_MOTOR].setSpeed(speed);
    }
}

void setRawMotorSpeed(uint8_t motorNum, uint8_t speed)
{
    if (motorNum == LEFT_MOTOR || motorNum == BOTH_MOTORS) {
        motor[LEFT_MOTOR].setRawSpeed(speed);
    }

    if (motorNum == RIGHT_MOTOR || motorNum == BOTH_MOTORS) {
        motor[RIGHT_MOTOR].setRawSpeed(speed);
    }
}

void readLineSensor(uint16_t *sensorValues)
{
    qtr.read(sensorValues, QTRReadMode::OddEven);
}

void clearMinMax(uint16_t *sensorMin, uint16_t *sensorMax)
{
    for (int x = 0; x < LS_NUM_SENSORS; x++) {
        sensorMin[x] = 9000;
        sensorMax[x] = 0;
    }
}

void calibrateLineSensor(uint8_t mode = DARK_LINE, uint32_t duration = 100)
{
    uint32_t scanTime = millis() + duration;

    lineMode = mode;
    clearMinMax(sensorMinValues, sensorMaxValues);
    do {
        readLineSensor(sensorRawValues);
        setSensorMinMax(sensorRawValues, sensorMinValues, sensorMaxValues);
    } while (millis() < scanTime);
}

void readCalLineSensor(uint16_t *calVal)
{
    readLineSensor(sensorRawValues);
    readCalLineSensor(sensorRawValues, calVal, sensorMinValues, 
                                        sensorMaxValues, lineMode);
}

void readCalLineSensor(uint16_t *sensorValues,
                       uint16_t *calVal,
                       uint16_t *sensorMinVal,
                       uint16_t *sensorMaxVal,
                       uint8_t mode)
{
    for (int x = 0; x < LS_NUM_SENSORS; x++) {
        if (mode)  {
            calVal[x] = 0;
            if (sensorValues[x] < sensorMinVal[x])
                calVal[x] = map(sensorValues[x], 0, sensorMinVal[x], 1000, 0);
        } else {
            calVal[x] = 0;
            if (sensorValues[x] > sensorMaxVal[x])
                calVal[x] = map(sensorValues[x], sensorMaxVal[x], 2500, 0, 1000);
        }
    }
}

uint32_t getLinePosition()
{
    readCalLineSensor(calibrationValues);
    return getLinePosition(calibrationValues, lineMode);
}

uint32_t getLinePosition(uint16_t *calVal, uint8_t mode)
{
    uint32_t avg = 0; // this is for the weighted total
    uint32_t sum = 0; // this is for the denominator, which is <= 64000

    uint32_t _lastPosition;
    for (uint8_t i = 0; i < LS_NUM_SENSORS; i++) {
        uint16_t value = calVal[i];

        // only average in values that are above a noise threshold
        if (value > 50) {
            avg += (uint32_t)value * (i * 1000);
            sum += value;
        }
    }

    _lastPosition = avg / sum;
    return _lastPosition;
}

void setSensorMinMax(uint16_t *sensor, uint16_t *sensorMin, uint16_t *sensorMax)
{
    for (int x = 0; x < LS_NUM_SENSORS; x++) {
        if (sensor[x] < sensorMin[x]) {
            sensorMin[x] = sensor[x];
        }
        if (sensor[x] > sensorMax[x]) {
            sensorMax[x] = sensor[x];
        }
    }
}

void setupWaitBtn(uint8_t btn)
{
    pinMode(btn, INPUT_PULLUP);
}

void setupLed(uint8_t ledPin)
{
    pinMode(ledPin, OUTPUT);
}

void waitBtnPressed(uint8_t btnPin, String msg, int8_t ledPin)
{
    uint8_t btnCnt = 0;
    uint8_t pinVal = HIGH;

    /* Turn on led */
    if (ledPin > 0)
        digitalWrite(ledPin, pinVal);
    while (digitalRead(btnPin) == 1) {
        delay(25);
        btnCnt++;
        if (btnCnt == 40) {
            digitalWrite(ledPin, pinVal);
            btnCnt = 0;
            pinVal = !pinVal;

            if (msg != "")
                Serial.println(msg);
        }
    }

    pinVal = LOW;
    if (ledPin > 0)
        digitalWrite(ledPin, pinVal);

    /* Wait for a short period to avoid button debounce */
    delay(50);
    while (digitalRead(btnPin) == 0)
        ;

    /* Wait for a short period to avoid button debounce */
    delay(50);
}
