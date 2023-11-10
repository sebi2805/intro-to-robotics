const int numDigits = 4; // Number of digits in the display

// Pin definitions for the buttons
const int startPauseButtonPin = 2;
const int saveLapButtonPin = 3;
const int resetButtonPin = 8;

// Pin definitions for the 74HC595 shift register
const int dataPin = 13;  // Connect to DS of 74HC595
const int latchPin = 12; // Connect to ST_CP of 74HC595
const int clockPin = 11; // Connect to SH_CP of 74HC595

// Variables for stopwatch logic
volatile bool isRunning = false;
unsigned long stopwatchTime = 0;
unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 100; // Update interval in milliseconds (10th of a second)
unsigned long lapTimes[4] = {0};
int lapIndex = 0;

// ISR for start/pause button
volatile unsigned long lastDebounceTimeStartPause = 0;
volatile unsigned long lastDebounceTimeSaveLap = 0;
volatile unsigned long lastDebounceTimeReset = 0;
const unsigned long debounceDelay = 50; // Debounce delay in milliseconds

const int digitPins[4] = {4, 5, 6, 7};

// Function to turn off all digits
void clearDigits()
{
  for (int i = 0; i < 4; i++)
  {
    digitalWrite(digitPins[i], LOW); // Set to HIGH for common cathode
  }
}

// Function to display a number on a specific digit
void displayDigit(int digit, byte segments)
{
  clearDigits();                        // Turn off all digits
  writeShiftRegister(segments);         // Send segment data to the shift register
  digitalWrite(digitPins[digit], HIGH); // Turn on the current digit (set to LOW for common cathode)
}

void startPauseISR()
{
  unsigned long currentTime = millis();
  if ((currentTime - lastDebounceTimeStartPause) > debounceDelay)
  {
    Serial.println("Start/Pause button pressed");
    isRunning = !isRunning;
    lastDebounceTimeStartPause = currentTime;
  }
}

void saveLapISR()
{
  Serial.println("Save Lap button pressed");
  unsigned long currentTime = millis();
  if ((currentTime - lastDebounceTimeSaveLap) > debounceDelay)
  {
    if (isRunning)
    {
      lapTimes[lapIndex] = stopwatchTime;
      lapIndex = (lapIndex + 1) % 4; // Cycle back to 0 after reaching 4
    }
    // Lap cycling logic will be in the loop
    lastDebounceTimeSaveLap = currentTime;
  }
}

void reset()
{

  static unsigned long lastResetButtonPress = 0;
  if (digitalRead(resetButtonPin) == LOW)
  { // Assuming button connects pin to GND when pressed
    unsigned long currentTime = millis();
    if (currentTime - lastResetButtonPress > debounceDelay)
    {
      Serial.println("Reset button pressed");
      if (!isRunning)
      {
        stopwatchTime = 0;
        lastUpdateTime = currentTime;
        for (int i = 0; i < 4; ++i)
        {
          lapTimes[i] = 0;
        }
        lapIndex = 0;
      }
      lastResetButtonPress = currentTime;
    }
  }
}

// Function to write a byte to the shift register
void writeShiftRegister(byte data)
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, data);
  digitalWrite(latchPin, HIGH);
}

// Placeholder for a function to get the segment values for a digit
// Segment values for a common anode 7-segment display
// Assuming segment 'a' is the LSB and 'dp' (decimal point) is the MSB
const byte segmentValues[] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111, // 9
};

byte getSegmentValue(int digit)
{
  if (digit >= 0 && digit <= 9)
  {
    return segmentValues[digit];
  }
  return 0x00; // Return all segments off if the digit is out of bounds
}
void setDigit(int digit, byte segments)
{
  // Calculate which 74HC595 outputs to enable for this digit
  byte digitSelect = 0b00010000 << digit; // This assumes that you have additional outputs for digit selection

  // Send the digit selection followed by the segment values to the shift registers
  writeShiftRegister(digitSelect);
  writeShiftRegister(segments);

  // Enable the output (latch the shift register)
  digitalWrite(latchPin, HIGH);
}

// Function to update the display
void updateDisplay()
{
  static int currentDigit = 0;
  byte segments;
  // Split the stopwatch time into individual digits
  int tenths = stopwatchTime % 10;
  int seconds = (stopwatchTime / 10) % 60;
  int minutes = (stopwatchTime / 600) % 60; // assuming you want to count minutes as well

  Serial.print("Minutes: ");
  Serial.print(minutes);
  Serial.print(", Seconds: ");
  Serial.print(seconds);
  Serial.print(", Tenths: ");
  Serial.println(tenths);
  // Convert these digits into the corresponding segments
  byte digits[numDigits] = {
      getSegmentValue(minutes / 10),
      getSegmentValue(minutes % 10),
      getSegmentValue(seconds / 10),
      getSegmentValue(seconds % 10)};

  // Add the decimal point to the second digit (for tenths of a second)
  digits[2] |= 0b10000000; // Assuming 'dp' is the MSB

  // Update the current digit
  setDigit(currentDigit, digits[currentDigit]);

  // Display the segments on the current digit
  Serial.print("Digit: ");
  Serial.print(currentDigit);
  Serial.print(", Segments: ");
  Serial.println(digits[currentDigit], BIN); // Print in binary format

  displayDigit(1, digits[currentDigit]);

  // Move to the next digit
  currentDigit = (currentDigit + 1) % 4;
}

void setup()
{
  // Button setup
  pinMode(startPauseButtonPin, INPUT_PULLUP);
  pinMode(resetButtonPin, INPUT_PULLUP);
  pinMode(saveLapButtonPin, INPUT_PULLUP);

  // Attach the interrupts to the buttons
  attachInterrupt(digitalPinToInterrupt(startPauseButtonPin), startPauseISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(saveLapButtonPin), saveLapISR, CHANGE);

  // Shift register setup
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  // Clear the shift register
  writeShiftRegister(0x00);

  // Serial setup for debugging
  Serial.begin(9600);
}

void loop()
{
  // Stopwatch logic
  if (isRunning)
  {
    if (millis() - lastUpdateTime >= updateInterval)
    {
      stopwatchTime++;
      lastUpdateTime += updateInterval;
    }
  }

  // Update the display
  updateDisplay();
  reset();

  // Small delay to reduce button bounce
  delay(10);
}
