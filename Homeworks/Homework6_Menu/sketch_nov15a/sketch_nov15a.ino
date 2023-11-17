#include <Arduino.h>
// Global variables for sensor settings
unsigned long sensorSamplingInterval = 5; // Default 5 seconds
int ultrasonicThreshold = 100;            // Example default threshold in centimeters
int ldrThreshold = 500;                   // Example default threshold for LDR

// Global variable to simulate data storage
const int MAX_LOG_ENTRIES = 10; // Maximum number of log entries (for example)
int logData[MAX_LOG_ENTRIES];   // Array to store log data
int logIndex = 0;               // Index for the next log entry

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

// Initialize the log data with dummy values (for testing)
void initializeLogData()
{
    for (int i = 0; i < MAX_LOG_ENTRIES; i++)
    {
        logData[i] = -1; // -1 indicates no data
    }
    logIndex = 0;
}

void setup()
{
    Serial.begin(9600); // Initialize serial communication at 9600 baud rate
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    setRGBColor(redValue, greenValue, blueValue);
    // Initialize any other components (sensors, LEDs, etc.) here
}

void setRGBColor(int red, int green, int blue)
{
    analogWrite(RED_PIN, red);
    analogWrite(GREEN_PIN, green);
    analogWrite(BLUE_PIN, blue);
}
void loop()
{
    showMainMenu();
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
    while (!Serial.available())
    {
        // Wait for user input
    }

    String input = Serial.readStringUntil('\n');
    input.trim(); // Remove any leading/trailing whitespace
    return input;
}

void showMainMenu()
{
    Serial.println(F("=== Main Menu ==="));
    Serial.println(F("1. Sensor Settings"));
    Serial.println(F("2. Reset Logger Data"));
    Serial.println(F("3. System Status"));
    Serial.println(F("4. RGB LED Control"));
    Serial.println(F("Enter your choice: "));

    while (!Serial.available())
    {
        // Wait for user input
    }

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
        break;
    }
}

void handleSensorSettingsMenu()
{
    Serial.println(F("=== Sensor Settings Menu ==="));
    Serial.println(F("1. Set Sampling Interval (1-10 seconds)"));
    Serial.println(F("2. Set Ultrasonic Alert Threshold"));
    Serial.println(F("3. Set LDR Alert Threshold"));
    Serial.println(F("4. Back"));
    Serial.println(F("Enter your choice: "));

    while (!Serial.available())
    {
        // Wait for user input
    }

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
        return;
    default:
        Serial.println(F("Invalid choice, try again."));
        break;
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
    Serial.println(F("=== Reset Logger Data ==="));
    Serial.println(F("Are you sure you want to delete all data?"));
    Serial.println(F("1. Yes"));
    Serial.println(F("2. No"));
    Serial.println(F("Enter your choice: "));

    while (!Serial.available())
    {
        // Wait for user input
    }

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
        initializeLogData();
        Serial.println(F("All data has been deleted."));
    }
    else if (choice == 2)
    {
        // Do nothing, just return to the main menu
        Serial.println(F("Data deletion canceled."));
    }
    else
    {
        Serial.println(F("Invalid choice."));
    }
}

void displayCurrentSensorReadings()
{
    Serial.println(F("Current Sensor Readings:"));
    // Example readings; replace with actual sensor data retrieval
    Serial.print(F("Ultrasonic Sensor: "));
    Serial.println(100); // Placeholder value
    Serial.print(F("LDR: "));
    Serial.println(500); // Placeholder value
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

void displayLoggedData()
{
    Serial.println(F("Logged Data:"));
    for (int i = 0; i < MAX_LOG_ENTRIES; i++)
    {
        if (logData[i] != -1)
        { // Assuming -1 means no data
            Serial.print(F("Entry "));
            Serial.print(i);
            Serial.print(F(": "));
            Serial.println(logData[i]);
        }
    }
}

void handleSystemStatusMenu()
{
    Serial.println(F("=== System Status ==="));
    Serial.println(F("1. Current Sensor Readings"));
    Serial.println(F("2. Current Sensor Settings"));
    Serial.println(F("3. Display Logged Data"));
    Serial.println(F("4. Back"));
    Serial.println(F("Enter your choice: "));

    while (!Serial.available())
    {
        // Wait for user input
    }

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
        // Go back to the main menu
        return;
    default:
        Serial.println(F("Invalid choice, try again."));
        break;
    }
}

void handleRGBControlMenu()
{
    Serial.println(F("=== RGB LED Control ==="));
    Serial.println(F("1. Manual Color Control"));
    Serial.println(F("2. Toggle Automatic Mode (ON/OFF)"));
    Serial.println(F("3. Back"));
    Serial.println(F("Enter your choice: "));

    while (!Serial.available())
    {
        // Wait for user input
    }

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
        return; // Go back to the main menu
    default:
        Serial.println(F("Invalid choice, try again."));
        break;
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
