// Define connections to the shift register
const int latchPin = 11; // Connects to STCP (latch pin) on the shift register
const int clockPin = 10; // Connects to SHCP (clock pin) on the shift register
const int dataPin = 12;  // Connects to DS (data pin) on the shift register
// Define connections to the digit control pins for a 4-digit display
const int segD1 = 4;
const int segD2 = 5;
const int segD3 = 6;
const int segD4 = 7;
// Store the digits in an array for easy access
int displayDigits[] = {segD1, segD2, segD3, segD4};
const int displayCount = 4; // Number of digits in the display
// Define the number of unique encodings (0-9, A-F for hexadecimal)
const int encodingsNumber = 16;
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
    B11101110, // A
    B00111110, // b
    B10011100, // C
    B01111010, // d
    B10011110, // E
    B10001110  // F
};
// Variables for controlling the display update timing
unsigned long lastIncrement = 0;
unsigned long delayCount = 50; // Delay between updates (milliseconds)
unsigned long number = 0;      // The number being displayed
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
          // Begin serial communication for debugging purposes
          Serial.begin(9600);
}
void loop()
{
          // DONE: Check if the current time is greater than 'lastIncrement' plus 'delayCount'
          if (millis() - lastIncrement > delayCount)
          {
                    // DONE: Increment the number and reset 'lastIncrement'
                    number++;
                    // DONE: Ensure that 'number' wraps around after reaching 9999 to start over from 0
                    number %= 10000; // Wrap around after 9999
                    lastIncrement = millis();
          }
          // DONE: Display the incremented number on the 7-segment display using multiplexing
          writeNumber(number);
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
          // DONE: Initialize necessary variables for tracking the current number and digit position
          int currentNumber = number;
          int displayDigit = 3; // Start with the least significant digit
          int lastDigit = 0;
          // DONE: Loop through each digit of the current number
          while (currentNumber != 0)
          {
                    // DONE: Extract the last digit of the current number
                    lastDigit = currentNumber % 10;
                    // DONE: Activate the current digit on the display
                    activateDisplay(displayDigit);
                    // DONE: Output the byte encoding for the last digit to the display
                    writeReg(byteEncodings[lastDigit]);
                    // DONE: Implement a delay if needed for multiplexing visibility
                    delay(0); // A delay can be increased to visualize multiplexing
                    // DONE: Move to the next digit
                    displayDigit--;
                    // DONE: Update 'currentNumber' by removing the last digit
                    currentNumber /= 10;
                    // DONE: Clear the display to prevent ghosting between digit activations
                    writeReg(B00000000); // Clear the register to avoid ghosting
          }
}