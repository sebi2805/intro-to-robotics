
// Pin definitions for the buttons
const int startPauseButtonPin = 2;
const int saveLapButtonPin = 3;
const int resetButtonPin = 8;

// Variables for stopwatch logic
volatile bool isRunning = false;

unsigned long lastDebounceTimeStartPause = 0;
unsigned long lastDebounceTimeSaveLap = 0;

unsigned long lastUpdateTime = 0;
unsigned long lapTimes[4] = {0};
int lapIndex = 0;

int displayNumber = 0;
int number = 0;

const int buzzerPin = 9;
// Define connections to the shift register
const int latchPin = 11; // Connects to STCP (latch pin) on the shift register
const int clockPin = 10; // Connects to SHCP (clock pin) on the shift register
const int dataPin = 12;  // Connects to DS (data pin) on the shift register
// Define connections to the digit control pins for a 4-digit display
const int segD1 = 4;
const int segD2 = 5;
const int segD3 = 6;
const int segD4 = 7;
const int segD5 = 13; // for the index lap display
// Store the digits in an array for easy access
int displayDigits[] = {segD1, segD2, segD3, segD4, segD5};
const int displayCount = 5; // Number of digits in the display
// Define the number of unique encodings (0-9, A-F for hexadecimal)
const int debounceDelay = 100;

const int encodingsNumber = 10; // Variables for controlling the display update timing
unsigned long lastIncrement = 0;
unsigned long delayCount = 50; // Delay between updates (milliseconds)

const int resetToneFreq = 1000;    // Frequency in Hertz for reset tone
const int resetToneDuration = 200; // Duration in milliseconds for reset tone

const int startPauseToneFreq = 800;     // Frequency for start/pause tone
const int startPauseToneDuration = 150; // Duration for start/pause tone

const int saveLapToneFreq = 1200;    // Frequency for save lap tone
const int saveLapToneDuration = 150; // Duration for save lap tone

// Define byte encodings for the hexadecimal characters 0-F
byte byteEncodings[encodingsNumber] = {
    // A B C D E F G DP
    B11111100, // 0
    B01100000, // 1
    B11011010, // 2
    B11110010, // 3
    B01100110, // 4
    B10110110, // 5
    B10111110, // 6
    B11100000, // 7
    B11111110, // 8
    B11110110, // 9

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
    digitalWrite(displayDigits[i], HIGH);
  }
  digitalWrite(displayDigits[displayCount - 2], LOW);
  digitalWrite(displayDigits[displayCount - 1], LOW);

  pinMode(startPauseButtonPin, INPUT_PULLUP);
  pinMode(resetButtonPin, INPUT_PULLUP);
  pinMode(saveLapButtonPin, INPUT_PULLUP);

  pinMode(buzzerPin, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(startPauseButtonPin), startPauseISR, RISING);
  attachInterrupt(digitalPinToInterrupt(saveLapButtonPin), saveLapISR, RISING);
  writeReg(byteEncodings[0]); // Clear the register to avoid ghosting
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

void writeNumber(int number)
{

  if (number == 0)
  {
    writeReg(B00000000);
    activateDisplay(3);
    writeReg(byteEncodings[0]);

    writeReg(B00000000);
    activateDisplay(4);
    writeReg(byteEncodings[lapIndex]);
  }
  else
  {
    int currentNumber = number;
    int displayDigit = 4;
    int lastDigit = 0;

    while (currentNumber != 0)
    {
      activateDisplay(displayDigit);
      if (displayDigit == 4)
      {

        writeReg(byteEncodings[lapIndex]);
      }
      else
      {
        lastDigit = currentNumber % 10;

        if (displayDigit == 2)
        {
          writeReg((byteEncodings[lastDigit] | B00000001));
        }
        else
        {
          writeReg(byteEncodings[lastDigit]);
        }
        currentNumber /= 10;
      }
      delay(0); // A delay can be increased to visualize multiplexing
      displayDigit--;
      writeReg(B00000000); // Clear the register to avoid ghosting
    }
  }
}
