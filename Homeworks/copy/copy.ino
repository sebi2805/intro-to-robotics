#include <LedControl.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

// ------------------------------
// Hardware Pin Configuration
// ------------------------------
int DIN = 3;
int CLK = 4;
int CS = 5;
int joystickXPin = A0;
int joystickYPin = A1;
int buttonPin = 2;
int brightnessLcdPin = 9;
int RS_PIN = 8;
int E_PIN = 7;
int D4_PIN = 10;
int D5_PIN = 11;
int D6_PIN = 12;
int D7_PIN = 13;
const int BUZZER_PIN = 6; // Buzzer connected to pin 6

LedControl lc = LedControl(DIN, CLK, CS, 1);
LiquidCrystal lcd(RS_PIN, E_PIN, D4_PIN, D5_PIN, D6_PIN, D7_PIN);
// Structures
struct HighScoresEntry
{
    char playerName[4]; // 3 characters + 1 for the null terminator
    int score;
};

struct MenuItem
{
    String name;
    void (*function)();
};

struct Player
{
    int x;
    int y;
    bool isVisible;
    unsigned long lastBlinkTime;
    int points;
} player;

struct Treasure
{
    int x;
    int y;
    bool isVisible;
    unsigned long lastBlinkTime;
    unsigned long spawnTime; // Time when the treasure is placed
} treasure;

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
enum SoundType
{
    MenuNavigationSound,
    ButtonPressSound,
    // Add more sound types as needed
};

enum MenuOptions
{
    MENU_YES = 0,
    MENU_NO = 1,
    MENU_CANCEL = 2,
    MENU_SAVE = 3
};
enum EndGameOption
{
    RETRY = 0,
    MAIN_MENU = 1,
    UNKNOWN = 2
};

// ------------------------------
// Constants and Global Variables
// ------------------------------
#define EEPROM_START_ADDRESS 0
#define MAX_HIGH_SCORES 3
#define LCD_BRIGHTNESS_ADDRESS (EEPROM_START_ADDRESS + sizeof(HighScoresEntry) * MAX_HIGH_SCORES)
#define MATRIX_BRIGHTNESS_ADDRESS (LCD_BRIGHTNESS_ADDRESS + sizeof(int))
#define SOUND_SETTING_ADDRESS (MATRIX_BRIGHTNESS_ADDRESS + sizeof(int))
#define JOYSTICK_THRESHOLD 512 / 2
#define DEBOUNCE_DELAY 200
#define BLINK_INTERVAL 250

// ------------------------------
// Text Constants
const String aboutText = "Game: Space Adventure - By: DevTeam - GitHub: @DevTeam";
const String howToPlayText = "Move joystick to navigate. Button to select. Avoid obstacles, collect items. Have fun!";
const char characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
const int numCharacters = sizeof(characters) - 1; // -1 for the null terminator

// Custom characters
byte upArrow[8] = {
    0b00100,
    0b01110,
    0b11111,
    0b00100,
    0b00100,
    0b00000,
    0b00000,
    0b00000};

byte downArrow[8] = {
    0b00000,
    0b00000,
    0b00000,
    0b00100,
    0b00100,
    0b11111,
    0b01110,
    0b00100};

byte rightArrow[8] = {
    0b00000,
    0b00100,
    0b00010,
    0b11111,
    0b00010,
    0b00100,
    0b00000,
    0b00000};

int virtualMatrix[8][8] = {
    {0, 1, 0, 0, 0, 1, 1, 0},
    {0, 1, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 1, 0, 1},
    {1, 1, 0, 0, 0, 1, 0, 1},
    {0, 0, 0, 0, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 0, 1, 1},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {1, 1, 0, 1, 1, 1, 1, 0}};

// Global Variables
int joystickCenter = 512;
int currentSelection = -1;
int settingsCurrentSelection = -1;
int lastJoystickState = joystickCenter; // Center position
unsigned long lastMoveTime = 0;
int lcdBrightness = 255;     // Starting brightness value
int brightnessSelection = 1; // 0 for adjusting brightness, 1 for 'Save', 2 for 'Cancel'
int matrixBrightness = 15;   // Default value
ProgramState currentState = MENU;
SettingsState currentStateSettings = SETTINGS_MENU;
int highScoreStartIndex = 0;
const int BUZZER_FREQUENCY = 1000; // Frequency of the buzzer sound in Hertz
const int BUZZER_DURATION = 50;
const int BUTTON_PRESS_FREQUENCY = 1500; // Frequency for button press sound
const int BUTTON_PRESS_DURATION = 100;
const int highScoreDisplayCount = 2; // Number of high scores to display at a time
bool soundEnabled = true;            // Default value, can be true or false
HighScoresEntry highScores[MAX_HIGH_SCORES];
unsigned long gameStartTime;
unsigned long timeLimit = 5000;
String playerName = "";
int playerNameCharIndex = 0;
int playerNameLength = 0;

// ------------------------------
// Function Declarations
// ------------------------------
void goBackSettings();
void displayLCDBrightnessMenu();
void updateLCDBrightness();
void handleLCDBrightnessSelection();
void lcdBrightnessControl();
void matrixBrightnessControl();
void displayLCDBrightness();
void soundControl();
void goBack();
void startGame();
void showSettings();
void showHighScore();
void aboutGame();
void howToPlay();
bool joystickButtonPressed();
void saveHighScores(HighScoresEntry scores[], int count);
void loadHighScores();
void displayGenericMenu(const MenuItem menuItems[], int menuItemCount, int &currentSelection, int displayCount);
void updateHighScores();
void displayHighScores();
void displayMatrix();
void updatePlayerPosition();
void placeTreasure();
void updateTreasureDisplay(unsigned long currentMillis);
void applyLCDBrightness();
void saveLCDBrightness();
void loadLCDBrightness();
void runGame();
void endGame();
void restartGame();
void updateMenuNavigation(int &currentSelection, const int menuItemCount);
void scrollText(const String &text);
void settingsMenuDisplay();
void displayMatrixBrightnessMenu();
void updateMatrixBrightness();
void applyMatrixBrightness();
void previewMatrixBrightness();
void saveMatrixBrightness();
void loadMatrixBrightness();
void displayMatrixBrightness();
void handleMatrixBrightnessSelection();
void playSound(SoundType sound);
void toggleSoundSettings();
void saveSoundSettings();
void loadSoundSettings();
void displayEndGameMessage();
void displayEndGameMenu(EndGameOption &lastEndGameOption, EndGameOption endGameOption);
void clearMatrix();
EndGameOption handleEndGameControls(EndGameOption &endGameOption, bool &hasEnteredEndGame, unsigned long displayEndTime);
bool enterPlayerName(bool reset = false);
// ------------------------------
MenuItem settingsMenu[] = {
    {"LCD Brightness", lcdBrightnessControl},
    {"Matrix Brightness", matrixBrightnessControl},
    {"Sound", soundControl},
    {"Go back", goBack}};
int settingsMenuItemCount = sizeof(settingsMenu) / sizeof(MenuItem);

// ------------------------------

MenuItem mainMenu[] = {
    {"Start Game", startGame},
    {"High Score", showHighScore},
    {"Settings", showSettings},
    {"About", aboutGame},
    {"How To Play", howToPlay}};

int menuItemCount = sizeof(mainMenu) / sizeof(MenuItem);
// ------------------------------
// Main Setup and Loop
// ------------------------------
void setup()
{
    // Seed the random number generator
    randomSeed(analogRead(0));

    // Initialize the LED matrix
    lc.shutdown(0, false); // Wake up display
    lc.clearDisplay(0);    // Clear display register

    // Initialize the LCD
    lcd.begin(16, 2);              // Assuming a 16x2 LCD
    lcd.createChar(0, upArrow);    // Create custom up arrow character
    lcd.createChar(1, downArrow);  // Create custom down arrow character
    lcd.createChar(2, rightArrow); // Create custom right arrow character
    // Initialize player settings
    player.x = 3;
    player.y = 3;
    player.points = 0;
    player.isVisible = true;
    player.lastBlinkTime = millis();

    // Initialize hardware pins
    pinMode(joystickXPin, INPUT);      // Joystick X-axis
    pinMode(joystickYPin, INPUT);      // Joystick Y-axis
    pinMode(buttonPin, INPUT_PULLUP);  // Joystick button
    pinMode(brightnessLcdPin, OUTPUT); // LCD brightness control

    // Load settings
    loadMatrixBrightness(); // Load matrix brightness from EEPROM
    loadLCDBrightness();    // Load LCD brightness from EEPROM
    loadSoundSettings();
    loadHighScores();
    // Game-specific setup
    placeTreasure(); // Place the treasure in the game
    Serial.begin(9600);
    // formatEEPROM();
}

void formatEEPROM()
{
    HighScoresEntry defaultEntry;
    memset(defaultEntry.playerName, 0, sizeof(defaultEntry.playerName)); // Set all characters to 0 (null)
    defaultEntry.score = 0;                                              // Default score

    for (int i = 0; i < MAX_HIGH_SCORES; i++)
    {
        EEPROM.put(EEPROM_START_ADDRESS + i * sizeof(HighScoresEntry), defaultEntry);
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
        break;
    case HOW_TO_PLAY:
        scrollText(howToPlayText);
        break;
    }
    delay(1);
}

// ------------------------------
// Function Implementations
void goBackSettings()
{
    settingsCurrentSelection = -1;
    currentStateSettings = SETTINGS_MENU;
};
////////////////////////////////////////////////////////////////
bool joystickButtonPressed()
{
    static unsigned long lastPress = 0;
    if (digitalRead(buttonPin) == LOW)
    {
        if (millis() - lastPress > DEBOUNCE_DELAY)
        {
            lastPress = millis();
            playSound(ButtonPressSound); // Play a different sound
            return true;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////
void displayLCDBrightnessMenu()
{
    static int lastLcdBrightness = -1;
    static int lastBrightnessSelection = -1;

    // Clear the LCD and force update on the first call
    if (lastLcdBrightness == -1 && lastBrightnessSelection == -1)
    {
        lcd.clear();
    }

    // Update the brightness value if it has changed
    if (lastLcdBrightness != lcdBrightness)
    {
        lcd.setCursor(0, 0);
        lcd.print("Brightness:     "); // Clear the previous value
        lcd.setCursor(12, 0);
        lcd.print(lcdBrightness);
        lastLcdBrightness = lcdBrightness;
    }

    // Update the selection options if they have changed
    if (lastBrightnessSelection != brightnessSelection)
    {
        lcd.setCursor(0, 1);
        if (brightnessSelection == 1)
        {
            lcd.print((char)2); // Right arrow
            lcd.print("Save  Cancel");
        }
        else if (brightnessSelection == 2)
        {
            lcd.print("Save  ");
            lcd.print((char)2); // Right arrow
            lcd.print("Cancel");
        }
        else
        {
            lcd.print("Save  Cancel");
        }
        lastBrightnessSelection = brightnessSelection;
    }
}

void updateLCDBrightness()
{
    int joystickY = analogRead(joystickYPin);
    int joystickX = analogRead(joystickXPin);
    // Check if the joystick is moved significantly from the center
    if (abs(joystickY - joystickCenter) > JOYSTICK_THRESHOLD)
    {
        if (joystickY < joystickCenter)
        {
            // Joystick moved up
            lcdBrightness = max(lcdBrightness - 1, 0); // Increase brightness
        }
        else
        {
            // Joystick moved down
            lcdBrightness = min(lcdBrightness + 1, 255); // Decrease brightness
        }
        applyLCDBrightness(); // Apply the brightness to the LCD immediately
    }
    // Adjust selection based on left/right movement
    else if (joystickX < JOYSTICK_THRESHOLD)
    {
        brightnessSelection = 1; // Select "Save"
    }
    else if (joystickX > 1023 - JOYSTICK_THRESHOLD)
    {
        brightnessSelection = 2; // Select "Cancel"
    }

    // Update the display
    displayLCDBrightnessMenu();
}

void handleLCDBrightnessSelection()
{

    if (joystickButtonPressed())
    {
        if (brightnessSelection == 1)
        {
            // Save brightness to EEPROM
            EEPROM.put(LCD_BRIGHTNESS_ADDRESS, lcdBrightness);
            applyLCDBrightness();
        }
        else if (brightnessSelection == 2)
        {
            // Cancel: Reload the brightness from EEPROM
            EEPROM.get(LCD_BRIGHTNESS_ADDRESS, lcdBrightness);
            applyLCDBrightness();
        }

        // Reset selection and go back to the previous menu
        brightnessSelection = 1;
        goBackSettings();
    }
}

void lcdBrightnessControl()
{
    currentStateSettings = LCD_BRIGHTNESS;
    EEPROM.get(LCD_BRIGHTNESS_ADDRESS, lcdBrightness);
}

void displayLCDBrightness()
{

    handleLCDBrightnessSelection();
    updateLCDBrightness();
}
void applyLCDBrightness()
{
    int pwmValue = map(lcdBrightness, 0, 255, 0, 255);
    analogWrite(brightnessLcdPin, pwmValue);
}
void saveLCDBrightness()
{
    EEPROM.put(LCD_BRIGHTNESS_ADDRESS, lcdBrightness);
    applyLCDBrightness(); // Apply and save the brightness
}

void loadLCDBrightness()
{
    EEPROM.get(LCD_BRIGHTNESS_ADDRESS, lcdBrightness);
    applyLCDBrightness();
}
////////////////////////////////////////////////////////////////
void displayMatrixBrightnessMenu()
{
    static int lastMatrixBrightness = -1;
    static int lastBrightnessSelection = -1;

    // Clear the LCD and force update on the first call
    if (lastMatrixBrightness == -1 && lastBrightnessSelection == -1)
    {
        lcd.clear();
        lastMatrixBrightness = matrixBrightness - 1;       // Force update
        lastBrightnessSelection = brightnessSelection - 1; // Force update
    }

    // Update the brightness value if it has changed
    if (lastMatrixBrightness != matrixBrightness)
    {
        lcd.setCursor(0, 0);
        lcd.print("Matrix Bright: ");
        lcd.setCursor(14, 0);

        if (matrixBrightness < 10)
        {
            lcd.print('0'); // Print leading zero for single-digit numbers
        }
        lcd.print(matrixBrightness);

        lastMatrixBrightness = matrixBrightness;
    }

    // Update the selection options if they have changed
    if (lastBrightnessSelection != brightnessSelection)
    {
        lcd.setCursor(0, 1);
        lcd.print(brightnessSelection == 1 ? (char)2 : ' '); // Right arrow for 'Save'
        lcd.print("Save  ");
        lcd.print(brightnessSelection == 2 ? (char)2 : ' '); // Right arrow for 'Cancel'
        lcd.print("Cancel");
        lastBrightnessSelection = brightnessSelection;
    }
}
void clearMatrix()
{
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            lc.setLed(0, row, col, false); // Turn off the LED at row, col
        }
    }
}
void displayMatrixBrightness()
{
    handleMatrixBrightnessSelection();
    updateMatrixBrightness();
};

void handleMatrixBrightnessSelection()
{
    static bool hasEntered = false;
    if (!hasEntered)
    {
        previewMatrixBrightness();
        hasEntered = true;
    }

    if (joystickButtonPressed()) // Assuming you have a function to check if the joystick button is pressed
    {
        if (brightnessSelection == 1) // 'Save' selected
        {
            saveMatrixBrightness();
            applyMatrixBrightness(); // Apply and save the brightness setting
            hasEntered = false;
        }
        else if (brightnessSelection == 2) // 'Cancel' selected
        {
            loadMatrixBrightness();  // Reload the brightness from EEPROM
            applyMatrixBrightness(); // Apply the reloaded brightness setting
            hasEntered = false;
        }

        // Reset selection and possibly go back to the previous menu
        brightnessSelection = 1;
        // Other actions to return to the previous menu or state
    }
}

void updateMatrixBrightness()
{
    static unsigned long lastUpdateTime = 0;
    const unsigned long updateDelay = 200; // Delay in milliseconds

    int joystickY = analogRead(joystickYPin);
    int joystickX = analogRead(joystickXPin);

    if (millis() - lastUpdateTime > updateDelay)
    {
        if (abs(joystickY - 512) > JOYSTICK_THRESHOLD)
        {
            matrixBrightness = constrain(matrixBrightness + (joystickY < 512 ? -1 : 1), 0, 15);
            applyMatrixBrightness();
            previewMatrixBrightness();
            lastUpdateTime = millis(); // Update the last update time
        }
        else if (joystickX < JOYSTICK_THRESHOLD)
        {
            brightnessSelection = 1; // Select "Save"
            lastUpdateTime = millis();
        }
        else if (joystickX > 1023 - JOYSTICK_THRESHOLD)
        {
            brightnessSelection = 2; // Select "Cancel"
            lastUpdateTime = millis();
        }

        displayMatrixBrightnessMenu();
    }
}

void applyMatrixBrightness()
{
    lc.setIntensity(0, matrixBrightness);
}
void saveMatrixBrightness()
{
    EEPROM.put(MATRIX_BRIGHTNESS_ADDRESS, matrixBrightness);
    lc.clearDisplay(0);
    goBackSettings();
}

void loadMatrixBrightness()
{
    EEPROM.get(MATRIX_BRIGHTNESS_ADDRESS, matrixBrightness);
    lc.setIntensity(0, matrixBrightness);
    lc.clearDisplay(0);
    goBackSettings();
}
void previewMatrixBrightness()
{
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            lc.setLed(0, row, col, true); // Turn on all LEDs
        }
    }
}

////////////////////////////////////////////////////////////////
void matrixBrightnessControl()
{
    currentStateSettings = MATRIX_BRIGHTNESS;
    EEPROM.get(MATRIX_BRIGHTNESS_ADDRESS, matrixBrightness);
}
////////////////////////////////////////////////////////////////

void soundControl()
{
    currentStateSettings = SOUND;
};

////////////////////////////////////////////////////////////////
void goBack()
{
    currentSelection = -1;
    currentState = MENU;
};
void startGame()
{
    currentState = GAME;
    gameStartTime = millis();
}

void showSettings()
{
    currentState = SETTINGS;
}

void showHighScore()
{
    currentState = HIGHSCORES;
}

void aboutGame()
{
    currentState = ABOUT;
}

void howToPlay()
{
    currentState = HOW_TO_PLAY;
}
////////////////////////////////////////////////////////////////
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
}
////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////
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

void updatePlayerPosition()
{
    static bool isJoystickNeutral = true;

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
////////////////////////////////////////////////////////////////
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
    treasure.spawnTime = millis();
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
void runGame()
{
    unsigned long currentMillis = millis();
    if (currentMillis - gameStartTime > timeLimit)
    {
        endGame();
        return;
    }
    else
    {
        // Game logic...
        // Update the player's blinking logic
        unsigned long timeElapsed = currentMillis - gameStartTime;
        unsigned long timeRemaining = (timeLimit > timeElapsed) ? (timeLimit - timeElapsed) : 0;
        int minutesRemaining = timeRemaining / 60000;          // Convert milliseconds to minutes
        int secondsRemaining = (timeRemaining % 60000) / 1000; // Convert remaining milliseconds to seconds

        // Update the LCD only if necessary to avoid flickering
        static unsigned long lastLCDUpdate = 0;
        if (currentMillis - lastLCDUpdate > 500)
        { // Update every half second
            lcd.clear();

            // Display remaining time on the first row
            lcd.setCursor(0, 0);
            lcd.print("Time: ");
            lcd.print(minutesRemaining);
            lcd.print(":");
            if (secondsRemaining < 10)
            {
                lcd.print("0"); // Leading zero for single digit seconds
            }
            lcd.print(secondsRemaining);

            // Display player's points on the second row
            lcd.setCursor(0, 1);
            lcd.print("Points: ");
            lcd.print(player.points);

            lastLCDUpdate = currentMillis;
        }
        if (currentMillis - player.lastBlinkTime >= BLINK_INTERVAL)
        { // Blink interval (250 ms)
            player.isVisible = !player.isVisible;
            player.lastBlinkTime = currentMillis;
        }
        updateTreasureDisplay(currentMillis);

        // Check for treasure collection
        if (player.x == treasure.x && player.y == treasure.y)
        {
            // Calculate points based on the new formula
            unsigned long timeTaken = currentMillis - treasure.spawnTime;
            int pointsForThisTreasure = max(50 - (timeTaken / 1000), 10); // Decrease points every second, min 10
            player.points += pointsForThisTreasure;

            placeTreasure(); // Reposition the treasure
        }
        // Display the updated matrix
        displayMatrix();
        updatePlayerPosition();
    }
}

void displayEndGameMessage()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Game finished!");
    lcd.setCursor(0, 1);
    lcd.print("Points: ");
    lcd.print(player.points);
}
void displayEndGameMenu(EndGameOption &lastEndGameOption, EndGameOption endGameOption)
{
    // Update the display only if the option has changed
    if (lastEndGameOption != endGameOption)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(endGameOption == RETRY ? (char)2 : ' ');
        lcd.print("Retry");
        lcd.setCursor(0, 1);
        lcd.print(endGameOption == MAIN_MENU ? (char)2 : ' ');
        lcd.print("Main Menu");

        lastEndGameOption = endGameOption;
    }
}

EndGameOption handleEndGameControls(EndGameOption &endGameOption, bool &hasSelected, unsigned long displayEndTime)
{
    if (millis() >= displayEndTime)
    {
        // Read joystick input for navigation
        int joystickY = analogRead(joystickYPin);

        if (joystickY < JOYSTICK_THRESHOLD && endGameOption != RETRY)
        {
            endGameOption = RETRY;
        }
        else if (joystickY > 1023 - JOYSTICK_THRESHOLD && endGameOption != MAIN_MENU)
        {
            endGameOption = MAIN_MENU;
        }

        if (joystickButtonPressed())
        {
            hasSelected = true;   // Reset for next time entering end game
            return endGameOption; // Return the selected option
        }
    }
    return endGameOption;
}

bool enterPlayerName(bool reset = false)
{
    static bool isJoystickNeutral = true; // Flag to track if the joystick is in the neutral position
    static char lastDisplayedChar = '\0'; // Variable to track the last displayed character

    if (reset)
    {
        isJoystickNeutral = true;
        lastDisplayedChar = '\1';
        playerNameCharIndex = 0;
        // Force an initial display update
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enter Name:");
        lcd.setCursor(0, 1);
        lcd.print(playerName + characters[playerNameCharIndex]);
        return false; // Exit the function after resetting
    }

    if (playerNameLength < 3)
    {
        char currentChar = characters[playerNameCharIndex];

        // Read joystick input for character selection
        int joystickX = analogRead(joystickXPin);

        // Check if joystick is in the neutral position
        bool joystickNeutral = abs(joystickX - joystickCenter) < JOYSTICK_THRESHOLD;

        if (joystickNeutral)
        {
            isJoystickNeutral = true;
        }
        else if (isJoystickNeutral)
        {
            if (joystickX < JOYSTICK_THRESHOLD)
            {
                playerNameCharIndex = (playerNameCharIndex > 0) ? playerNameCharIndex - 1 : numCharacters - 1;
            }
            else if (joystickX > 1023 - JOYSTICK_THRESHOLD)
            {
                playerNameCharIndex = (playerNameCharIndex < numCharacters - 1) ? playerNameCharIndex + 1 : 0;
            }
            isJoystickNeutral = false;
            currentChar = characters[playerNameCharIndex]; // Update currentChar after joystick movement
        }

        // Update the display if the current character has changed
        if (lastDisplayedChar != currentChar)
        {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Enter Name:");
            lcd.setCursor(0, 1);
            lcd.print(playerName + currentChar);
            lastDisplayedChar = currentChar;
        }

        // Confirm character with joystick button
        if (joystickButtonPressed())
        {
            playerName += currentChar;
            playerNameLength++;
            playerNameCharIndex = 0;  // Reset for next character
            isJoystickNeutral = true; // Reset joystick to neutral for next input
            lastDisplayedChar = '\0'; // Reset last displayed char

            if (playerNameLength >= 3)
            {
                playerNameLength = 0; // Reset for the next game
                return true;          // Name entry complete
            }
        }
    }
    return false; // Name entry not yet complete
}

int checkHighScore(int playerScore)
{
    loadHighScores(); // Assuming this function loads the high scores from EEPROM

    for (int i = 0; i < MAX_HIGH_SCORES; ++i)
    {
        if (playerScore > highScores[i].score)
        {
            return i; // Player's score beats the score at position i
        }
    }
    return -1; // Player's score does not beat any high score
}
void updateHighScores()
{
    int position = checkHighScore(player.points);
    if (position != -1)
    {
        // Shift lower scores down by one position
        for (int i = MAX_HIGH_SCORES - 1; i > position; --i)
        {
            highScores[i] = highScores[i - 1];
        }

        // Insert the new high score
        if (enterPlayerName())
        {
            highScores[position].score = player.points;

            // If playerName is a String, use c_str() to convert to char*
            strncpy(highScores[position].playerName, playerName.c_str(), sizeof(highScores[position].playerName));
            // Ensure null termination
            highScores[position].playerName[sizeof(highScores[position].playerName) - 1] = '\0';
        }
        playerName = "";                             // Reset player name for next game
        saveHighScores(highScores, MAX_HIGH_SCORES); // Save the updated scores to EEPROM
    }
}

void endGame()
{
    static bool hasEnteredEndGame = false;
    static bool hasEnteredEndGameMenu = false;
    static bool isEnteringHighScore = false;
    static bool hasSelected = false;

    static EndGameOption endGameOption = RETRY;
    static EndGameOption lastEndGameOption = UNKNOWN;
    static unsigned long displayEndTime = 0;
    const unsigned long displayDuration = 2000; // 2 seconds

    // Step 1: Display End Game Message
    if (!hasEnteredEndGame)
    {
        displayEndGameMessage();
        displayEndTime = millis() + displayDuration; // Set the end time for the display
        hasEnteredEndGame = true;
        return; // Exit the function to allow the message to display for the full duration
    }

    // Step 2: Enter Player Name for High Score
    if (millis() > displayEndTime && !isEnteringHighScore && !hasEnteredEndGameMenu)
    {
        if (checkHighScore(player.points) != -1)
        {
            isEnteringHighScore = true;
        }
        else
        {
            // No high score, skip to end game menu
            isEnteringHighScore = false;
            hasEnteredEndGameMenu = true;
            return;
        }
    }

    if (isEnteringHighScore)
    {
        if (enterPlayerName())
        {
            updateHighScores();

            isEnteringHighScore = false;
            hasEnteredEndGameMenu = true;
            return;
        }
    }

    // Step 3: Display End Game Menu
    if (hasEnteredEndGameMenu)
    {
        if (!hasSelected)
        {
            endGameOption = handleEndGameControls(endGameOption, hasSelected, displayEndTime);
            if (endGameOption != lastEndGameOption)
            {
                displayEndGameMenu(lastEndGameOption, endGameOption);
            }
        }
        else
        {
            hasEnteredEndGame = false;
            hasEnteredEndGameMenu = false;
            isEnteringHighScore = false;
            hasSelected = false;

            endGameOption = RETRY;
            lastEndGameOption = UNKNOWN;
            displayEndTime = 0;

            // Execute the selected option
            if (endGameOption == RETRY)
            {
                enterPlayerName(true);
                restartGame();
            }
            else
            {
                goBack();
                clearMatrix();
            }
        }
    }
}

void restartGame()
{
    // Reset player's position and state
    player.x = 3; // Set to initial player X position
    player.y = 3; // Set to initial player Y position
    player.isVisible = true;
    player.lastBlinkTime = millis();

    // Reset the treasure
    placeTreasure(); // This function will also set treasure's initial position and spawn time

    // Reset game variables
    gameStartTime = millis(); // Restart the game timer
    player.points = 0;        // Reset the player's score

    // Reset any other game-related variables if needed

    // Optionally, clear the display and redraw the initial game state
    lcd.clear();
    displayMatrix(); // Assuming this function draws the initial game state on the matrix
}

////////////////////////////////////////////////////////////////
void updateMenuNavigation(int &currentSelection, const int menuItemCount)
{
    const unsigned long moveDelay = 200;
    static unsigned long lastMoveTime = 0;
    int joystickY = analogRead(joystickYPin);

    if (millis() - lastMoveTime > moveDelay)
    {
        bool selectionChanged = false;
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
            selectionChanged = true;
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
            selectionChanged = true;
            lastMoveTime = millis();
        }
        if (selectionChanged)
        {
            playSound(MenuNavigationSound); // Play sound on the buzzer
        }
    }
}

////////////////////////////////////////////////////////////////
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
    if (joystickButtonPressed())
    {
        goBack();
    }
}
////////////////////////////////////////////////////////////////
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
        displayMatrixBrightness();
        break;
    case SOUND:
        displaySoundSettingsMenu();
        break;
    }
}
/////////////////////////////////////////////////////////////////////////
void playSound(SoundType sound)
{
    if (!soundEnabled)
    {
        return; // Do nothing if sound is disabled
    }

    switch (sound)
    {
    case MenuNavigationSound:
        tone(BUZZER_PIN, BUZZER_FREQUENCY, BUZZER_DURATION); // Example: 1000 Hz for 50 ms
        break;
    case ButtonPressSound:
        tone(BUZZER_PIN, BUTTON_PRESS_FREQUENCY, BUTTON_PRESS_DURATION); // Example: 1500 Hz for 100 ms
        break;
    // Add more cases for different sounds
    default:
        break; // Optionally handle unknown sound types
    }
}
void toggleSoundSettings()
{
    soundEnabled = !soundEnabled; // Toggle the sound setting
    saveSoundSettings();          // Save the new setting to EEPROM
}
void saveSoundSettings()
{
    EEPROM.put(SOUND_SETTING_ADDRESS, soundEnabled);
    goBackSettings();
}
void loadSoundSettings()
{
    EEPROM.get(SOUND_SETTING_ADDRESS, soundEnabled);
}
void displaySoundSettingsMenu()
{
    static MenuOptions soundMenuSelection = MENU_YES; // Default to 'YES'
    static bool hasEntered = false;                   // Track if it's the first time entering the menu
    static unsigned long lastMoveTime = millis();     // For debouncing
    const unsigned long moveDelay = 200;              // Delay in milliseconds for debouncing

    // Clear the LCD only on the first entry
    if (!hasEntered)
    {
        lcd.clear();
        lastMoveTime = millis();
        hasEntered = true;
    }

    // Read joystick input for navigation
    int joystickX = analogRead(joystickXPin);

    // Check if enough time has passed since the last move
    if (millis() - lastMoveTime > moveDelay)
    {
        // Adjust selection based on left/right movement
        if (joystickX < JOYSTICK_THRESHOLD)
        {
            soundMenuSelection = static_cast<MenuOptions>(max(static_cast<int>(soundMenuSelection) - 1, static_cast<int>(MENU_YES)));
            lastMoveTime = millis();
        }
        else if (joystickX > 1023 - JOYSTICK_THRESHOLD)
        {
            soundMenuSelection = static_cast<MenuOptions>(min(static_cast<int>(soundMenuSelection) + 1, static_cast<int>(MENU_CANCEL)));
            lastMoveTime = millis();
        }
    }

    // Update the display only if there was a change
    if (millis() - lastMoveTime <= moveDelay)
    {
        lcd.setCursor(0, 0);
        lcd.print("Sound: ");
        lcd.setCursor(0, 1);
        lcd.print(soundMenuSelection == MENU_YES ? (char)2 : ' '); // Special character for 'YES'
        lcd.print("YES ");
        lcd.print(soundMenuSelection == MENU_NO ? (char)2 : ' '); // Special character for 'NO'
        lcd.print("NO ");
        lcd.print(soundMenuSelection == MENU_CANCEL ? (char)2 : ' '); // Special character for 'CANCEL'
        lcd.print("CANCEL");
    }

    // Handle button press
    if (joystickButtonPressed())
    {
        if (soundMenuSelection == MENU_YES)
        {
            soundEnabled = true;
            saveSoundSettings();
        }
        else if (soundMenuSelection == MENU_NO)
        {
            soundEnabled = false;
            // Reset the last move time to prevent accidental double-press
            saveSoundSettings();
        }
        // Reset the last move time to prevent accidental double-press
        // For both YES/NO and CANCEL, return to the previous menu
        goBackSettings();   // Assuming this function is defined to handle going back
        hasEntered = false; // Reset the flag when leaving the menu
    }
}
