// Pin definitions
const int startPauseButtonPin = 2; // Start/Pause button
const int saveLapButtonPin = 3;    // Save Lap button
const int resetButtonPin = 8;      // Reset button
const int buzzerPin = 9;           // Buzzer

// Display pins
const int latchPin = 11; // STCP (latch pin) on the shift register
const int clockPin = 10; // SHCP (clock pin) on the shift register
const int dataPin = 12;  // DS (data pin) on the shift register

// Digit number
const int digit1 = 0;
const int digit2 = 1;
const int digit3 = 2;
const int digit4 = 3;
const int digit5Lap = 4;

// Segment control pins for a 5-digit display
const int segD1 = 4;
const int segD2 = 5;
const int segD3 = 6;
const int segD4 = 7;
const int segD5 = 13; // For the lap index display

// Display configuration
int displayDigits[] = {segD1, segD2, segD3, segD4, segD5};
const int displayCount = 5; // Number of digits in the display

// Stopwatch variables
volatile bool isRunning = false;
unsigned long lastDebounceTimeStartPause = 0;
unsigned long lastDebounceTimeSaveLap = 0;
unsigned long lastUpdateTime = 0;
unsigned long lapTimes[4] = {0};
int lapIndex = 0;
int displayNumber = 0;
int number = 0;

// Timing constants
const int debounceDelay = 100;       // Debounce delay in milliseconds
const unsigned long delayCount = 50; // Delay between display updates in milliseconds
unsigned long lastIncrement = 0;

// Tone frequencies and durations
const int resetToneFreq = 1000;         // Frequency for reset tone
const int resetToneDuration = 200;      // Duration for reset tone
const int startPauseToneFreq = 800;     // Frequency for start/pause tone
const int startPauseToneDuration = 150; // Duration for start/pause tone
const int saveLapToneFreq = 1200;       // Frequency for save lap tone
const int saveLapToneDuration = 150;    // Duration for save lap tone

// Byte encodings for numbers 0-9 on a 7-segment display
const int encodingsNumber = 10; // Number of unique encodings
byte byteEncodings[encodingsNumber] = {
    B11111100, // 0
    B01100000, // 1
    B11011010, // 2
    B11110010, // 3
    B01100110, // 4
    B10110110, // 5
    B10111110, // 6
    B11100000, // 7
    B11111110, // 8
    B11110110  // 9
};

// Function to turn off all digits

void startPauseISR()
{
  unsigned long currentTime = millis();
  if ((currentTime - lastDebounceTimeStartPause) > debounceDelay)
  {
    tone(buzzerPin, startPauseToneFreq, startPauseToneDuration);
    isRunning = !isRunning;
    lastDebounceTimeStartPause = currentTime;
  }
}

void saveLapISR()
{

  unsigned long currentTime = millis();
  if ((currentTime - lastDebounceTimeSaveLap) > debounceDelay)
  {
    tone(buzzerPin, saveLapToneFreq, saveLapToneDuration);
    if (isRunning)
    {
      lapTimes[lapIndex] = number;
      lapIndex = (lapIndex + 1) % 4;
    }
    else
    {
      lapIndex = (lapIndex + 1) % 4;
      displayNumber = lapTimes[lapIndex];
    }
    // Lap cycling logic will be in the loop
    lastDebounceTimeSaveLap = currentTime;
  }
}

void reset()
{
  static unsigned long lastResetButtonPress = 0;
  if (digitalRead(resetButtonPin) == LOW)
  {
    unsigned long currentTime = millis();
    if (currentTime - lastResetButtonPress > debounceDelay)
    {
      tone(buzzerPin, resetToneFreq, resetToneDuration);
      number = 0;
      displayNumber = 0;
      if (!isRunning)
      {
        for (int i = 0; i < 4; ++i)
        {
          lapTimes[i] = 0;
        }
      }

      lastResetButtonPress = currentTime;
    }
  }
}

void setup()
{
  // Initialize the pins connected to the shift register as outputs
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  // Initialize digit control pins and set them to LOW (off)
  for (int i = 0; i < displayCount; i++)
  {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }

  // Button pin setups with internal pull-up resistors
  pinMode(startPauseButtonPin, INPUT_PULLUP);
  pinMode(resetButtonPin, INPUT_PULLUP);
  pinMode(saveLapButtonPin, INPUT_PULLUP);

  // Buzzer pin setup
  pinMode(buzzerPin, OUTPUT);

  // Attach interrupts to the button pins
  attachInterrupt(digitalPinToInterrupt(startPauseButtonPin), startPauseISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(saveLapButtonPin), saveLapISR, FALLING);

  // Clear the shift register to avoid ghosting on the display
  writeReg(byteEncodings[0]);

  // Begin serial communication for debugging purposes
  Serial.begin(9600);
}

void loop()
{

  if (isRunning)
  {
    if (millis() - lastIncrement > delayCount)
    {
      number++;
      displayNumber = number % 10000;
      lastIncrement = millis();
    }
  }
  writeNumber(displayNumber);
  reset();
}

void writeReg(int digit)
{
  // Prepare to shift data by setting the latch pin low
  digitalWrite(latchPin, LOW);
  // Shift out the byte representing the current digit to the shift register
  shiftOut(dataPin, clockPin, MSBFIRST, digit);
  // Latch the data onto the output pins by setting the latch pin high
  digitalWrite(latchPin, HIGH);
}

void activateDisplay(int displayNumber)
{
  // Turn off all digit control pins to avoid ghosting
  for (int i = 0; i < displayCount; i++)
  {
    digitalWrite(displayDigits[i], HIGH);
  }
  // Turn on the current digit control pin
  digitalWrite(displayDigits[displayNumber], LOW);
}

void writeDigitNumber(int digit, int number, bool decimal = false)
{
  writeReg(B00000000);
  activateDisplay(digit);
  if (decimal)
  {
    writeReg((byteEncodings[number] | B00000001));
  }
  else
  {
    writeReg(byteEncodings[number]);
  }
  delay(0);
}

void writeNumber(int number)
{
  int fourth = number % 10;
  int third = (number / 10) % 10;
  int second = (number / 100) % 10;
  int first = (number / 1000) % 10;

  writeDigitNumber(digit1, first);
  writeDigitNumber(digit2, second);
  writeDigitNumber(digit3, third, true);
  writeDigitNumber(digit4, fourth);
  writeDigitNumber(digit5Lap, lapIndex);
}
