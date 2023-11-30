#include <LedControl.h>
#include <LiquidCrystal.h>

// Pins for the MAX7219 matrix
int DIN = 3;
int CLK = 4;
int CS = 5;

const int joystickXPin = A0;
const int joystickYPin = A1;
const int buttonPin = 2;


const int RS_PIN = 8;
const int E_PIN = 9;
const int D4_PIN = 10;
const int D5_PIN = 11;
const int D6_PIN = 12;
const int D7_PIN = 13;

// Initialize the LCD with the interface pins
LiquidCrystal lcd(RS_PIN, E_PIN, D4_PIN, D5_PIN, D6_PIN, D7_PIN);

// Blinking interval
const unsigned long BLINK_INTERVAL = 250;

// Create a LedControl instance
LedControl lc = LedControl(DIN, CLK, CS, 1);



// Player struct
struct Player
{
  int x;
  int y;
  bool isVisible;
  unsigned long lastBlinkTime;
} player;
struct Treasure {
  int x;
  int y;
  bool isVisible;
  unsigned long lastBlinkTime;
} treasure;
  int virtualMatrix[8][8] = {
  {0, 1, 0, 0, 0, 1, 1, 0},
  {0, 1, 0, 1, 0, 0, 0, 0},
  {0, 0, 0, 1, 0, 1, 0, 1},
  {1, 1, 0, 0, 0, 1, 0, 1},
  {0, 0, 0, 0, 0, 0, 1, 0},
  {0, 1, 1, 1, 1, 0, 1, 1},
  {0, 0, 0, 1, 0, 0, 0, 0},
  {1, 1, 0, 1, 1, 1, 1, 0}
};

void displayMatrix() {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      // Display the maze walls
      if (virtualMatrix[row][col] == 1) {
        lc.setLed(0, row, col, true); // Wall
      } else {
        lc.setLed(0, row, col, virtualMatrix[row][col]); // Open space or player/treasure
      }
    }
  }
  // Display player and treasure
  lc.setLed(0, player.x, player.y, player.isVisible);
  lc.setLed(0, treasure.x, treasure.y, treasure.isVisible);
}

// Threshold for joystick sensitivity
const int JOYSTICK_THRESHOLD = 512 / 2;

// Variable to track if the joystick was in neutral position
bool isJoystickNeutral = true;

void updatePlayerPosition() {
  int joystickX = analogRead(joystickXPin);
  int joystickY = analogRead(joystickYPin);

  // Check if joystick is in the neutral position
  bool joystickNeutral = (abs(joystickX - 512) < JOYSTICK_THRESHOLD) &&
                         (abs(joystickY - 512) < JOYSTICK_THRESHOLD);

  if (isJoystickNeutral) {
    // Store the current position in case we need to revert the move
    int previousX = player.x;
    int previousY = player.y;

    // Attempt to move the player based on joystick input
    if (joystickX < 512 - JOYSTICK_THRESHOLD) {
      player.x = max(player.x + 1, 0); // Move left
      isJoystickNeutral = false;
    } else if (joystickX > 512 + JOYSTICK_THRESHOLD) {
      player.x = min(player.x - 1, 7); // Move right
      isJoystickNeutral = false;
    }

    if (joystickY < 512 - JOYSTICK_THRESHOLD) {
      player.y = min(player.y - 1, 7); // Move down
      isJoystickNeutral = false;
    } else if (joystickY > 512 + JOYSTICK_THRESHOLD) {
      player.y = max(player.y + 1, 0); // Move up
      isJoystickNeutral = false;
    }

    // Check for wall collision (maze wall and not treasure position)
    if ((virtualMatrix[player.x][player.y] == 1) && (player.x != treasure.x || player.y != treasure.y)) {
      // If there's a wall and it's not the treasure, revert the move
      player.x = previousX;
      player.y = previousY;
    }
  } else if (joystickNeutral) {
    // Reset to allow another move only when joystick returns to neutral
    isJoystickNeutral = true;
  }
}

// Function to place the treasure at a random position
void placeTreasure() {
  treasure.x = random(0, 8); // Random X position
  treasure.y = random(0, 8); // Random Y position
  treasure.isVisible = true;
  treasure.lastBlinkTime = millis();
}
void updateTreasureDisplay(unsigned long currentMillis) {
  const unsigned long TREASURE_BLINK_INTERVAL = 100; // Faster blink interval for treasure
  if (currentMillis - treasure.lastBlinkTime >= TREASURE_BLINK_INTERVAL) {
    treasure.isVisible = !treasure.isVisible;
    treasure.lastBlinkTime = currentMillis;
  }
  
}
void setup()
{
  randomSeed(analogRead(0));
  placeTreasure();
  lc.shutdown(0, false);
  lc.setIntensity(0, 15);
  lc.clearDisplay(0);
  // Initialize player
  player.x = 3;
  player.y = 3;
  player.isVisible = true;
  player.lastBlinkTime = millis();
 lcd.begin(16, 2); // Assuming a 16x2 LCD

  // Print a message to the LCD.
  lcd.print("Game Starting!");
  pinMode(joystickXPin, INPUT);
  pinMode(joystickYPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void runGame(){
// Game logic...
  // Update the player's blinking logic
  unsigned long currentMillis = millis();
  if (currentMillis - player.lastBlinkTime >= BLINK_INTERVAL)
  { // Blink interval (250 ms)
    player.isVisible = !player.isVisible;
    player.lastBlinkTime = currentMillis;
  }
updateTreasureDisplay(currentMillis);

  // Check for treasure collection
  if (player.x == treasure.x && player.y == treasure.y) {
    // Player collected the treasure, reposition it
    placeTreasure();
  }
  // Display the updated matrix
  displayMatrix();
  updatePlayerPosition();
}

void loop()
{
  runGame();
  delay(1);
  // Rest of your game loop...
  // (No delay needed here)
}
