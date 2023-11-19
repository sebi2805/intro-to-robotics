#include <EEPROM.h>
// Global variables for sensor settings
unsigned long sensorSamplingInterval = 1000; // 1 second in milliseconds
unsigned long lastSampleTime = 0;
int ultrasonicThreshold = 24; // Example default threshold in centimeters
int ldrThreshold = 24;        // Example default threshold for LDR

const float THRESHOLD_MARGIN = 0.8;
// Global variable to simulate data storage
const int MAX_LOG_ENTRIES = 10; // Maximum number of log entries (for example)

const int ultrasonicTriggerPin = 2; // Example pin for the ultrasonic trigger
const int ultrasonicEchoPin = 3;    // Example pin for the ultrasonic echo
const int ldrPin = A0;
// RGB LED pins (assuming common anode RGB LED)
const int RED_PIN = 11;
const int GREEN_PIN = 10;
const int BLUE_PIN = 9;

// RGB LED state
int redValue = 0;
int greenValue = 0;
int blueValue = 0;

// Automatic mode flag
bool isAutoMode = false;

int mainMenuIndex = 0;

enum SensorType
{
    LDR_SENSOR,
    ULTRASONIC_SENSOR
    // Add more sensor types here if needed
};

const int LDR_START_ADDR = 0;                              // Starting address for LDR readings
const int MAX_READINGS_PER_SENSOR = 20;                    // Max number of readings per sensor
const int ULTRASONIC_START_ADDR = MAX_READINGS_PER_SENSOR; // Starting address for Ultrasonic readings
int currentLDRIndex = 0;                                   // Index for the next LDR reading in EEPROM
int currentUltrasonicIndex = 0;

int readUltrasonic()
{
    // Clear the trigger pin
    digitalWrite(ultrasonicTriggerPin, LOW);
    delayMicroseconds(2);

    // Trigger the sensor by setting the trigger pin high for 10 microseconds
    digitalWrite(ultrasonicTriggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(ultrasonicTriggerPin, LOW);

    // Read the echo pin; pulseIn returns the duration in microseconds
    long duration = pulseIn(ultrasonicEchoPin, HIGH);

    // Calculate the distance
    int distance = static_cast<int>(duration * 0.034 / 2); // Speed of sound wave divided by 2 (go and back)

    return distance;
}

void writeSensorDataToEEPROM(SensorType sensorType, int index, int value)
{
    int address;
    if (sensorType == LDR_SENSOR)
    {
        address = index;               // LDR data starts at 0
        EEPROM.update(address, value); // Write LDR value (assuming it's already mapped to 0-255)
    }
    else if (sensorType == ULTRASONIC_SENSOR)
    {
        address = MAX_READINGS_PER_SENSOR * 2 + index * 2; // Ultrasonic data follows LDR data
        byte lowByte = value & 0xFF;                       // Extract low byte
        byte highByte = (value >> 8) & 0xFF;               // Extract high byte
        EEPROM.update(address, lowByte);                   // Write low byte
        EEPROM.update(address + 1, highByte);              // Write high byte
    }
    else
    {
        Serial.println(F("Invalid Sensor Type"));
        return;
    }
}

int readSensorDataFromEEPROM(SensorType sensorType, int index)
{
    int address, value;

    if (sensorType == LDR_SENSOR)
    {
        address = index;              // LDR data starts at 0
        value = EEPROM.read(address); // Read single byte
    }
    else if (sensorType == ULTRASONIC_SENSOR)
    {
        address = MAX_READINGS_PER_SENSOR * 2 + index * 2; // Ultrasonic data follows LDR data
        byte lowByte = EEPROM.read(address);               // Read low byte
        byte highByte = EEPROM.read(address + 1);          // Read high byte
        value = (highByte << 8) | lowByte;                 // Combine bytes to form the original value
    }
    else
    {
        Serial.println(F("Invalid Sensor Type"));
        return -1; // Return an error or invalid value
    }

    return value;
}

// Initialize the log data with dummy values (for testing)

void initializeLogData()
{
    // Initialize LDR data
    for (int i = 0; i < MAX_READINGS_PER_SENSOR; i++)
    {
        writeSensorDataToEEPROM(LDR_SENSOR, i, 0);
    }

    // Initialize Ultrasonic data
    for (int i = 0; i < MAX_READINGS_PER_SENSOR; i++)
    {
        writeSensorDataToEEPROM(ULTRASONIC_SENSOR, i, 0);
    }
}

void performSensorSampling()
{
    int ldrValue = analogRead(ldrPin);
    ldrValue = map(ldrValue, 0, 1023, 0, 255); // Read LDR value
    int ultrasonicValue = readUltrasonic();    // Read Ultrasonic sensor value

    // Write these values to EEPROM
    writeSensorDataToEEPROM(LDR_SENSOR, currentLDRIndex, ldrValue);
    writeSensorDataToEEPROM(ULTRASONIC_SENSOR, currentUltrasonicIndex, ultrasonicValue);

    // Update indexes for the next write operation
    updateIndexes();
}
void updateIndexes()
{
    // Increment the LDR index and wrap if it reaches the maximum
    currentLDRIndex = (currentLDRIndex + 1) % MAX_READINGS_PER_SENSOR;

    // Increment the Ultrasonic index and wrap if it reaches the maximum
    currentUltrasonicIndex = (currentUltrasonicIndex + 1) % MAX_READINGS_PER_SENSOR;
}

// Start Ultrasonic index after LDR readings
// Index for the next Ultrasonic reading in EEPROM

void setup()
{
    Serial.begin(9600); // Initialize serial communication at 9600 baud rate
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    setRGBColor(redValue, greenValue, blueValue);
    pinMode(ultrasonicTriggerPin, OUTPUT);
    pinMode(ultrasonicEchoPin, INPUT);
    // Initialize any other components (sensors, LEDs, etc.) here
}

void setRGBColor(int red, int green, int blue)
{
    analogWrite(RED_PIN, red);
    analogWrite(GREEN_PIN, green);
    analogWrite(BLUE_PIN, blue);
}

void updateLEDColor()
{
    int ldrValue = analogRead(ldrPin);
    int ultrasonicValue = readUltrasonic();

    if (ldrValue >= ldrThreshold || ultrasonicValue >= ultrasonicThreshold)
    {
        // At least one sensor exceeded its threshold - RED
        setRGBColor(255, 0, 0);
    }
    else if (ldrValue >= ldrThreshold * THRESHOLD_MARGIN || ultrasonicValue >= ultrasonicThreshold * THRESHOLD_MARGIN)
    {
        // Sensors are approaching their thresholds - YELLOW
        setRGBColor(255, 255, 0);
    }
    else
    {
        // All sensor values are in the safe range - GREEN
        setRGBColor(0, 255, 0);
    }
}

void loop()
{

    showMainMenu();
    updateLEDColor();
    if (millis() - lastSampleTime >= sensorSamplingInterval)
    {
        performSensorSampling();
        lastSampleTime = millis(); // Reset the last sample time
    }
    delay(9);
}

bool getNumericInput(const String &input, int &outNumber)
{
    if (input.length() == 0 || !isNumeric(input))
    {
        return false;
    }

    outNumber = input.toInt();
    return true;
}

bool isNumeric(const String &str)
{
    for (char c : str)
    {
        if (!isDigit(c) && c != '-' && c != '+')
        { // Allow for negative and positive numbers
            return false;
        }
    }
    return true;
}
String readLine()
{
    if (Serial.available())
    {
        String input = Serial.readStringUntil('\n');
        input.trim(); // Remove any leading/trailing whitespace
        return input;
    }
}

void displayMainMenuOptions()
{
    static bool shouldDisplay = true;
    if (shouldDisplay)
    {
        Serial.println(F("=== Main Menu ==="));
        Serial.println(F("1. Sensor Settings"));
        Serial.println(F("2. Reset Logger Data"));
        Serial.println(F("3. System Status"));
        Serial.println(F("4. RGB LED Control"));
        Serial.println(F("Enter your choice: "));
        shouldDisplay = false;
    }
    if (Serial.available())
    {
        String input = readLine();

        if (!getNumericInput(input, mainMenuIndex))
        {
            Serial.println(F("Invalid choice, please enter a number."));
            return;
        }
        shouldDisplay = true;
    }
}

void reset()
{
    mainMenuIndex = 0;
}

void showMainMenu()
{

    switch (mainMenuIndex)
    {
    case 0:
        displayMainMenuOptions();
        break;
    case 1:
        handleSensorSettingsMenu();
        break;
    case 2:
        handleResetLoggerData();
        break;
    case 3:
        handleSystemStatusMenu();
        break;
    case 4:
        handleRGBControlMenu();
        break;
    default:
        Serial.println(F("Invalid choice, try again."));
        reset();
        break;
    }
}

void handleSensorSettingsMenu()
{

    static bool shouldDisplay = true;
    if (shouldDisplay)
    {
        Serial.println(F("=== Sensor Settings Menu ==="));
        Serial.println(F("1. Set Sampling Interval (1-10 seconds)"));
        Serial.println(F("2. Set Ultrasonic Alert Threshold"));
        Serial.println(F("3. Set LDR Alert Threshold"));
        Serial.println(F("4. Back"));
        Serial.println(F("Enter your choice: "));
        shouldDisplay = false;
    }
    if (Serial.available())
    {
        String input = readLine();
        int choice;
        if (!getNumericInput(input, choice))
        {
            Serial.println(F("Invalid choice, please enter a number."));
            return;
        }
        shouldDisplay = true;
        switch (choice)
        {
        case 1:
            setSamplingInterval();
            break;
        case 2:
            setUltrasonicThreshold();
            break;
        case 3:
            setLDRThreshold();
            break;
        case 4:
            // Go back to the main menu
            reset();
            return;
        default:
            Serial.println(F("Invalid choice, try again."));
            break;
        }
    }
}

void setSamplingInterval()
{
    Serial.println(F("Enter sampling interval (1-10 seconds):"));

    String input = readLine();
    int interval;

    if (getNumericInput(input, interval) && interval >= 1 && interval <= 10)
    {
        sensorSamplingInterval = interval;
        Serial.print(F("Sampling interval set to "));
        Serial.print(interval);
        Serial.println(F(" seconds."));
    }
    else
    {
        Serial.println(F("Invalid interval. Please enter a value between 1 and 10."));
    }
}

void setUltrasonicThreshold()
{
    Serial.println(F("Enter ultrasonic threshold (2-400 cm):"));

    String input = readLine();
    int threshold;

    if (getNumericInput(input, threshold) && threshold >= 2 && threshold <= 400)
    {
        ultrasonicThreshold = threshold;
        Serial.print(F("Ultrasonic threshold set to "));
        Serial.print(threshold);
        Serial.println(F(" cm."));
    }
    else
    {
        Serial.println(F("Invalid threshold. Please enter a value between 2 and 400 cm."));
    }
}

void setLDRThreshold()
{
    Serial.println(F("Enter LDR threshold (0-1023):"));

    String input = readLine();
    int threshold;

    if (getNumericInput(input, threshold) && threshold >= 0 && threshold <= 1023)
    {
        ldrThreshold = threshold;
        Serial.print(F("LDR threshold set to "));
        Serial.print(threshold);
        Serial.println(F("."));
    }
    else
    {
        Serial.println(F("Invalid threshold. Please enter a value between 0 and 1023."));
    }
}

void handleResetLoggerData()
{
    static bool shouldDisplay = true;
    if (shouldDisplay)
    {
        Serial.println(F("=== Reset Logger Data ==="));
        Serial.println(F("Are you sure you want to delete all data?"));
        Serial.println(F("1. Yes"));
        Serial.println(F("2. No"));
        shouldDisplay = false;
    }
    if (Serial.available())
    {

        Serial.println(F("Enter your choice: "));
        String input = readLine();
        int choice;

        if (!getNumericInput(input, choice))
        {
            Serial.println(F("Invalid choice, please enter a number."));
            return;
        }
        if (choice == 1)
        {
            // Reset the log data
            Serial.println(F("All data has been deleted."));
            initializeLogData();
            reset();
        }
        else if (choice == 2)
        {
            // Do nothing, just return to the main menu
            Serial.println(F("Data deletion canceled."));
            reset();
        }
        else
        {
            Serial.println(F("Invalid choice."));
            // dont reset
        }
        shouldDisplay = true;
    }
}

void displayCurrentSensorReadings()
{
    Serial.println(F("Current EEPROM Values:"));

    // Read and display the current value from EEPROM for the LDR sensor
    int currentLDRValue = readSensorDataFromEEPROM(LDR_SENSOR, currentLDRIndex);
    Serial.print(F("LDR Value: "));
    Serial.println(currentLDRValue);

    // Read and display the current value from EEPROM for the Ultrasonic sensor
    int currentUltrasonicValue = readSensorDataFromEEPROM(ULTRASONIC_SENSOR, currentUltrasonicIndex);
    Serial.print(F("Ultrasonic Value: "));
    Serial.println(currentUltrasonicValue);
}

void displayLoggedData()
{
    Serial.println(F("Logged Data:"));

    // Display LDR Sensor Data
    Serial.println(F("LDR Sensor Data:"));
    for (int i = 0; i < MAX_READINGS_PER_SENSOR; i++)
    {
        int ldrValue = readSensorDataFromEEPROM(LDR_SENSOR, i);
        Serial.print(F("Entry "));
        Serial.print(i);
        Serial.print(F(": "));
        Serial.println(ldrValue);
    }

    // Display Ultrasonic Sensor Data
    Serial.println(F("Ultrasonic Sensor Data:"));
    for (int i = 0; i < MAX_READINGS_PER_SENSOR; i++)
    {
        int ultrasonicValue = readSensorDataFromEEPROM(ULTRASONIC_SENSOR, i);
        Serial.print(F("Entry "));
        Serial.print(i);
        Serial.print(F(": "));
        Serial.println(ultrasonicValue);
    }
}

void handleRGBControlMenu()
{
    static bool shouldDisplay = true;
    if (shouldDisplay)
    {
        Serial.println(F("=== RGB LED Control ==="));
        Serial.println(F("1. Manual Color Control"));
        Serial.println(F("2. Toggle Automatic Mode (ON/OFF)"));
        Serial.println(F("3. Back"));
        Serial.println(F("Enter your choice: "));
        shouldDisplay = false;
    }
    if (Serial.available())
    {
        String input = readLine();
        int choice;

        if (!getNumericInput(input, choice))
        {
            Serial.println(F("Invalid choice, please enter a number."));
            return;
        }

        switch (choice)
        {
        case 1:
            manualColorControl();
            break;
        case 2:
            toggleAutomaticMode();
            break;
        case 3:
            reset();
            return; // Go back to the main menu
        default:
            Serial.println(F("Invalid choice, try again."));
            break;
        }
        shouldDisplay = true;
    }
}
void displayCurrentSensorSettings()
{
    Serial.println(F("Current Sensor Settings:"));
    Serial.print(F("Sampling Interval: "));
    Serial.print(sensorSamplingInterval);
    Serial.println(F(" seconds"));
    Serial.print(F("Ultrasonic Threshold: "));
    Serial.println(ultrasonicThreshold);
    Serial.print(F("LDR Threshold: "));
    Serial.println(ldrThreshold);
}

void handleSystemStatusMenu()
{
    static bool shouldDisplay = true;
    if (shouldDisplay)
    {
        Serial.println(F("=== System Status ==="));
        Serial.println(F("1. Current Sensor Readings"));
        Serial.println(F("2. Current Sensor Settings"));
        Serial.println(F("3. Display Logged Data"));
        Serial.println(F("4. Back"));
        Serial.println(F("Enter your choice: "));
        shouldDisplay = false;
    }

    if (Serial.available())
    {
        String input = readLine();
        int choice;

        if (!getNumericInput(input, choice))
        {
            Serial.println(F("Invalid choice, please enter a number."));
            return;
        }

        switch (choice)
        {
        case 1:
            displayCurrentSensorReadings();
            break;
        case 2:
            displayCurrentSensorSettings();
            break;
        case 3:
            displayLoggedData();
            break;
        case 4:
            reset();
            // Go back to the main menu
            return;
        default:
            Serial.println(F("Invalid choice, try again."));
            break;
        }
        shouldDisplay = true;
    }
}
void manualColorControl()
{
    int r = getIndividualColorValue("R");
    int g = getIndividualColorValue("G");
    int b = getIndividualColorValue("B");

    setRGBColor(r, g, b);
    Serial.print(F("RGB color set to R:"));
    Serial.print(r);
    Serial.print(F(", G:"));
    Serial.print(g);
    Serial.print(F(", B:"));
    Serial.println(b);
}

int getIndividualColorValue(const char *colorName)
{
    while (true)
    {
        Serial.print(F("Enter "));
        Serial.print(colorName);
        Serial.println(F(" value (0-255):"));

        String input = readLine();
        int value;

        if (getNumericInput(input, value) && value >= 0 && value <= 255)
        {
            return value;
        }
        else
        {
            Serial.println(F("Invalid value. Please enter a number between 0 and 255."));
        }
    }
}

void toggleAutomaticMode()
{
    isAutoMode = !isAutoMode;
    String modeStatus = isAutoMode ? "ON" : "OFF";
    Serial.print(F("Automatic mode "));
    Serial.println(modeStatus);
    // Additional code to handle automatic mode logic
}
