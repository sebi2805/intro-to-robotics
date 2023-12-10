#include <LedControl.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

// ------------------------------
// Hardware Pin Configuration
// ------------------------------
// Pins for LED Matrix Control
const int dinPin = 3;
const int clkPin = 4;
const int csPin = 5;

// Pins for Joystick Control
const int joystickXPin = A0;
const int joystickYPin = A1;

// Pin for Button Control
const int buttonPin = 2;

// Pin for LCD Brightness Control
const int brightnessLcdPin = 9;

// Pins for Liquid Crystal Display (LCD)
const int rsPin = 8;
const int ePin = 7;
const int d4Pin = 10;
const int d5Pin = 11;
const int d6Pin = 12;
const int d7Pin = 13;

// Pin for Buzzer
const int buzzerPin = 6; // Buzzer connected to pin 6

LedControl lc = LedControl(dinPin, clkPin, csPin, 1);
LiquidCrystal lcd(rsPin, ePin, d4Pin, d5Pin, d6Pin, d7Pin);

// ------------------------------
// Structures
// ------------------------------
struct HighscoresEntry
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
    bool isCollected = false;
    bool isKilled = false;
    unsigned long killTime;
    unsigned long lastBlinkTime;
    unsigned long spawnTime; // Time when the treasure is placed needed for computing the actual store
};

enum ProgramState
{
    menu,
    game,
    settings,
    highscoresState,
    about,
    howToPlayState
};

enum SettingsState
{
    settingsMenuState,
    lcdBrightnessState,
    matrixBrightnessState,
    sound
};
enum SoundType
{
    menuNavigationSound,
    buttonPressSound,
    gameStartSound,
    introStartSound,
    outroEndingSound,
    unknownSound,
    treasureCollectionSound,
    allTreasuresKilledSound
};

enum MenuOptions
{
    menuYes = 0,
    menuNo = 1,
    menuCancel = 2,
    menuSave = 3,
    unknownMenuOption = 4
};
enum EndgameOption
{
    retry = 0,
    mainMenuState = 1,
    unknown = 2
};

enum SoundState
{
    soundOff,
    soundOn
};

// ------------------------------
// Constants Variables
// ------------------------------

// High Score Settings
const int maxHighScores = 3;
const int highScoreDisplayCount = 2; // Number of high scores to display at a time

// EEPROM Addresses
const int eepromStartAddress = 0;
const int lcdBrightnessAddress = eepromStartAddress + sizeof(HighscoresEntry) * maxHighScores;
const int matrixBrightnessAddress = lcdBrightnessAddress + sizeof(int);
const int soundSettingAddress = matrixBrightnessAddress + sizeof(int);

// Game Configuration
const int worldSize = 16;
const int maxTreasures = 4;

// Joystick Settings
const int joystickThreshold = 512 / 2;

// Timing Constants
const int debounceDelay = 200;
const int blinkInterval = 250;

// Buzzer Sounds for Game Events
const int buzzerMoveFrequency = 1000; // Frequency of the buzzer sound in Hertz
const int buzzerMoveDuration = 50;
const int buttonPressFrequency = 1500; // Frequency for button press sound
const int buttonPressDuration = 100;

// Constants for the Intro Sound
const int introSoundFrequencies[] = {440, 554, 659, 880};                // Example frequencies in Hz (A4, C#5, E5, A5)
const int introSoundDurations[] = {100, 100, 100, 300};                  // Durations in milliseconds
const int introSoundNotes = sizeof(introSoundFrequencies) / sizeof(int); // Number of notes

// Constants for the Game Start Sound
const int gameStartFrequencies[] = {523, 587, 659, 698, 784, 880, 988, 1047}; // Frequencies in Hz (C5, D5, E5, F5, G5, A5, B5, C6)
const int gameStartDurations[] = {100, 100, 100, 100, 100, 100, 100, 300};    // Durations in milliseconds
const int gameStartNotes = sizeof(gameStartFrequencies) / sizeof(int);        // Number of notes

// Constants for the Sad Ending Outro Sound
const int outroSoundFrequencies[] = {659, 587, 523, 440};                // Frequencies in Hz (E5, D5, C5, A4)
const int outroSoundDurations[] = {300, 300, 300, 600};                  // Durations in milliseconds (longer to convey sadness)
const int outroSoundNotes = sizeof(outroSoundFrequencies) / sizeof(int); // Number of notes

// Constants for the Treasure Collection Sound
const int treasureCollectionFrequencies[] = {880, 988, 1047};                            // Frequencies in Hz (A5, B5, C6)
const int treasureCollectionDurations[] = {50, 50, 100};                                 // Short, quick durations in milliseconds
const int treasureCollectionNotes = sizeof(treasureCollectionFrequencies) / sizeof(int); // Number of notes

// Constants for the All Treasures Killed Sound
const int allTreasuresKilledFrequencies[] = {1047, 880, 784, 659, 523}; // Frequencies from C6 to C5
const int allTreasuresKilledDurations[] = {50, 50, 50, 50, 100};        // Durations in milliseconds
const int allTreasuresKilledNotes = sizeof(allTreasuresKilledFrequencies) / sizeof(int);

// ------------------------------
// Text Constants
// ------------------------------
const String aboutText = "game: Space Adventure - By: DevTeam - GitHub: @DevTeam";
const String howToPlayText = "Move joystick to navigate. Button to select. Avoid obstacles, collect items. Have fun!";
const char characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
const int numCharacters = sizeof(characters) - 1; // -1 for the null terminator

// ------------------------------
// Custom characters
// ------------------------------
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

byte treasureChestLogo[8] = {
    0b00111100,
    0b01111110,
    0b11111111,
    0b11111111,
    0b10111101,
    0b10111101,
    0b11111111,
    0b01111110};
byte treasureChestChar[8] = {
    0b00000,
    0b01110,
    0b11111,
    0b11111,
    0b11111,
    0b10101,
    0b11111,
    0b01110};
byte starChar[8] = {
    0b00100,
    0b01110,
    0b11111,
    0b01110,
    0b00100,
    0b00000,
    0b00000,
    0b00000};

byte menuLogo[8] = {
    0b00001000,
    0b00011000,
    0b00111100,
    0b01111110,
    0b01000010,
    0b01011010,
    0b01011010,
    0b01111110};

byte gameLogo[8] = {
    0b00001000,
    0b00011000,
    0b00111000,
    0b00111110,
    0b00111000,
    0b00011000,
    0b00001000,
    0b00000000};

byte settingsLogo[8] = {
    0b00000000,
    0b00100100,
    0b01011010,
    0b11111111,
    0b01111110,
    0b01011010,
    0b00100100,
    0b00000000};

byte highscoresLogo[8] = {
    0b00011000,
    0b00011000,
    0b00111100,
    0b01111110,
    0b11111111,
    0b00111100,
    0b00111100,
    0b01011010};

byte aboutLogo[8] = {
    0b00000000,
    0b00011000,
    0b00100100,
    0b01000010,
    0b01111110,
    0b01000010,
    0b01000010,
    0b01000010};

byte howToPlayLogo[8] = {
    0b00011000,
    0b00100100,
    0b01000010,
    0b00000010,
    0b00000100,
    0b00011000,
    0b00000000,
    0b00011000};

// ------------------------------
// Virtual World
// ------------------------------
int virtualMatrix[worldSize][worldSize] = {
    {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1},
    {0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0},
    {1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 0},
    {1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1},
    {1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1},
    {0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0},
    {1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0}};

// Game State Variables
ProgramState currentState = menu;
SettingsState currentStatesettings = settingsMenuState;
unsigned long gameStartTime;
bool gameStarted = true;
unsigned long timeLimit = 60000;
bool introDisplayed = false;
unsigned long introStartTime = 0;

// Player and High Score Variables
String playerName = "";
int playerNameCharIndex = 0;
int playerNameLength = 0;
int highscorestartIndex = 0;
HighscoresEntry highscores[maxHighScores];

// Joystick and Selection Variables
int joystickCenter = 512;
int currentSelection = -1;
int settingsCurrentSelection = -1;
int lastJoystickState = joystickCenter; // Center position
unsigned long lastMoveTime = 0;

// Display and Brightness Variables
int lcdBrightness = 255;                             // Starting brightness value
MenuOptions brightnessSelection = unknownMenuOption; // 0 for adjusting brightness, 1 for 'Save', 2 for 'Cancel'
int matrixBrightness = 15;                           // Default value

int lastLcdBrightness = -1;
int lastMatrixBrightness = -1;
int lastBrightnessSelection = -1;

// Sound Settings
bool soundEnabled = true; // Default value, can be true or false
SoundState soundState = soundOff;
bool playOutroSound = true;
SoundType lastSound = unknownSound;
unsigned long soundStartTime;
int noteIndex = 0;

// Game Elements
Treasure treasures[maxTreasures];

// ------------------------------
// Function Declarations
// ------------------------------
// Settings and Configuration Functions
void goBacksettings();
void displayLCDBrightnessmenu();
void updateLCDBrightness();
void handleLCDBrightnessSelection();
void lcdBrightnessControl();
void matrixBrightnessControl();
void displayLCDBrightness();
void soundControl();
void goBack();

// Game Control Functions
void startgame();
void runGame();
void endgame();
void restartgame();
void updatePlayerPosition();
void checkTreasureCollection();
unsigned long getEarliestKillTime();
void placeTreasures();
bool areAllTreasuresKilled();
int getCurrentRoom();

// Display Functions
void showSettings();
void showHighScore();
void aboutGame();
void howToPlay();
void displayGenericMenu(const MenuItem menuItems[], int menuItemCount, int &currentSelection, int displayCount);
void displayhighscores();
void displayMatrix();
void displayIntroMessage();
void updateTreasureDisplay(unsigned long currentMillis);
void scrollText(const String &text);
void settingsmenuDisplay();
void displayMatrixBrightnessmenu();
void displayEndgameMessage();
void displayStateLogo();

// Brightness and Sound Settings
void applyLCDBrightness();
void saveLCDBrightness();
void loadLCDBrightness();
void applyMatrixBrightness();
void previewMatrixBrightness();
void saveMatrixBrightness();
void loadMatrixBrightness();
void displayMatrixBrightness();
void handleMatrixBrightnessSelection();
void playSound(SoundType sound);
void updateSound();
void togglesoundsettings();
void savesoundsettings();
void loadsoundsettings();

// High Score Functions
void saveHighscores(HighscoresEntry scores[], int count);
void loadhighscores();
bool updateHighscores();

// Game Logic and Navigation
void updateMenuNavigation(int &currentSelection, const int menuItemCount);
void restartPlayer();
void displayEndgameMenu(EndgameOption &lastEndgameOption, EndgameOption endgameOption);
void clearMatrix();
int countUncollectedTreasures();
EndgameOption handleEndgameControls(EndgameOption endgameOption, bool &hasEnteredEndgame, unsigned long displayEndTime);
bool enterPlayerName(bool reset = false);
bool isUncollectedTreasureAt(int x, int y);

// Utility Functions
bool joystickButtonPressed();
bool isTreasureInCurrentRoom(int treasureIndex);

// ------------------------------
// Menu Items
// ------------------------------
MenuItem settingsMenu[] = {
    {"LCD Brightness", lcdBrightnessControl},
    {"Matrix Brightness", matrixBrightnessControl},
    {"Sound", soundControl},
    {"Go back", goBack}};
int settingsMenuItemCount = sizeof(settingsMenu) / sizeof(MenuItem);

// ------------------------------

MenuItem mainMenu[] = {
    {"Start game", startgame},
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
    lcd.begin(16, 2);                     // Assuming a 16x2 LCD
    lcd.createChar(0, upArrow);           // Create custom up arrow character
    lcd.createChar(1, downArrow);         // Create custom down arrow character
    lcd.createChar(2, rightArrow);        // Create custom right arrow character
    lcd.createChar(3, treasureChestChar); // Register the treasure chest character as character 0
    lcd.createChar(4, starChar);
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
    loadsoundsettings();
    loadhighscores();

    analogWrite(brightnessLcdPin, lcdBrightness);
    // game-specific setup
    placeTreasures(); // Place the treasure in the game
    // Serial.begin(9600);
    // formatEEPROM();
}

void formatEEPROM()
{
    HighscoresEntry defaultEntry;
    memset(defaultEntry.playerName, 0, sizeof(defaultEntry.playerName)); // Set all characters to 0 (null)
    defaultEntry.score = 0;                                              // Default score

    for (int i = 0; i < maxHighScores; i++)
    {
        EEPROM.put(eepromStartAddress + i * sizeof(HighscoresEntry), defaultEntry);
    }
}
void loop()
{
    if (introDisplayed)
    {
        switch (currentState)
        {
        case menu:
            updateMenuNavigation(currentSelection, menuItemCount);
            displayGenericMenu(mainMenu, menuItemCount, currentSelection, 2);

            if (joystickButtonPressed())
            {
                mainMenu[currentSelection].function();
            }
            break;
        case game:
            runGame(); // Placeholder function for running the game
            break;
        case highscoresState:
            updateMenuNavigation(highscorestartIndex, maxHighScores);
            displayhighscores();

            break;
        case settings:
            settingsmenuDisplay();
            break;
        case about:
            scrollText(aboutText);
            break;
        case howToPlayState:
            scrollText(howToPlayText);
            break;
        }
        displayStateLogo();
    }
    else
        displayIntroMessage();
    updateSound();
    delay(1);
}

// ------------------------------
// Function Implementations
void goBacksettings()
{
    settingsCurrentSelection = -1;
    currentStatesettings = settingsMenuState;
};
////////////////////////////////////////////////////////////////
bool joystickButtonPressed()
{
    static unsigned long lastPress = 0;
    if (digitalRead(buttonPin) == LOW)
    {
        if (millis() - lastPress > debounceDelay)
        {
            lastPress = millis();
            playSound(buttonPressSound); // Play a different sound
            return true;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////

void displayLCDBrightnessmenu()
{

    // Clear the LCD and force update on the first call
    if (lastLcdBrightness == -1 && lastBrightnessSelection == -1)
    {
        lcd.clear();
        lastLcdBrightness = lcdBrightness - 1;             // Force update
        lastBrightnessSelection = brightnessSelection - 1; // Force update
    }

    // Update the brightness value if it has changed
    if (lastLcdBrightness != lcdBrightness)
    {
        lcd.setCursor(0, 0);
        lcd.print(F("Brightness:     ")); // Clear the previous value
        lcd.setCursor(12, 0);
        lcd.print(lcdBrightness);
        lastLcdBrightness = lcdBrightness;
    }

    // Update the selection options if they have changed
    if (lastBrightnessSelection != brightnessSelection)
    {
        lcd.setCursor(0, 1);
        if (brightnessSelection == menuSave)
        {
            lcd.print((char)2); // Right arrow
            lcd.print(F("Save  Cancel"));
        }
        else if (brightnessSelection == menuCancel)
        {
            lcd.print(F("Save  "));
            lcd.print((char)2); // Right arrow
            lcd.print(F("Cancel"));
        }
        else
        {
            lcd.print(F("Save  Cancel"));
        }

        lastBrightnessSelection = brightnessSelection;
    }
}

void updateLCDBrightness()
{
    int joystickY = analogRead(joystickYPin);
    int joystickX = analogRead(joystickXPin);
    // Check if the joystick is moved significantly from the center
    if (abs(joystickY - joystickCenter) > joystickThreshold)
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
    else if (joystickX < joystickThreshold)
    {
        brightnessSelection = menuSave; // Select "Save"
    }
    else if (joystickX > 1023 - joystickThreshold)
    {
        brightnessSelection = menuCancel; // Select "Cancel"
    }

    // Update the display
    displayLCDBrightnessmenu();
}

void handleLCDBrightnessSelection()
{

    if (joystickButtonPressed())
    {
        lastLcdBrightness = -1;
        lastBrightnessSelection = -1;
        if (brightnessSelection == menuSave)
        {
            // Save brightness to EEPROM
            EEPROM.put(lcdBrightnessAddress, lcdBrightness);
            applyLCDBrightness();
        }
        else if (brightnessSelection == menuCancel)
        {
            // Cancel: Reload the brightness from EEPROM
            EEPROM.get(lcdBrightnessAddress, lcdBrightness);
            applyLCDBrightness();
        }

        brightnessSelection = unknownMenuOption;
        // Reset selection and go back to the previous menu
        goBacksettings();
    }
}

void lcdBrightnessControl()
{
    currentStatesettings = lcdBrightnessState;
    EEPROM.get(lcdBrightnessAddress, lcdBrightness);
}

void displayLCDBrightness()
{

    updateLCDBrightness();
    handleLCDBrightnessSelection();
}
void applyLCDBrightness()
{
    int pwmValue = map(lcdBrightness, 0, 255, 0, 255);
    analogWrite(brightnessLcdPin, pwmValue);
}
void saveLCDBrightness()
{
    EEPROM.put(lcdBrightnessAddress, lcdBrightness);
    applyLCDBrightness(); // Apply and save the brightness
}

void loadLCDBrightness()
{
    EEPROM.get(lcdBrightnessAddress, lcdBrightness);
    applyLCDBrightness();
}
////////////////////////////////////////////////////////////////
void displayMatrixBrightnessmenu()
{

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
        lcd.print(F("Matrix Bright: "));
        lcd.setCursor(14, 0);

        if (matrixBrightness < 10)
        {
            lcd.print('0'); // Print leadinPing zero for single-digit numbers
        }
        lcd.print(matrixBrightness);

        lastMatrixBrightness = matrixBrightness;
    }

    // Update the selection options if they have changed
    if (lastBrightnessSelection != brightnessSelection)
    {
        lcd.setCursor(0, 1);

        // For 'Save'
        if (brightnessSelection == menuSave)
        {
            lcd.print((char)2); // Right arrow for 'Save'
        }
        else
        {
            lcd.print(' ');
        }
        lcd.print(F("Save  "));

        // For 'Cancel'
        if (brightnessSelection == menuCancel)
        {
            lcd.print((char)2); // Right arrow for 'Cancel'
        }
        else
        {
            lcd.print(' ');
        }
        lcd.print(F("Cancel"));

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
    updateMatrixBrightness();
    handleMatrixBrightnessSelection();
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
        lastBrightnessSelection = -1;
        lastMatrixBrightness = -1;
        if (brightnessSelection == menuSave) // 'Save' selected
        {
            saveMatrixBrightness();
            applyMatrixBrightness(); // Apply and save the brightness setting
            hasEntered = false;
        }
        else if (brightnessSelection == menuCancel) // 'Cancel' selected
        {
            loadMatrixBrightness();  // Reload the brightness from EEPROM
            applyMatrixBrightness(); // Apply the reloaded brightness setting
            hasEntered = false;
        }

        brightnessSelection = unknownMenuOption;
        // Reset selection and possibly go back to the previous menu
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
        if (abs(joystickY - 512) > joystickThreshold)
        {
            matrixBrightness = constrain(matrixBrightness + (joystickY < 512 ? -1 : 1), 0, 15);
            applyMatrixBrightness();
            previewMatrixBrightness();
            lastUpdateTime = millis(); // Update the last update time
        }
        else if (joystickX < joystickThreshold)
        {
            brightnessSelection = 1; // Select "Save"
            lastUpdateTime = millis();
        }
        else if (joystickX > 1023 - joystickThreshold)
        {
            brightnessSelection = 2; // Select "Cancel"
            lastUpdateTime = millis();
        }

        displayMatrixBrightnessmenu();
    }
}

void applyMatrixBrightness()
{
    lc.setIntensity(0, matrixBrightness);
}
void saveMatrixBrightness()
{
    EEPROM.put(matrixBrightnessAddress, matrixBrightness);
    lc.clearDisplay(0);
    goBacksettings();
}

void loadMatrixBrightness()
{
    EEPROM.get(matrixBrightnessAddress, matrixBrightness);
    lc.setIntensity(0, matrixBrightness);
    lc.clearDisplay(0);
    goBacksettings();
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
    currentStatesettings = matrixBrightnessState;
    EEPROM.get(matrixBrightnessAddress, matrixBrightness);
}
////////////////////////////////////////////////////////////////

void soundControl()
{
    currentStatesettings = sound;
};

////////////////////////////////////////////////////////////////
void goBack()
{
    currentSelection = -1;
    currentState = menu;
};
void startgame()
{
    currentState = game;
    gameStartTime = millis();
}

void showSettings()
{
    currentState = settings;
}

void showHighScore()
{
    currentState = highscoresState;
}

void aboutGame()
{
    currentState = about;
}

void howToPlay()
{
    currentState = howToPlayState;
}
////////////////////////////////////////////////////////////////
void saveHighscores(HighscoresEntry scores[], int count)
{
    int address = eepromStartAddress;
    for (int i = 0; i < count; i++)
    {
        EEPROM.put(address, scores[i]);
        address += sizeof(HighscoresEntry);
    }
}
void loadhighscores()
{
    int address = eepromStartAddress;
    for (int i = 0; i < maxHighScores; i++)
    {
        EEPROM.get(address, highscores[i]);
        address += sizeof(HighscoresEntry);
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
                lcd.print(F(">"));
            }
            else
            {
                lcd.print(F(" "));
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

void displayhighscores()
{
    static int lastStartIndex = -1;
    // Only clear and update the display if there are changes
    if (highscorestartIndex != lastStartIndex)
    {
        lcd.clear();
        for (int i = 0; i < highScoreDisplayCount; i++)
        {
            int scoreIndex = highscorestartIndex + i;
            if (scoreIndex < maxHighScores)
            {
                lcd.setCursor(0, i);
                lcd.print(scoreIndex + 1);
                lcd.print(F(". "));
                lcd.print(highscores[scoreIndex].playerName);
                lcd.print(F(" - "));
                lcd.print(highscores[scoreIndex].score);
            }
        }

        // Upward arrow
        if (highscorestartIndex > 0)
        {
            lcd.setCursor(15, 0);
            lcd.write(byte(0));
        }

        // Downward arrow
        if (highscorestartIndex < maxHighScores - highScoreDisplayCount)
        {
            lcd.setCursor(15, 1);
            lcd.write(byte(1));
        }

        lastStartIndex = highscorestartIndex;
    }
    if (joystickButtonPressed())
    {
        goBack();
        lastStartIndex = -1;
    }
}
////////////////////////////////////////////////////////////
void displayStateLogo()
{
    byte *logo;

    switch (currentState)
    {
    case menu:
        logo = menuLogo;
        break;
    case settings:
        logo = settingsLogo;
        break;
    case highscoresState:
        logo = highscoresLogo;
        break;
    case about:
        logo = aboutLogo;
        break;
    case howToPlayState:
        logo = howToPlayLogo;
        break;
    default:
        return; // Do nothing if the state is not recognized
    }

    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            bool isOn = logo[row] & (1 << col);
            lc.setLed(0, row, col, isOn);
        }
    }
}

////////////////////////////////////////////////////////////
void displayIntroMessage()
{
    static bool isFirstTime = true;

    if (!introDisplayed)
    {
        // Display intro message on the LCD
        if (isFirstTime)
        {
            lcd.clear();
            isFirstTime = false;
            playSound(introStartSound);
        }
        lcd.setCursor(0, 0); // Start at the first row
        lcd.write(byte(3));
        lcd.print(F("  WELCOME TO  "));
        lcd.write(byte(3)); // Star character

        // Second Row: "*treasure chest* TREASURE HUNT *treasure chest*"
        lcd.setCursor(0, 1); // Move to the second row
        lcd.write(byte(4));  // Treasure chest character
        lcd.print(F(" TREASUREHUNT "));
        lcd.write(byte(4)); // Treasure chest character

        // Display the logo on the LED matrix
        for (int row = 0; row < 8; row++)
        {
            byte displayRow = treasureChestLogo[row];
            for (int col = 0; col < 8; col++)
            {
                bool isOn = displayRow & (1 << col);
                lc.setLed(0, row, col, isOn);
            }
        }
    }

    if (millis() - introStartTime > 3000)
    { // 3 seconds delay
        lcd.clear();
        lc.clearDisplay(0);
        introDisplayed = true;
    }
}
////////////////////////////////////////////////////////////
void displayMatrix()
{
    int roomX = player.x / 8; // Determine the room's X-coordinPinate (0 or 1)
    int roomY = player.y / 8; // Determine the room's Y-coordinPinate (0 or 1)

    int startX = roomX * 8;
    int startY = roomY * 8;

    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            int worldRow = startY + row;
            int worldCol = startX + col;

            // Display the walls, open space, player, and treasure based on the virtual world
            if (virtualMatrix[worldRow][worldCol] == 1)
            {
                lc.setLed(0, row, col, true); // Wall
            }
            else
            {
                lc.setLed(0, row, col, virtualMatrix[worldRow][worldCol]); // Open space or other elements
            }
        }
    }

    // Adjust player and treasure positions relative to the current room
    lc.setLed(0, player.y % 8, player.x % 8, player.isVisible);

    // Display treasures
    for (int i = 0; i < maxTreasures; i++)
    {
        if (treasures[i].isVisible && isTreasureInCurrentRoom(i) && !treasures[i].isCollected)
        {
            lc.setLed(0, treasures[i].y % 8, treasures[i].x % 8, true);
        }
    }
}
int countUncollectedTreasures()
{
    int uncollectedCount = 0;
    for (int i = 0; i < maxTreasures; i++)
    {
        if (!treasures[i].isCollected && !treasures[i].isKilled)
        {
            uncollectedCount++;
        }
    }
    return uncollectedCount;
}
void checkTreasureCollection()
{
    unsigned long currentMillis = millis();
    for (int i = 0; i < maxTreasures; i++)
    {
        if (player.x == treasures[i].x && player.y == treasures[i].y && treasures[i].isVisible && !treasures[i].isCollected)
        {
            unsigned long timeTaken = currentMillis - treasures[i].spawnTime;
            int timeBasedDeduction = min(timeTaken / 1000, 40); // Max deduction is 40
            int pointsForThisTreasure = max(50 - timeBasedDeduction, 10);
            player.points += pointsForThisTreasure;

            treasures[i].isCollected = true;    // Mark the treasure as collected
            playSound(treasureCollectionSound); // Play the treasure sound
        }

        if (!treasures[i].isKilled && currentMillis > treasures[i].killTime && !treasures[i].isCollected)
        {

            playSound(allTreasuresKilledSound);
            treasures[i].isKilled = true;
            treasures[i].isVisible = false; // Optionally hide killed treasures
        }
    }
}

bool isTreasureInCurrentRoom(int treasureIndex)
{
    int roomX = player.x / 8;
    int roomY = player.y / 8;
    return (treasures[treasureIndex].x / 8 == roomX && treasures[treasureIndex].y / 8 == roomY);
}
bool isUncollectedTreasureAt(int x, int y)
{
    for (int i = 0; i < maxTreasures; i++)
    {
        if (treasures[i].x == x && treasures[i].y == y && !treasures[i].isCollected)
        {
            return true;
        }
    }
    return false;
}

void updatePlayerPosition()
{
    static bool isJoystickNeutral = true; // Tracks if the joystick is in the neutral position

    int joystickY = analogRead(joystickYPin); // Read Y-axis
    int joystickX = analogRead(joystickXPin); // Read X-axis

    // Only allow movement if the joystick was in the neutral position
    if (isJoystickNeutral)
    {
        int previousX = player.x;
        int previousY = player.y;

        // Check for X-axis movement
        if (joystickX < 512 - joystickThreshold && player.x > 0)
        {
            player.x--; // Move left
        }
        else if (joystickX > 512 + joystickThreshold && player.x < worldSize - 1)
        {
            player.x++; // Move right
        }

        // Check for Y-axis movement
        if (joystickY < 512 - joystickThreshold && player.y < worldSize - 1)
        {
            player.y++; // Move down
        }
        else if (joystickY > 512 + joystickThreshold && player.y > 0)
        {
            player.y--; // Move up
        }

        // Check for collision or treasure
        if (virtualMatrix[player.y][player.x] == 1 && !isUncollectedTreasureAt(player.x, player.y))
        {
            player.x = previousX; // Revert if hitting a wall and no treasure
            player.y = previousY;
        }

        // Set the joystick to non-neutral after movement
        if (player.x != previousX || player.y != previousY)
        {
            isJoystickNeutral = false;
        }
    }

    // Reset to neutral if joystick is in the center position
    if ((abs(joystickX - 512) < joystickThreshold) && (abs(joystickY - 512) < joystickThreshold))
    {
        isJoystickNeutral = true;
    }
}

////////////////////////////////////////////////////////////////
unsigned long getEarliestKillTime()
{
    unsigned long earliestKillTime = treasures[0].killTime; // Initialize with maximum possible value
    for (int i = 1; i < maxTreasures; i++)
    {
        if (!treasures[i].isCollected && !treasures[i].isKilled && treasures[i].killTime < earliestKillTime)
        {
            earliestKillTime = treasures[i].killTime;
        }
    }
    return earliestKillTime;
}
void placeTreasures()
{
    int nonKilledTreasures = 0;
    for (int i = 0; i < maxTreasures; i++)
    {
        if (!treasures[i].isKilled)
        {
            nonKilledTreasures++;
        }
    }
    for (int i = 0, placedTreasures = 0; i < maxTreasures && placedTreasures < nonKilledTreasures; i++)
    {
        if (!treasures[i].isKilled)
        {
            int newTreasureX, newTreasureY;
            int roomX = (i % 2) * 8; // Room X-coordinPinate (either 0 or 8)
            int roomY = (i / 2) * 8; // Room Y-coordinPinate (either 0 or 8)

            do
            {
                newTreasureX = roomX + random(0, 8);                  // Random X within the room
                newTreasureY = roomY + random(0, 8);                  // Random Y within the room
            } while (virtualMatrix[newTreasureY][newTreasureX] == 1); // Ensure it's not placed on a wall

            treasures[i].x = newTreasureX;
            treasures[i].y = newTreasureY;

            treasures[i].killTime = millis() + 20000; // 20 seconds kill time
            treasures[i].isVisible = true;
            treasures[i].isCollected = false;
            treasures[i].spawnTime = millis();
            placedTreasures++;
        }
    }
}
int getCurrentRoom()
{
    int roomX = player.x / (worldSize / 2);
    int roomY = player.y / (worldSize / 2);
    return roomY * 2 + roomX + 1; // +1 to make rooms start from 1 instead of 0
}
bool areAllTreasuresKilled()
{
    for (int i = 0; i < maxTreasures; i++)
    {
        if (!treasures[i].isKilled)
        {
            return false; // Found a treasure that is not killed
        }
    }
    return true; // All treasures are killed
}

void updateTreasureDisplay(unsigned long currentMillis)
{
    const unsigned long TREASURE_blinkInterval = 100; // Faster blink interval for treasure

    for (int i = 0; i < maxTreasures; i++)
    {
        if (!treasures[i].isCollected)
        { // Only update display for uncollected treasures
            if (currentMillis - treasures[i].lastBlinkTime >= TREASURE_blinkInterval)
            {
                treasures[i].isVisible = !treasures[i].isVisible;
                treasures[i].lastBlinkTime = currentMillis;
            }
        }
    }
}

void runGame()
{
    unsigned long currentMillis = millis();
    if (gameStarted)
    {
        playSound(gameStartSound); // Play the game start sound
        gameStarted = false;       // Set the flag to false after playing the sound
    }
    if (currentMillis - gameStartTime > timeLimit || areAllTreasuresKilled())
    {
        if (playOutroSound)
        {
            playSound(outroEndingSound);
            playOutroSound = false;
        }
        gameStarted = false;
        endgame();
        return;
    }
    else
    {
        // game logic...
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

            unsigned long timeElapsed = currentMillis - gameStartTime;
            unsigned long totalRemainingTime = (timeLimit > timeElapsed) ? (timeLimit - timeElapsed) : 0;
            int totalRemainingMinutes = totalRemainingTime / 60000;
            int totalRemainingSeconds = (totalRemainingTime % 60000) / 1000;

            // Current round time calculation
            unsigned long earliestKillTime = getEarliestKillTime();
            int roundTimeRemaining = (earliestKillTime > currentMillis) ? (earliestKillTime - currentMillis) / 1000 : 0; // Time in seconds until the earliest treasure despawns

            // Display total remaining time and round time on the first row
            lcd.setCursor(0, 0);
            lcd.print(F("T:"));
            lcd.print(totalRemainingMinutes);
            lcd.print(F(":"));
            if (totalRemainingSeconds < 10)
            {
                lcd.print(F("0")); // Leading zero for single digit seconds
            }
            lcd.print(totalRemainingSeconds);
            lcd.print(F(" R:"));
            lcd.print(roundTimeRemaining);

            // Display player's points on the second row
            lcd.setCursor(0, 1);
            lcd.print(F("Pts: "));
            lcd.print(player.points);
            lcd.print(F(" Rm:"));
            lcd.print(getCurrentRoom());
            lastLCDUpdate = currentMillis;
        }

        if (currentMillis - player.lastBlinkTime >= blinkInterval)
        { // Blink interval (250 ms)
            player.isVisible = !player.isVisible;
            player.lastBlinkTime = currentMillis;
        }
        updateTreasureDisplay(currentMillis);

        // Check if all treasures are collected
        if (countUncollectedTreasures() == 0)
        {
            placeTreasures(); // Spawn new treasures
        }
        // Check for treasure collection
        checkTreasureCollection();

        // Display the updated matrix
        displayMatrix();
        updatePlayerPosition();
    }
}

void displayEndgameMessage()
{
    int highScoreRank = checkHighScore(player.points);

    lcd.clear();
    lcd.setCursor(0, 0);

    if (highScoreRank != -1)
    {
        // Player achieved a high score
        lcd.print(F("Congrats! Rank "));
        lcd.print(highScoreRank + 1); // Adding 1 because rank is 0-indexed
    }
    else
    {
        // Player did not achieve a high score
        lcd.print(F("Game finished!"));
    }

    // Display player's points
    lcd.setCursor(0, 1);
    lcd.print(F("Points: "));
    lcd.print(player.points);
}

void displayEndgameMenu(EndgameOption &lastEndgameOption, EndgameOption endgameOption)
{
    // Update the display only if the option has changed
    if (lastEndgameOption != endgameOption)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(endgameOption == retry ? (char)2 : ' ');
        lcd.print(F("Retry"));
        lcd.setCursor(0, 1);
        lcd.print(endgameOption == mainMenuState ? (char)2 : ' ');
        lcd.print(F("Main menu"));

        lastEndgameOption = endgameOption;
    }
}

EndgameOption handleEndgameControls(EndgameOption endgameOption, bool &hasSelected, unsigned long displayEndTime)
{
    if (millis() >= displayEndTime)
    {
        // Read joystick input for navigation
        int joystickY = analogRead(joystickYPin);

        if (joystickY < joystickThreshold && endgameOption != mainMenuState)
        {
            playSound(menuNavigationSound);
            return mainMenuState;
        }
        else if (joystickY > 1023 - joystickThreshold && endgameOption != retry)
        {
            playSound(menuNavigationSound);
            return retry;
        }

        if (joystickButtonPressed())
        {
            hasSelected = true;   // Reset for next time entering end game
            return endgameOption; // Return the selected option
        }
    }
    return endgameOption;
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
        lcd.print(F("Enter Name:"));
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
        bool joystickNeutral = abs(joystickX - joystickCenter) < joystickThreshold;

        if (joystickNeutral)
        {
            isJoystickNeutral = true;
        }
        else if (isJoystickNeutral)
        {
            if (joystickX < joystickThreshold)
            {
                playerNameCharIndex = (playerNameCharIndex > 0) ? playerNameCharIndex - 1 : numCharacters - 1;
            }
            else if (joystickX > 1023 - joystickThreshold)
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
            lcd.print(F("Enter Name:"));
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
    loadhighscores(); // Assuming this function loads the high scores from EEPROM

    for (int i = 0; i < maxHighScores; ++i)
    {
        if (playerScore > highscores[i].score)
        {
            return i; // Player's score beats the score at position i
        }
    }
    return -1; // Player's score does not beat any high score
}

bool updateHighscores()
{
    int position = checkHighScore(player.points);
    if (position != -1)
    {
        // Shift lower scores down by one position
        for (int i = maxHighScores - 1; i > position; --i)
        {
            highscores[i] = highscores[i - 1];
        }

        // Insert the new high score
        if (enterPlayerName())
        {
            highscores[position].score = player.points;

            // If playerName is a String, use c_str() to convert to char*
            strncpy(highscores[position].playerName, playerName.c_str(), sizeof(highscores[position].playerName));
            // Ensure null termination
            highscores[position].playerName[sizeof(highscores[position].playerName) - 1] = '\0';
            saveHighscores(highscores, maxHighScores); // Save the updated scores to EEPROM
            playerName = "";
            return true; // Reset player name for next game
        }
    }
    return false;
}

void endgame()
{
    static bool hasEnteredEndgame = false;
    static bool hasEnteredEndgamemenu = false;
    static bool isEnteringHighScore = false;
    static bool hasSelected = false;

    static EndgameOption endgameOption = retry;
    static EndgameOption lastEndgameOption = unknown;
    static unsigned long displayEndTime = 0;
    const unsigned long displayDuration = 4000; // 2 seconds

    // Step 1: Display End game Message
    if (!hasEnteredEndgame)
    {
        displayEndgameMessage();
        displayEndTime = millis() + displayDuration; // Set the end time for the display
        hasEnteredEndgame = true;
        return; // Exit the function to allow the message to display for the full duration
    }

    // Step 2: Enter Player Name for High Score
    if (millis() > displayEndTime && !isEnteringHighScore && !hasEnteredEndgamemenu)
    {
        if (checkHighScore(player.points) != -1)
        {
            isEnteringHighScore = true;
        }
        else
        {
            // No high score, skip to end game menu
            isEnteringHighScore = false;
            hasEnteredEndgamemenu = true;
            return;
        }
    }

    if (isEnteringHighScore)
    {
        if (updateHighscores())
        {
            isEnteringHighScore = false;
            hasEnteredEndgamemenu = true;
        }
        return;
    }

    // Step 3: Display End game menu
    if (hasEnteredEndgamemenu)
    {
        if (!hasSelected)
        {
            endgameOption = handleEndgameControls(endgameOption, hasSelected, displayEndTime);
            if (endgameOption != lastEndgameOption)
            {
                displayEndgameMenu(lastEndgameOption, endgameOption);
            }
        }
        else
        {

            // Execute the selected option
            if (endgameOption == retry)
            {
                enterPlayerName(true);
                restartgame();
            }
            else
            {
                goBack();
                clearMatrix();
            }

            hasEnteredEndgame = false;
            playOutroSound = true;
            hasEnteredEndgamemenu = false;
            isEnteringHighScore = false;
            hasSelected = false;

            endgameOption = retry;
            lastEndgameOption = unknown;
            displayEndTime = 0;
        }
    }
}

void restartPlayer()
{
    // Reset player's position and state
    player.x = 3;      // Set to initial player X position
    player.y = 3;      // Set to initial player Y position
    player.points = 0; // Reset the player's score
    player.isVisible = true;
    player.lastBlinkTime = millis();
}
void restartgame()
{
    for (int i = 0; i < maxTreasures; i++)
    {
        treasures[i].isCollected = false;
        treasures[i].isKilled = false;
        treasures[i].isVisible = false;
    }
    restartPlayer();
    // Reset the treasure
    placeTreasures(); // This function will also set treasure's initial position and spawn time

    // Reset game variables
    gameStartTime = millis(); // Restart the game timer

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

        if (joystickY > 1023 - joystickThreshold)
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
        else if (joystickY < joystickThreshold)
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
            playSound(menuNavigationSound); // Play sound on the buzzer
        }
    }
}

////////////////////////////////////////////////////////////////
void scrollText(const String &text)
{
    static int currentLine = 0;
    static unsigned long lastJoystickMoveTime = 0;
    const int maxLineLength = 17;
    const int totalLines = (text.length() + maxLineLength - 1) / maxLineLength;
    const unsigned long joystickMoveDelay = 200; // Delay to prevent rapid joystick input

    int joystickY = analogRead(joystickYPin);
    unsigned long currentMillis = millis();

    if (currentMillis - lastJoystickMoveTime > joystickMoveDelay)
    {
        if (joystickY < joystickThreshold)
        {
            if (currentLine > 0)
                currentLine--;
            lastJoystickMoveTime = currentMillis;
        }
        else if (joystickY > 1023 - joystickThreshold)
        {
            if (currentLine < totalLines - 2)
                currentLine++;
            lastJoystickMoveTime = currentMillis;
        }
    }

    lcd.clear();
    for (int row = 0; row < 2; row++)
    {
        int lineIndex = currentLine + row;
        if (lineIndex < totalLines)
        {
            lcd.setCursor(0, row);
            int startIndex = lineIndex * maxLineLength;
            int endIndex = min(startIndex + maxLineLength, text.length());
            lcd.print(text.substring(startIndex, endIndex));
        }
    }

    if (joystickButtonPressed())
    {
        goBack();
    }
}

////////////////////////////////////////////////////////////////
void settingsmenuDisplay()
{
    switch (currentStatesettings)
    {
    case settingsMenuState:
        updateMenuNavigation(settingsCurrentSelection, settingsMenuItemCount);
        displayGenericMenu(settingsMenu, settingsMenuItemCount, settingsCurrentSelection, 2);
        if (joystickButtonPressed())
        {
            settingsMenu[settingsCurrentSelection].function();
        }
        break;
    case lcdBrightnessState:
        displayLCDBrightness();
        break;
    case matrixBrightnessState:
        // Logic for adjusting matrix brightness
        displayMatrixBrightness();
        break;
    case sound:
        displaysoundsettingsmenu();
        break;
    }
}
/////////////////////////////////////////////////////////////////////////
void playSound(SoundType sound)
{
    lastSound = sound; // Save the last sound played
    if (!soundEnabled)
    {
        return; // Do nothing if sound is disabled
    }

    switch (sound)
    {
    case menuNavigationSound:
        tone(buzzerPin, buzzerMoveFrequency, buzzerMoveDuration); // Example: 1000 Hz for 50 ms
        break;
    case buttonPressSound:
        tone(buzzerPin, buttonPressFrequency, buttonPressDuration); // Example: 1500 Hz for 100 ms
        break;
    case introStartSound:
        soundState = soundOn;
        noteIndex = 0;
        soundStartTime = millis();
        tone(buzzerPin, introSoundFrequencies[noteIndex], introSoundDurations[noteIndex]);
        break;
    case gameStartSound:
        soundState = soundOn;
        noteIndex = 0;
        soundStartTime = millis();
        tone(buzzerPin, gameStartFrequencies[noteIndex], gameStartDurations[noteIndex]);
        break;
    case outroEndingSound:
        soundState = soundOn;
        noteIndex = 0;
        soundStartTime = millis();
        tone(buzzerPin, outroSoundFrequencies[noteIndex], outroSoundDurations[noteIndex]);
        break;
    case treasureCollectionSound:
        soundState = soundOn;
        noteIndex = 0;
        soundStartTime = millis();
        tone(buzzerPin, treasureCollectionFrequencies[noteIndex], treasureCollectionDurations[noteIndex]);
        lastSound = treasureCollectionSound;
        break;
    case allTreasuresKilledSound:
        for (int i = 0; i < allTreasuresKilledNotes; i++)
        {
            tone(buzzerPin, allTreasuresKilledFrequencies[i], allTreasuresKilledDurations[i]);
            delay(allTreasuresKilledDurations[i]);
        }
        break;
    // Add more cases for different sounds
    default:
        break; // Optionally handle unknown sound types
    }
}

void updateSound()
{
    if (soundState == soundOn)
    {
        unsigned long currentMillis = millis();

        int currentNoteDuration;
        int currentFrequency;

        switch (lastSound)
        {
        case introStartSound:
            if (noteIndex < introSoundNotes)
            {
                currentNoteDuration = introSoundDurations[noteIndex];
                currentFrequency = introSoundFrequencies[noteIndex];
            }
            break;
        case gameStartSound:
            if (noteIndex < gameStartNotes)
            {
                currentNoteDuration = gameStartDurations[noteIndex];
                currentFrequency = gameStartFrequencies[noteIndex];
            }
            break;
        case outroEndingSound:
            if (noteIndex < outroSoundNotes)
            {
                currentNoteDuration = outroSoundDurations[noteIndex];
                currentFrequency = outroSoundFrequencies[noteIndex];
            }
            break;
        case treasureCollectionSound:

            if (noteIndex < treasureCollectionNotes)
            {
                currentNoteDuration = treasureCollectionDurations[noteIndex];
                currentFrequency = treasureCollectionFrequencies[noteIndex];
            }
            break;
        case allTreasuresKilledSound:

            if (noteIndex < allTreasuresKilledNotes)
            {
                currentNoteDuration = allTreasuresKilledDurations[noteIndex];
                currentFrequency = allTreasuresKilledFrequencies[noteIndex];
            }
            break;
        default:
            return; // No sound or unknown sound type
        }

        if (currentMillis - soundStartTime >= currentNoteDuration)
        {
            int numberOfNotes = 0;
            switch (lastSound)
            {
            case introStartSound:
                numberOfNotes = introSoundNotes;
                break;
            case gameStartSound:
                numberOfNotes = gameStartNotes;
                break;
            case outroEndingSound:
                numberOfNotes = outroSoundNotes;
                break;
            case treasureCollectionSound:
                numberOfNotes = treasureCollectionNotes;
                break;
            case allTreasuresKilledSound:
                numberOfNotes = allTreasuresKilledNotes;
                break;
            default:
                return; // No sound or unknown sound type
            }

            noteIndex++;
            soundStartTime = currentMillis; // Update the start time

            if (noteIndex < numberOfNotes)
            { // Replace with correct note count for the current sound
                tone(buzzerPin, currentFrequency, currentNoteDuration);
            }
            else
            {
                soundState = soundOff;    // All notes played, turn off the sound
                lastSound = unknownSound; // Reset last sound
            }
        }
    }
}

void togglesoundsettings()
{
    soundEnabled = !soundEnabled; // Toggle the sound setting
    savesoundsettings();          // Save the new setting to EEPROM
}
void savesoundsettings()
{
    EEPROM.put(soundSettingAddress, soundEnabled);
    goBacksettings();
}
void loadsoundsettings()
{
    EEPROM.get(soundSettingAddress, soundEnabled);
}
void displaysoundsettingsmenu()
{
    static MenuOptions soundMenuSelection = menuYes; // Default to 'YES'
    static bool hasEntered = false;                  // Track if it's the first time entering the menu
    static unsigned long lastMoveTime = millis();    // For debouncing
    const unsigned long moveDelay = 200;             // Delay in milliseconds for debouncing

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
        if (joystickX < joystickThreshold)
        {
            soundMenuSelection = static_cast<MenuOptions>(max(static_cast<int>(soundMenuSelection) - 1, static_cast<int>(menuYes)));
            lastMoveTime = millis();
        }
        else if (joystickX > 1023 - joystickThreshold)
        {
            soundMenuSelection = static_cast<MenuOptions>(min(static_cast<int>(soundMenuSelection) + 1, static_cast<int>(menuCancel)));
            lastMoveTime = millis();
        }
    }

    // Update the display only if there was a change
    if (millis() - lastMoveTime <= moveDelay)
    {
        lcd.setCursor(0, 0);
        lcd.print(F("Sound: "));
        lcd.setCursor(0, 1);

        // For 'YES'
        if (soundMenuSelection == menuYes)
        {
            lcd.print((char)2); // Special character for 'YES'
        }
        else
        {
            lcd.print(' ');
        }
        lcd.print(F("YES "));

        // For 'NO'
        if (soundMenuSelection == menuNo)
        {
            lcd.print((char)2); // Special character for 'NO'
        }
        else
        {
            lcd.print(' ');
        }
        lcd.print(F("NO "));

        // For 'CANCEL'
        if (soundMenuSelection == menuCancel)
        {
            lcd.print((char)2); // Special character for 'CANCEL'
        }
        else
        {
            lcd.print(' ');
        }
        lcd.print(F("CANCEL"));
    }

    // Handle button press
    if (joystickButtonPressed())
    {
        if (soundMenuSelection == menuYes)
        {
            soundEnabled = true;
            savesoundsettings();
        }
        else if (soundMenuSelection == menuNo)
        {
            soundEnabled = false;
            // Reset the last move time to prevent accidental double-press
            savesoundsettings();
        }
        // Reset the last move time to prevent accidental double-press
        // For both YES/NO and CANCEL, return to the previous menu
        goBacksettings();   // Assuming this function is defined to handle going back
        hasEntered = false; // Reset the flag when leaving the menu
    }
}
