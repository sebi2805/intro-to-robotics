// Number of readings to average
// Because I will use moving averages as a filter
const int NUM_READINGS = 10;

// Input controls
const int RED_COLOR_INPUT_PIN = A0;
const int GREEN_COLOR_INPUT_PIN = A1;
const int BLUE_COLOR_INPUT_PIN = A2;

const int RED_COLOR_OUTPUT_PIN = 9;
const int GREEN_COLOR_OUTPUT_PIN = 10;
const int BLUE_COLOR_OUTPUT_PIN = 11;

const int MAX_ANALOG_READ = 1023; // Maximum value that can be read from an analog input (Potentiometer in my case of testing)
const int PWM_RANGE = 255;        // Maximum value for PWM output

// Threshold for considering a signal significant. Signals below this are ignored.
// This can always be changed to 0 or better to have another potentiometer to use it
// as of right now I don't have a fourth potentiometer to actually implement this, but this was my original idea.
const int MIN_SIGNAL_THRESHOLD = 0;
// I did choose a 20ms delay in order to smooth things out
const int DELAY = 20;
// Arrays to hold the last NUM_READINGS analog readings for each color
int redInputValues[NUM_READINGS];
int greenInputValues[NUM_READINGS];
int blueInputValues[NUM_READINGS];

int readIndex = 0; // Index for the current reading within the arrays

// Sum of the last NUM_READINGS analog readings for each color
int totalRed = 0, totalGreen = 0, totalBlue = 0;

// Averaged analog readings for each color
int averageRed = 0, averageGreen = 0, averageBlue = 0;

// PWM values mapped from the averaged analog readings
int mappedRedValue, mappedGreenValue, mappedBlueValue;

void setup()
{
  Serial.begin(9600);

  // I have
  pinMode(RED_COLOR_INPUT_PIN, INPUT);
  pinMode(GREEN_COLOR_INPUT_PIN, INPUT);
  pinMode(BLUE_COLOR_INPUT_PIN, INPUT);

  pinMode(RED_COLOR_OUTPUT_PIN, OUTPUT);
  pinMode(GREEN_COLOR_OUTPUT_PIN, OUTPUT);
  pinMode(BLUE_COLOR_OUTPUT_PIN, OUTPUT);

  for (int i = 0; i < NUM_READINGS; i++)
  {
    redInputValues[i] = 0;
    greenInputValues[i] = 0;
    blueInputValues[i] = 0;
  }
}

void loop()
{
  // Remove the oldest reading
  totalRed -= redInputValues[readIndex];
  totalGreen -= greenInputValues[readIndex];
  totalBlue -= blueInputValues[readIndex];

  // Read new analog values
  redInputValues[readIndex] = analogRead(RED_COLOR_INPUT_PIN);
  greenInputValues[readIndex] = analogRead(GREEN_COLOR_INPUT_PIN);
  blueInputValues[readIndex] = analogRead(BLUE_COLOR_INPUT_PIN);

  // Add the new reading to the total
  totalRed += redInputValues[readIndex];
  totalGreen += greenInputValues[readIndex];
  totalBlue += blueInputValues[readIndex];

  // Calculate the average
  averageRed = totalRed / NUM_READINGS;
  averageGreen = totalGreen / NUM_READINGS;
  averageBlue = totalBlue / NUM_READINGS;

  // Map the average values to PWM values
  mappedRedValue = map(averageRed, 0, MAX_ANALOG_READ, 0, PWM_RANGE);
  mappedGreenValue = map(averageGreen, 0, MAX_ANALOG_READ, 0, PWM_RANGE);
  mappedBlueValue = map(averageBlue, 0, MAX_ANALOG_READ, 0, PWM_RANGE);

  // Write the mapped values to the corresponding color output pins if they exceed the minimum threshold
  analogWrite(RED_COLOR_OUTPUT_PIN, (mappedRedValue > MIN_SIGNAL_THRESHOLD) ? mappedRedValue : 0);
  analogWrite(GREEN_COLOR_OUTPUT_PIN, (mappedGreenValue > MIN_SIGNAL_THRESHOLD) ? mappedGreenValue : 0);
  analogWrite(BLUE_COLOR_OUTPUT_PIN, (mappedBlueValue > MIN_SIGNAL_THRESHOLD) ? mappedBlueValue : 0);

  // Increment the index
  // I would have enjoyed to use a queue here, but I didn't want to implement it.
  readIndex = (readIndex + 1) % NUM_READINGS;

  // Display the mapped and averaged values
  // Serial.print("Mapped and Averaged Red: ");
  // Serial.print(mappedRedValue);
  // Serial.print(", Mapped and Averaged Green: ");
  // Serial.print(mappedGreenValue);
  // Serial.print(", Mapped and Averaged Blue: ");
  // Serial.println(mappedBlueValue);
  // I think it is bad practice to leave debugging code in the final product, so I commented it out.

  delay(DELAY);
}
