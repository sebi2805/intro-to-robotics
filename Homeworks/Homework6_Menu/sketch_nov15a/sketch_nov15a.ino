#include <EEPROM.h>
// Global variables for sensor settings
unsigned long sensorSamplingInterval = 1000; // 1 second in milliseconds
unsigned long lastSampleTime = 0;
int ultrasonicThreshold = 25; // Example default threshold in centimeters
int ldrThreshold = 40;        // Example default threshold for LDR

const float thresholdMargin = 0.8;

const int ultrasonicTriggerPin = 2; // Example pin for the ultrasonic trigger
const int ultrasonicEchoPin = 3;    // Example pin for the ultrasonic echo
const int ldrPin = A0;
// RGB LED pins (assuming common anode RGB LED)
const int redPin = 11;
const int greenPin = 10;
const int bluePin = 9;
const int buzzerPin = 12;

// RGB LED state
int redValue = 0;
int greenValue = 0;
int blueValue = 0;

// Automatic mode flag
bool isAutoMode = true;

int mainMenuIndex = 0;

enum SensorType
{
    ldrSensor,
    ultrasonicSensor
    // Add more sensor types here if needed
};

const int ldrStartAdress = 0;                            // Starting address for LDR readings
const int maxReadingsPerSensor = 20;                     // Max number of readings per sensor
const int ultrasonicStartAddress = maxReadingsPerSensor; // Starting address for Ultrasonic readings
int currentLDRIndex = 0;                                 // Index for the next LDR reading in EEPROM
int currentUltrasonicIndex = 0;

void setup()
{
    Serial.begin(9600); // Initialize serial communication at 9600 baud rate
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
    setRGBColor(redValue, greenValue, blueValue);
    pinMode(ultrasonicTriggerPin, OUTPUT);
    pinMode(ultrasonicEchoPin, INPUT);
    pinMode(ldrPin, INPUT);
    initializeLogData();
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

void initializeLogData()
{
    // Initialize LDR data
    for (int i = 0; i < maxReadingsPerSensor; i++)
    {
        writeSensorDataToEEPROM(ldrSensor, i, 0);
    }

    // Initialize Ultrasonic data
    for (int i = 0; i < maxReadingsPerSensor; i++)
    {
        writeSensorDataToEEPROM(ultrasonicSensor, i, 0);
    }
}

///////////////////////////////////////////////////////////////
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

void reset()
{
    mainMenuIndex = 0;
}
///////////////////////////////////////////////////////////////

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
    if (sensorType == ldrSensor)
    {
        address = index;               // LDR data starts at 0
        EEPROM.update(address, value); // Write LDR value (assuming it's already mapped to 0-255)
    }
    else if (sensorType == ultrasonicSensor)
    {
        address = maxReadingsPerSensor * 2 + index * 2; // Ultrasonic data follows LDR data
        byte lowByte = value & 0xFF;                    // Extract low byte
        byte highByte = (value >> 8) & 0xFF;            // Extract high byte
        EEPROM.update(address, lowByte);                // Write low byte
        EEPROM.update(address + 1, highByte);           // Write high byte
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

    if (sensorType == ldrSensor)
    {
        address = index;              // LDR data starts at 0
        value = EEPROM.read(address); // Read single byte
    }
    else if (sensorType == ultrasonicSensor)
    {
        address = maxReadingsPerSensor * 2 + index * 2; // Ultrasonic data follows LDR data
        byte lowByte = EEPROM.read(address);            // Read low byte
        byte highByte = EEPROM.read(address + 1);       // Read high byte
        value = (highByte << 8) | lowByte;              // Combine bytes to form the original value
    }
    else
    {
        Serial.println(F("Invalid Sensor Type"));
        return -1; // Return an error or invalid value
    }

    return value;
}

///////////////////////////////////////////////////////////////

void performSensorSampling()
{
    int ldrValue = analogRead(ldrPin);
    ldrValue = map(ldrValue, 0, 1023, 0, 255); // Read LDR value
    int ultrasonicValue = readUltrasonic();    // Read Ultrasonic sensor value

    // Write these values to EEPROM
    writeSensorDataToEEPROM(ldrSensor, currentLDRIndex, ldrValue);
    writeSensorDataToEEPROM(ultrasonicSensor, currentUltrasonicIndex, ultrasonicValue);

    // Update indexes for the next write operation
    updateIndexes();
}

///////////////////////////////////////////////////////////////

void updateIndexes()
{
    // Increment the LDR index and wrap if it reaches the maximum
    currentLDRIndex = (currentLDRIndex + 1) % maxReadingsPerSensor;

    // Increment the Ultrasonic index and wrap if it reaches the maximum
    currentUltrasonicIndex = (currentUltrasonicIndex + 1) % maxReadingsPerSensor;
}

///////////////////////////////////////////////////////////////

void setRGBColor(int red, int green, int blue)
{
    analogWrite(redPin, red);
    analogWrite(greenPin, green);
    analogWrite(bluePin, blue);
}

void updateLEDColor()
{
    if (isAutoMode)
    {
        int ldrValue = readSensorDataFromEEPROM(ldrSensor, currentLDRIndex - 1);
        int ultrasonicValue = readSensorDataFromEEPROM(ultrasonicSensor, currentUltrasonicIndex - 1);

        if (ldrValue >= ldrThreshold || ultrasonicValue >= ultrasonicThreshold)
        {
            int frequency = 1000;
            int duration = 500;

            tone(buzzerPin, frequency); // Start playing a tone
            delay(duration);            // Wait for the duration of the tone
            noTone(buzzerPin);
            setRGBColor(255, 0, 0);
        }
        else if (ldrValue >= ldrThreshold * thresholdMargin || ultrasonicValue >= ultrasonicThreshold * thresholdMargin)
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
}

///////////////////////////////////////////////////////////////

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
        Serial.println(F("5. Plot LDR Sensor Data"));
        Serial.println(F("6. Plot Ultrasonic Sensor Data"));
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
    case 5:
        plotSensorData(ldrSensor);
        break;
    case 6:
        plotSensorData(ultrasonicSensor);
        break;
    default:
        Serial.println(F("Invalid choice, try again."));
        reset();
        break;
    }
}
void plotSensorData(SensorType sensorType)
{
    Serial.println(F("Plotting data. Send 'q' to stop."));

    while (!Serial.available() || Serial.read() != 'q')
    {
        int value = (sensorType == ldrSensor) ? analogRead(ldrPin) : readUltrasonic();
        Serial.println(value);
        delay(sensorSamplingInterval);

        if (Serial.available() && Serial.read() == 'q')
        {
            break;
        }
    }

    Serial.println(F("Stopped plotting."));
    reset(); // Reset to show main menu again
}

///////////////////////////////////////////////////////////////

void handleSensorSettingsMenu()
{

    static int subMenuIndex = 0;
    static bool shouldDisplay = true;

    switch (subMenuIndex)
    {
    case 0:
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
            if (!getNumericInput(input, subMenuIndex))
            {
                Serial.println(F("Invalid choice, please enter a number."));
                return;
            }
            shouldDisplay = true;
        }
        break;
    case 1:
        if (setSamplingInterval())
            subMenuIndex = 0;
        break;
    case 2:
        if (setUltrasonicThreshold())
            subMenuIndex = 0;
        break;
    case 3:
        if (setLDRThreshold())
            subMenuIndex = 0;
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

bool setSamplingInterval()
{
    static bool shouldDisplay = true;
    if (shouldDisplay)
    {
        Serial.println(F("Enter sampling interval (1-10 seconds):"));
        shouldDisplay = false;
    }
    else if (Serial.available())
    {
        String input = readLine();
        int interval;
        Serial.println(input);
        if (!getNumericInput(input, interval))
        {
            Serial.println(F("Invalid interval. Please enter a number."));
        }
        else if (interval >= 1 && interval <= 10)
        {
            sensorSamplingInterval = interval * 1000;
            Serial.print(F("Sampling interval set to "));
            Serial.print(interval);
            Serial.println(F(" seconds."));
            shouldDisplay = true;
            return true;
        }
        else
        {
            Serial.println(F("Invalid interval. Please enter a value between 1 and 10."));
        }
    }
    return false;
}

bool setUltrasonicThreshold()
{
    static bool shouldDisplay = true;
    if (shouldDisplay)
    {
        Serial.println(F("Enter ultrasonic threshold (2-400 cm):"));
        shouldDisplay = false;
    }
    else if (Serial.available())
    {
        String input = readLine();
        int threshold;

        if (getNumericInput(input, threshold) && threshold >= 2 && threshold <= 400)
        {
            ultrasonicThreshold = threshold;
            Serial.print(F("Ultrasonic threshold set to "));
            Serial.print(threshold);
            Serial.println(F(" cm."));
            shouldDisplay = true;
            return true;
        }
        else
        {
            Serial.println(F("Invalid threshold. Please enter a value between 2 and 400 cm."));
        }
    }
    return false;
}

bool setLDRThreshold()
{
    static bool shouldDisplay = true;
    if (shouldDisplay)
    {
        Serial.println(F("Enter LDR threshold (0-1023):"));
        shouldDisplay = false;
    }
    else if (Serial.available())
    {
        String input = readLine();
        int threshold;

        if (getNumericInput(input, threshold) && threshold >= 0 && threshold <= 1023)
        {
            ldrThreshold = threshold;
            Serial.print(F("LDR threshold set to "));
            Serial.print(threshold);
            Serial.println(F("."));
            shouldDisplay = true;
            return true;
        }
        else
        {
            Serial.println(F("Invalid threshold. Please enter a value between 0 and 1023."));
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////

void displayCurrentSensorReadings()
{
    Serial.println(F("Current EEPROM Values:"));

    // Read and display the current value from EEPROM for the LDR sensor
    int currentLDRValue = readSensorDataFromEEPROM(ldrSensor, currentLDRIndex - 1);
    Serial.print(F("LDR Value: "));
    Serial.println(currentLDRValue);

    // Read and display the current value from EEPROM for the Ultrasonic sensor
    int currentUltrasonicValue = readSensorDataFromEEPROM(ultrasonicSensor, currentUltrasonicIndex - 1);
    Serial.print(F("Ultrasonic Value: "));
    Serial.println(currentUltrasonicValue);
}

void displayLoggedData()
{
    Serial.println(F("Logged Data:"));

    // Display LDR Sensor Data
    Serial.println(F("LDR Sensor Data:"));
    for (int i = 0; i < maxReadingsPerSensor; i++)
    {
        int ldrValue = readSensorDataFromEEPROM(ldrSensor, i);
        Serial.print(F("Entry "));
        Serial.print(i);
        Serial.print(F(": "));
        Serial.println(ldrValue);
    }

    // Display Ultrasonic Sensor Data
    Serial.println(F("Ultrasonic Sensor Data:"));
    for (int i = 0; i < maxReadingsPerSensor; i++)
    {
        int ultrasonicValue = readSensorDataFromEEPROM(ultrasonicSensor, i);
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

///////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////

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
}
