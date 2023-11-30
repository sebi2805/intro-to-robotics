#include <LedControl.h>
#include <LiquidCrystal.h>

// Pins for the MAX7219 matrix
int DIN = 3;
int CLK = 4;
int CS = 5;

const int joystickXPin = A0;
const int joystickYPin = A1;
const int buttonPin = 2;
const int DEBOUNCE_DELAY = 200;
const int THRESHOLD = 512 / 2;
const int RS_PIN = 8;
const int E_PIN = 9;
const int D4_PIN = 10;
const int D5_PIN = 11;
const int D6_PIN = 12;
const int D7_PIN = 13;

int lastJoystickState = 512; // Center position
unsigned long lastMoveTime = 0;
// Initialize the LCD with the interface pins
LiquidCrystal lcd(RS_PIN, E_PIN, D4_PIN, D5_PIN, D6_PIN, D7_PIN);

byte upArrow[8] = {
    0b00100,
    0b01110,
    0b11111,
    0b00100,
    0b00100,
    0b00000,
    0b00000,
    0b00000};

// Custom character for downward arrow
byte downArrow[8] = {
    0b00000,
    0b00000,
    0b00000,
    0b00100,
    0b00100,
    0b11111,
    0b01110,
    0b00100};

// Blinking interval
const unsigned long BLINK_INTERVAL = 250;

// Create a LedControl instance
LedControl lc = LedControl(DIN, CLK, CS, 1);

String menuItems[] = {"Start Game", "Highscore", "Settings", "About", "How to Play"};
int menuItemCount = 5;
int currentSelection = 0;

void displayMenu(int currentSelection, String menuItems[], int menuItemCount)
{
  lcd.clear();

  // Determine the start index for display based on current selection
  int startIndex = (currentSelection >= menuItemCount - 1) ? menuItemCount - 2 : currentSelection;

  // Display two menu items starting from startIndex
  for (int i = startIndex; i < startIndex + 2 && i < menuItemCount; i++)
  {
    if (i == currentSelection)
    {
      // Highlight the selected item
      lcd.print(">");
    }
    else
    {
      lcd.print(" ");
    }
    lcd.print(menuItems[i]);
    lcd.setCursor(0, i - startIndex + 1); // Move to the next line
  }

  // Indicate more options above if not at the start
  if (startIndex > 0)
  {
    lcd.setCursor(15, 0); // Position for the upward arrow
    lcd.write(byte(0));   // Custom character for upward arrow
  }

  // Indicate more options below if not at the end
  if (startIndex < menuItemCount - 2)
  {
    lcd.setCursor(15, 1); // Position for the downward arrow
    lcd.write(byte(1));   // Custom character for downward arrow
  }
}

void handleMenuSelection()
{
  switch (currentSelection)
  {
  case 0:
    runGame(); // Start the game
    break;
  case 1:
    showHighScore(); // Placeholder function for showing high scores
    break;
  case 2:
    showSettings(); // Placeholder function for settings menu
    break;
  case 3:
    showAbout(); // Placeholder function for about screen
    break;
  case 4:
    showHowToPlay(); // Placeholder function for how to play screen
    break;
  }
}

void navigateMenu()
{
  int joystickValue = analogRead(JOYSTICK_Y_PIN);

  // Check for movement only if enough time has passed since the last detected movement
  if (millis() - lastMoveTime > DEBOUNCE_DELAY)
  {
    if (joystickValue < 512 - THRESHOLD)
    {
      // Joystick moved up
      currentSelection = (currentSelection - 1 + menuItemCount) % menuItemCount;
      lastMoveTime = millis();
      displayMenu(currentSelection);
    }
    else if (joystickValue > 512 + THRESHOLD)
    {
      // Joystick moved down
      currentSelection = (currentSelection + 1) % menuItemCount;
      lastMoveTime = millis();
      displayMenu(currentSelection);
    }
  }
}
// Player struct
struct Player
{
  int x;
  int y;
  bool isVisible;
  unsigned long lastBlinkTime;
} player;
struct Treasure
{
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
    {1, 1, 0, 1, 1, 1, 1, 0}};

void displayMatrix()
{
  for (int row = 0; row < 8; row++)
  {
    for (int col = 0; col < 8; col++)
    {
      // Display the maze walls
      if (virtualMatrix[row][col] == 1)
      {
        lc.setLed(0, row, col, true); // Wall
      }
      else
      {
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

void updatePlayerPosition()
{
  int joystickX = analogRead(joystickXPin);
  int joystickY = analogRead(joystickYPin);

  // Check if joystick is in the neutral position
  bool joystickNeutral = (abs(joystickX - 512) < JOYSTICK_THRESHOLD) &&
                         (abs(joystickY - 512) < JOYSTICK_THRESHOLD);

  if (isJoystickNeutral)
  {
    // Store the current position in case we need to revert the move
    int previousX = player.x;
    int previousY = player.y;

    // Attempt to move the player based on joystick input
    if (joystickX < 512 - JOYSTICK_THRESHOLD)
    {
      player.x = max(player.x + 1, 0); // Move left
      isJoystickNeutral = false;
    }
    else if (joystickX > 512 + JOYSTICK_THRESHOLD)
    {
      player.x = min(player.x - 1, 7); // Move right
      isJoystickNeutral = false;
    }

    if (joystickY < 512 - JOYSTICK_THRESHOLD)
    {
      player.y = min(player.y - 1, 7); // Move down
      isJoystickNeutral = false;
    }
    else if (joystickY > 512 + JOYSTICK_THRESHOLD)
    {
      player.y = max(player.y + 1, 0); // Move up
      isJoystickNeutral = false;
    }

    // Check for wall collision (maze wall and not treasure position)
    if ((virtualMatrix[player.x][player.y] == 1) && (player.x != treasure.x || player.y != treasure.y))
    {
      // If there's a wall and it's not the treasure, revert the move
      player.x = previousX;
      player.y = previousY;
    }
  }
  else if (joystickNeutral)
  {
    // Reset to allow another move only when joystick returns to neutral
    isJoystickNeutral = true;
  }
}

// Function to place the treasure at a random position
void placeTreasure()
{
  treasure.x = random(0, 8); // Random X position
  treasure.y = random(0, 8); // Random Y position
  treasure.isVisible = true;
  treasure.lastBlinkTime = millis();
}
void updateTreasureDisplay(unsigned long currentMillis)
{
  const unsigned long TREASURE_BLINK_INTERVAL = 100; // Faster blink interval for treasure
  if (currentMillis - treasure.lastBlinkTime >= TREASURE_BLINK_INTERVAL)
  {
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
  lcd.createChar(0, upArrow);
  lcd.createChar(1, downArrow);
  // Print a message to the LCD.
  pinMode(joystickXPin, INPUT);
  pinMode(joystickYPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void runGame()
{
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
  if (player.x == treasure.x && player.y == treasure.y)
  {
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
