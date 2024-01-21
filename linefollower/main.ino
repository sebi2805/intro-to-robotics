#include <QTRSensors.h>
#include <EEPROM.h>

// Motor Pin Configuration
const int m11Pin = 7;    // Motor 1 pin 1
const int m12Pin = 6;    // Motor 1 pin 2
const int m21Pin = 5;    // Motor 2 pin 1
const int m22Pin = 4;    // Motor 2 pin 2
const int m1Enable = 11; // Motor 1 enable pin
const int m2Enable = 10; // Motor 2 enable pin

// Movement and Sensor Parameters
const int maxNumberOfMoves = 10;        // Maximum number of moves
const int sensorCalibrationBound = 700; // Sensor calibration boundary

// PID Controller Variables
int m1Speed = 0;   // Speed of motor 1
int m2Speed = 0;   // Speed of motor 2
float kp = 6.5;    // Proportional gain
float ki = 0.0001; // Integral gain
float kd = 1.5;    // Derivative gain
int p = 1;         // Proportional error
int i = 0;         // Integral error
int d = 0;         // Derivative error
int error = 0;     // Current error
int lastError = 0; // Last error

// Motor Speed Limits
const int maxSpeed = 255;  // Maximum speed
const int minSpeed = -255; // Minimum speed
// const int minSpeed = -200; // Alternative minimum speed

// Base Speed Constant
const int baseSpeed = 255; // Base speed for the motors

// Sensor Initialization
QTRSensors qtr;                                // QTR sensor object
const int sensorCount = 6;                     // Number of sensors
int sensorValues[sensorCount];                 // Array to hold sensor readings
int sensors[sensorCount] = {0, 0, 0, 0, 0, 0}; // Initial sensor values

void calibrateSensors()
{
    digitalWrite(LED_BUILTIN, HIGH);

    int calibrationSpeed = 200;
    bool movingLeft = true;
    int calibrationMovesCount = 0;

    while (calibrationMovesCount < maxNumberOfMoves)
    {
        qtr.calibrate();
        qtr.read(sensorValues);

        int sensorIndex = movingLeft ? 0 : 5;

        if (sensorValues[sensorIndex] < sensorCalibrationBound)
        {
            movingLeft = !movingLeft;
            calibrationMovesCount++;
        }

        int leftMotorSpeed = movingLeft ? calibrationSpeed : -calibrationSpeed;
        int rightMotorSpeed = -leftMotorSpeed;
        setMotorSpeed(leftMotorSpeed, rightMotorSpeed);
    }
    for (int i = 0; i < sensorCount; i++)
    {
        EEPROM.put(i * sizeof(int), qtr.calibratedMinimumOn[i]);
        EEPROM.put((i + sensorCount) * sizeof(int), qtr.calibratedMaximumOn[i]);
    }

    digitalWrite(LED_BUILTIN, LOW);
}

void setup()
{
    // Initialize motor control pins
    pinMode(motor1Pin1, OUTPUT);
    pinMode(motor1Pin2, OUTPUT);
    pinMode(motor2Pin1, OUTPUT);
    pinMode(motor2Pin2, OUTPUT);
    pinMode(motor1Enable, OUTPUT);
    pinMode(motor2Enable, OUTPUT);

    // Configure QTR sensor settings
    qtr.setTypeAnalog();
    // we did have the sensor actually in reverse so we had to reverse the pins
    qtr.setSensorPins((const uint8_t[]){A5, A4, A3, A2, A1, A0}, sensorCount);
    delay(10); // Short delay to ensure sensor configuration is stable

    pinMode(LED_BUILTIN, OUTPUT);
    calibrateSensors();
    //     Serial.begin(9600);
}

void calculateSpeed()
{
    int error = map(qtr.readLineBlack(sensorValues), 0, 5000, -50, 50);
    p = error;
    i = i + error;
    d = error - lastError;
    lastError = error;
    int motorSpeed = pidControl(kp, ki, kd, p, i, d);

    m1Speed = baseSpeed;
    m2Speed = baseSpeed;
    if (error < 0)
    {
        m1Speed += motorSpeed;
    }
    else if (error > 0)
    {
        m2Speed -= motorSpeed;
    }

    if (error > 35 || error < -35)
    {
        m1Speed = constrain(m1Speed, minSpeed, maxSpeed);
        m2Speed = constrain(m2Speed, minSpeed, maxSpeed);
    }
    else
    {
        m1Speed = constrain(m1Speed, 0, maxSpeed);
        m2Speed = constrain(m2Speed, 0, maxSpeed);
    }
}

bool isCalibrated = false;
void loop()
{
    // EEPROM.get(0, isCalibrated);

    // if (isCalibrated)
    // {
    //     // Load calibration data from EEPROM
    //     for (int i = 0; i < sensorCount; i++)
    //     {
    //         EEPROM.get(i * sizeof(int), qtr.calibratedMinimumOn[i]);
    //         EEPROM.get((i + sensorCount) * sizeof(int), qtr.calibratedMaximumOn[i]);
    //     }
    // }
    // else
    // {
    //     // Perform calibration
    //     calibrateSensors();

    //     // Set the flag to indicate that calibration data is now stored
    //     isCalibrated = true;
    //     EEPROM.put(0, isCalibrated);
    // }
    calculateSpeed();
    setMotorSpeed(m1Speed, m2Speed);
}
int pidControl(float kp, float ki, float kd, int p, int i, int d)
{
    return kp * p + ki * i + kd * d;
}
void setMotorSpeed(int motor1Speed, int motor2Speed)
{
    if (motor1Speed == 0)
    {
        digitalWrite(m11Pin, LOW);
        digitalWrite(m12Pin, LOW);
        analogWrite(m1Enable, motor1Speed);
    }
    else
    {
        if (motor1Speed > 0)
        {
            digitalWrite(m11Pin, HIGH);
            digitalWrite(m12Pin, LOW);
            analogWrite(m1Enable, motor1Speed);
        }
        if (motor1Speed < 0)
        {
            digitalWrite(m11Pin, LOW);
            digitalWrite(m12Pin, HIGH);
            analogWrite(m1Enable, -motor1Speed);
        }
    }
    if (motor2Speed == 0)
    {
        digitalWrite(m21Pin, LOW);
        digitalWrite(m22Pin, LOW);
        analogWrite(m2Enable, motor2Speed);
    }
    else
    {
        if (motor2Speed > 0)
        {
            digitalWrite(m21Pin, HIGH);
            digitalWrite(m22Pin, LOW);
            analogWrite(m2Enable, motor2Speed);
        }
        if (motor2Speed < 0)
        {
            digitalWrite(m21Pin, LOW);
            digitalWrite(m22Pin, HIGH);
            analogWrite(m2Enable, -motor2Speed);
        }
    }
}