#include <LedControl.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

// Pins for the MAX7219 matrix
int DIN = 3;
int CLK = 4;
int CS = 5;

#define EEPROM_START_ADDRESS 0
#define MAX_HIGH_SCORES 3

const int joystickXPin = A0;
const int joystickYPin = A1;
const int buttonPin = 2;

const int JOYSTICK_THRESHOLD = 512 / 2;
const int DEBOUNCE_DELAY = 200;

const int brightnessLcdPin = 9;
const int RS_PIN = 8;
const int E_PIN = 7;
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
#define SETTINGS_START_ADDRESS (EEPROM_START_ADDRESS + sizeof(HighScoresEntry) * MAX_HIGH_SCORES)

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

enum SettingsState
{
  SETTINGS_MENU,
  LCD_BRIGHTNESS,
  MATRIX_BRIGHTNESS,
  SOUND
};

struct MenuItem
{
  String name;
  void (*function)();
};
SettingsState currentStateSettings = SETTINGS_MENU;
void goBackSettings()
{
  currentStateSettings = SETTINGS_MENU;
};
ProgramState currentState = MENU;

// Assuming brightness is an integer

int lcdBrightness = 255;     // Starting brightness value
int brightnessSelection = 0; // 0 for adjusting brightness, 1 for 'Save', 2 for 'Cancel'

void displayLCDBrightnessMenu()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Brightness: ");
  lcd.print(lcdBrightness);

  lcd.setCursor(0, 1);
  if (brightnessSelection == 1)
  {
    lcd.print(">Save  Cancel");
  }
  else if (brightnessSelection == 2)
  {
    lcd.print(" Save >Cancel");
  }
  else
  {
    lcd.print("Save  Cancel");
  }
}

void updateLCDBrightness()
{
  int joystickY = analogRead(joystickYPin);

  if (joystickY < JOYSTICK_THRESHOLD)
  {
    // Joystick moved up
    if (brightnessSelection == 0)
    {
      lcdBrightness = min(lcdBrightness + 1, 255); // Increase brightness
    }
    else
    {
      brightnessSelection--; // Move selection up
    }
  }
  else if (joystickY > 1023 - JOYSTICK_THRESHOLD)
  {
    // Joystick moved down
    if (brightnessSelection == 0)
    {
      lcdBrightness = max(lcdBrightness - 1, 0); // Decrease brightness
    }
    else
    {
      brightnessSelection++; // Move selection down
    }
  }

  brightnessSelection = constrain(brightnessSelection, 0, 2); // Keep within bounds
  displayLCDBrightnessMenu();                                 // Update the display
}

void handleLCDBrightnessSelection()
{
  if (joystickButtonPressed())
  {
    if (brightnessSelection == 1)
    {
      // Save brightness to EEPROM
      EEPROM.put(SETTINGS_START_ADDRESS, lcdBrightness);
      // Optionally, apply the brightness setting to the LCD
    }
    else if (brightnessSelection == 2)
    {
      // Cancel: Reload the brightness from EEPROM
      EEPROM.get(SETTINGS_START_ADDRESS, lcdBrightness);
    }

    // Go back to the previous menu or main settings menu
    goBackSettings();
  }
}
int matrixBrightness = 100; // Default value

void lcdBrightnessControl()
{
  currentStateSettings = LCD_BRIGHTNESS;
  EEPROM.get(SETTINGS_START_ADDRESS, lcdBrightness);
}
void matrixBrightnessControl()
{
  currentStateSettings = MATRIX_BRIGHTNESS;
  EEPROM.get(SETTINGS_START_ADDRESS + sizeof(int), matrixBrightness);
}
void displayLCDBrightness()
{
  updateLCDBrightness();
  handleLCDBrightnessSelection();
}
void soundControl()
{
  currentStateSettings = SOUND;
};
void goBack()
{
  currentState = MENU;
}; // For additional game or system settings
const String aboutText = "Game: Space Adventure - By: DevTeam - GitHub: @DevTeam";

MenuItem settingsMenu[] = {
    {"LCD Brightness", lcdBrightnessControl},
    {"Matrix Brightness", matrixBrightnessControl},
    {"Sound", soundControl},
    {"Go back", goBack}};
int settingsMenuItemCount = sizeof(settingsMenu) / sizeof(MenuItem);
int settingsCurrentSelection = 0;

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
void displayGenericMenu(const MenuItem menuItems[], int menuItemCount, int &currentSelection, int displayCount)
{
  static int lastStartIndex = -1;
  static int lastSelection = -1;
  int startIndex = (currentSelection > menuItemCount - displayCount) ? menuItemCount - displayCount : currentSelection;
  if (startIndex < 0)
    startIndex = 0;

  if (startIndex != lastStartIndex || currentSelection != lastSelection)
  {
    lcd.clear();
    for (int i = 0; i < displayCount && (startIndex + i) < menuItemCount; i++)
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
      lcd.print(menuItems[startIndex + i].name);
    }

    // Upward arrow
    if (startIndex > 0)
    {
      lcd.setCursor(15, 0);
      lcd.write(byte(0));
    }

    // Downward arrow
    if (startIndex < menuItemCount - displayCount)
    {
      lcd.setCursor(15, 1);
      lcd.write(byte(1));
    }

    lastStartIndex = startIndex;
    lastSelection = currentSelection;
  }
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

void applyLCDBrightness()
{
  int pwmValue = map(lcdBrightness, 0, 100, 0, 255); // Assuming lcdBrightness is from 0 to 100
  analogWrite(brightnessLcdPin, pwmValue);
}
void saveLCDBrightness()
{
  EEPROM.put(SETTINGS_START_ADDRESS, lcdBrightness);
  applyLCDBrightness(); // Apply and save the brightness
}

void loadLCDBrightness()
{
  EEPROM.get(SETTINGS_START_ADDRESS, lcdBrightness);
  applyLCDBrightness(); // Apply the loaded brightness
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
  pinMode(brightnessLcdPin, OUTPUT);
  loadLCDBrightness();
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
void updateMenuNavigation(int &currentSelection, const int menuItemCount)
{
  const unsigned long moveDelay = 200;
  static unsigned long lastMoveTime = 0;
  int joystickY = analogRead(joystickYPin);

  if (millis() - lastMoveTime > moveDelay)
  {
    int direction = 1; // Reverses the logic if needed

    if (joystickY > 1023 - JOYSTICK_THRESHOLD)
    {
      currentSelection -= direction;
      if (currentSelection < 0)
      {
        currentSelection = menuItemCount - 1;
      }
      else if (currentSelection >= menuItemCount)
      {
        currentSelection = 0;
      }
      lastMoveTime = millis();
    }
    else if (joystickY < JOYSTICK_THRESHOLD)
    {
      currentSelection += direction;
      if (currentSelection < 0)
      {
        currentSelection = menuItemCount - 1;
      }
      else if (currentSelection >= menuItemCount)
      {
        currentSelection = 0;
      }
      lastMoveTime = millis();
    }
  }
}

const String howToPlayText = "Move joystick to navigate. Button to select. Avoid obstacles, collect items. Have fun!";
void scrollText(const String &text)
{
  static unsigned long lastScrollTime = 0;
  const unsigned long scrollDelay = 500; // Delay between scroll steps, in milliseconds
  static int scrollPosition = 0;

  if (millis() - lastScrollTime > scrollDelay)
  {
    lcd.clear();
    lcd.setCursor(0, 0);

    // Display a portion of the text starting from scrollPosition
    for (int i = 0; i < 16; i++) // Assuming a 16x2 LCD
    {
      if (scrollPosition + i < text.length())
      {
        lcd.print(text[scrollPosition + i]);
      }
    }

    // Update scrollPosition for the next frame
    scrollPosition++;
    if (scrollPosition >= text.length())
    {
      scrollPosition = 0; // Reset to start after reaching the end
    }

    lastScrollTime = millis();
  }
}

void settingsMenuDisplay()
{
  switch (currentStateSettings)
  {
  case SETTINGS_MENU:
    updateMenuNavigation(settingsCurrentSelection, settingsMenuItemCount);
    displayGenericMenu(settingsMenu, settingsMenuItemCount, settingsCurrentSelection, 2);
    if (joystickButtonPressed())
    {
      settingsMenu[settingsCurrentSelection].function();
    }
    break;
  case LCD_BRIGHTNESS:
    displayLCDBrightness();
    break;
  case MATRIX_BRIGHTNESS:
    // Logic for adjusting matrix brightness
    if (joystickButtonPressed())
    {
      // Perform matrix brightness adjustment
    }
    break;
  case SOUND:
    // Logic for adjusting sound settings
    if (joystickButtonPressed())
    {
      // Perform sound settings adjustment
    }
    break;
  }
}
void loop()
{

  switch (currentState)
  {
  case MENU:
    updateMenuNavigation(currentSelection, menuItemCount);
    displayGenericMenu(mainMenu, menuItemCount, currentSelection, 2);
    if (joystickButtonPressed())
    {
      mainMenu[currentSelection].function();
    }
    break;
  case GAME:
    runGame(); // Placeholder function for running the game
    break;
  case HIGHSCORES:
    updateMenuNavigation(highScoreStartIndex, MAX_HIGH_SCORES);
    displayHighScores();
    if (joystickButtonPressed())
    {
      currentState = MENU;
    }
    break;
  case SETTINGS:
    settingsMenuDisplay();
    break;
  case ABOUT:
    scrollText(aboutText);
    if (joystickButtonPressed())
    {
      currentState = MENU; // Return to main menu on button press
    }
    break;
  case HOW_TO_PLAY:
    scrollText(howToPlayText);
    if (joystickButtonPressed())
    {
      currentState = MENU; // Return to main menu on button press
    }
    break;
  }
  delay(1);
  // Rest of your game loop...
  // (No delay needed here)
}
