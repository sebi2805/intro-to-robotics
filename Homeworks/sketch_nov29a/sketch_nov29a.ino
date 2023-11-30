#include <LedControl.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
// Pins for the MAX7219 matrix
int DIN = 3;
int CLK = 4;
int CS = 5;
#define EEPROM_START_ADDRESS 0 // Starting address in EEPROM
#define MAX_HIGH_SCORES 3
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

int currentSelection = 0;
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
struct HighScoresEntry
{
  String playerName;
  int score;
};
// Create a LedControl instance
LedControl lc = LedControl(DIN, CLK, CS, 1);
enum ProgramState
{
  MENU,
  GAME,
  SETTINGS,
  HIGHSCORES,
  ABOUT,
  HOW_TO_PLAY
};

ProgramState currentState = MENU;

struct MenuItem
{
  String name;
  void (*function)();
};

// Placeholder functions for menu actions
void startGame()
{
  currentState = GAME;
  // Initialize game variables...
}

void showSettings()
{
  currentState = SETTINGS;
  // Display and handle settings
}

void showHighScore()
{
  currentState = HIGHSCORES;
  // Display high scores
}

void aboutGame()
{
  currentState = ABOUT;
  // Display about game information
}

void howToPlay()
{
  currentState = HOW_TO_PLAY;
  // Display how to play information
}

MenuItem mainMenu[] = {
    {"Start Game", startGame},
    {"High Score", showHighScore},
    {"Settings", showSettings},
    {"About", aboutGame},
    {"How To Play", howToPlay}};

int menuItemCount = 5;

void displayMenu()
{
  static int lastStartIndex = -1;
  static int lastSelection = -1;
  int startIndex = (currentSelection > menuItemCount - 2) ? menuItemCount - 2 : currentSelection;
  if (startIndex < 0)
    startIndex = 0;

  // Only clear and update the display if there are changes
  if (startIndex != lastStartIndex || currentSelection != lastSelection)
  {
    lcd.clear();
    for (int i = 0; i < 2 && (startIndex + i) < menuItemCount; i++)
    {
      lcd.setCursor(0, i);
      if (startIndex + i == currentSelection)
      {
        lcd.print(">");
      }
      else
      {
        lcd.print(" ");
      }
      lcd.print(mainMenu[startIndex + i].name);
    }

    if (startIndex > 0)
    {
      lcd.setCursor(15, 0);
      lcd.write(byte(0)); // Upward arrow
    }

    if (startIndex < menuItemCount - 2)
    {
      lcd.setCursor(15, 1);
      lcd.write(byte(1)); // Downward arrow
    }

    lastStartIndex = startIndex;
    lastSelection = currentSelection;
  }
}

void updateMenuSelection()
{
  static unsigned long lastMoveTime = 0;
  const unsigned long moveDelay = 200; // 200 milliseconds delay for debouncing

  int joystickY = analogRead(joystickYPin);

  // Check if enough time has passed since the last registered move
  if (millis() - lastMoveTime > moveDelay)
  {
    if (joystickY > 1023 - THRESHOLD)
    {
      // Joystick moved down (reversed to act as up)
      if (currentSelection > 0)
      {
        currentSelection--;
      }
      else
      {
        currentSelection = menuItemCount - 1; // Wrap around to the last item
      }
      lastMoveTime = millis();
    }
    else if (joystickY < THRESHOLD)
    {
      // Joystick moved up (reversed to act as down)
      if (currentSelection < menuItemCount - 1)
      {
        currentSelection++;
      }
      else
      {
        currentSelection = 0; // Wrap around to the first item
      }
      lastMoveTime = millis();
    }
  }
}

bool joystickButtonPressed()
{
  static unsigned long lastPress = 0;
  if (digitalRead(buttonPin) == LOW)
  {
    if (millis() - lastPress > DEBOUNCE_DELAY)
    {
      lastPress = millis();
      return true;
    }
  }
  return false;
}
// void navigateMenu()
// {
//   int joystickValue = analogRead(joystickYPin);

//   // Check for movement only if enough time has passed since the last detected movement
//   if (millis() - lastMoveTime > DEBOUNCE_DELAY)
//   {
//     if (joystickValue < 512 - THRESHOLD)
//     {
//       // Joystick moved up
//       currentSelection = (currentSelection - 1 + menuItemCount) % menuItemCount;
//       lastMoveTime = millis();
//       displayMenu(currentSelection);
//     }
//     else if (joystickValue > 512 + THRESHOLD)
//     {
//       // Joystick moved down
//       currentSelection = (currentSelection + 1) % menuItemCount;
//       lastMoveTime = millis();
//       displayMenu(currentSelection);
//     }
//   }
// }
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

HighScoresEntry highScores[MAX_HIGH_SCORES];
void saveHighScores(HighScoresEntry scores[], int count)
{
  int address = EEPROM_START_ADDRESS;
  for (int i = 0; i < count; i++)
  {
    EEPROM.put(address, scores[i]);
    address += sizeof(HighScoresEntry);
  }
}
void loadHighScores()
{
  int address = EEPROM_START_ADDRESS;
  for (int i = 0; i < MAX_HIGH_SCORES; i++)
  {
    EEPROM.get(address, highScores[i]);
    address += sizeof(HighScoresEntry);
  }
  // Add data integrity check if implemented
}
void updateHighScores(int newScore, String playerName)
{
  bool scoreUpdated = false;
  for (int i = 0; i < MAX_HIGH_SCORES; i++)
  {
    if (newScore > highScores[i].score)
    {
      // Shift down lower scores
      for (int j = MAX_HIGH_SCORES - 1; j > i; j--)
      {
        highScores[j] = highScores[j - 1];
      }
      // Insert the new score
      highScores[i] = {playerName, newScore};
      scoreUpdated = true;
      break;
    }
  }
  if (scoreUpdated)
  {
    saveHighScores(highScores, MAX_HIGH_SCORES);
  }
}
// Global variables for high score display
int highScoreStartIndex = 0;
const int highScoreDisplayCount = 2; // Number of high scores to display at a time

void displayHighScores()
{
  static int lastStartIndex = -1;

  // Only clear and update the display if there are changes
  if (highScoreStartIndex != lastStartIndex)
  {
    lcd.clear();
    for (int i = 0; i < highScoreDisplayCount; i++)
    {
      int scoreIndex = highScoreStartIndex + i;
      if (scoreIndex < MAX_HIGH_SCORES)
      {
        lcd.setCursor(0, i);
        lcd.print(scoreIndex + 1);
        lcd.print(". ");
        lcd.print(highScores[scoreIndex].playerName);
        lcd.print(" - ");
        lcd.print(highScores[scoreIndex].score);
      }
    }

    // Upward arrow
    if (highScoreStartIndex > 0)
    {
      lcd.setCursor(15, 0);
      lcd.write(byte(0));
    }

    // Downward arrow
    if (highScoreStartIndex < MAX_HIGH_SCORES - highScoreDisplayCount)
    {
      lcd.setCursor(15, 1);
      lcd.write(byte(1));
    }

    lastStartIndex = highScoreStartIndex;
  }
}

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
  // reversed until further movements of the joystick
  int joystickY = analogRead(joystickXPin);
  int joystickX = analogRead(joystickYPin);

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
  int newTreasureX = random(0, 8); // Random X position
  int newTreasureY = random(0, 8); // Random Y position
  while (virtualMatrix[newTreasureX][newTreasureY] == 1)
  {
    // If the new position is a wall, try again
    newTreasureX = random(0, 8);
    newTreasureY = random(0, 8);
  }
  treasure.x = newTreasureX; // Random X position
  treasure.y = newTreasureY; // Random Y position
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

void updateHighScoreDisplay()
{
  static unsigned long lastMoveTime = 0;
  const unsigned long moveDelay = 200; // Delay for debouncing, in milliseconds

  // Read the joystick's Y-axis value
  int joystickY = analogRead(joystickYPin);

  // Ensure enough time has passed since the last registered move (debouncing)
  if (millis() - lastMoveTime > moveDelay)
  {
    if (joystickY < THRESHOLD)
    {
      // Joystick moved up
      if (highScoreStartIndex < MAX_HIGH_SCORES - highScoreDisplayCount)
      {
        highScoreStartIndex++;
        lastMoveTime = millis(); // Update the last move time
      }
    }
    else if (joystickY > 1023 - THRESHOLD)
    {
      if (highScoreStartIndex > 0)
      {
        highScoreStartIndex--;
        lastMoveTime = millis(); // Update the last move time
      }
      // Joystick moved down
    }
  }
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

  switch (currentState)
  {
  case MENU:
    updateMenuSelection();
    displayMenu();
    if (joystickButtonPressed())
    {
      mainMenu[currentSelection].function();
    }
    break;
  case GAME:
    runGame(); // Placeholder function for running the game
    break;
  case HIGHSCORES:
    updateHighScoreDisplay();
    displayHighScores();
    if (joystickButtonPressed())
    {
      currentState = MENU;
    }
    break;
  case SETTINGS:
    showSettings(); // Placeholder function for settings
    break;
  }
  delay(1);
  // Rest of your game loop...
  // (No delay needed here)
}
