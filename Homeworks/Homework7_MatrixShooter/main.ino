#include "LedControl.h"

// Define enum for game elements
enum GameElement
{
  Empty,
  PlayerType,
  BulletType,
  Wall
};
enum Direction
{
  Up,
  Down,
  Left,
  Right,
  None
};
const int buzzerPin = 8;
struct Bullet
{
  byte x;
  byte y;
  bool state;
  Direction direction;
};

struct Player
{
  byte x;
  byte y;
  bool state;
  Direction lastDirection;
};

const int maxBullets = 10;
Bullet bullets[maxBullets];
int bulletCount = 0;
// Pins and matrix configuration
const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;
const byte matrixSize = 8;
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);

// Matrix state
GameElement matrix[matrixSize][matrixSize] = {Empty}; // 8x8 matrix of GameElements

// Player configuration

Player player = {4, 4, true, Up};

unsigned long bulletBlinkPeriodFrame = 1;
unsigned long playerBlinkPeriodFrame = 50; // at every 50th frame it should blink
unsigned long lastBlinkTime = 0;

// Joystick configuration
const int joystickX = A0;
const int joystickY = A1;
int joystickCenter = 512;
int joystickDeadZone = 100;
bool joystickCentered = true;

// Brightness
byte matrixBrightness = 3;
unsigned long currentFrame = 0;

const uint64_t smileyFace = 0x007e420024240000; // Smiley face bitmap
const uint64_t goText = 0x0077555751730000;     // "GO" text bitmap
void displayImage(uint64_t image)
{
  for (int row = 0; row < matrixSize; row++)
  {
    for (int col = 0; col < matrixSize; col++)
    {
      // Calculate the position of the bit in the bitmap
      uint64_t bit = 1ULL << (row * matrixSize + col);
      lc.setLed(0, row, col, (image & bit) != 0);
    }
  }
}

bool startGame = false;
void setup()
{
  pinMode(buzzerPin, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  lc.shutdown(0, false);
  lc.setIntensity(0, matrixBrightness);
  lc.clearDisplay(0);
  randomSeed(analogRead(0));
  generateWalls(20);
  playStartSound();
  displayImage(goText);
  matrix[player.x][player.y] = PlayerType; // Initialize player position
}

void playShootSound()
{
  tone(buzzerPin, 1000, 100); // Play a tone at 1000 Hz for 100 ms
}

void playWinSound()
{
  tone(buzzerPin, 2000, 500); // Play a higher tone at 2000 Hz for 500 ms
}

void playStartSound()
{
  tone(buzzerPin, 500, 500); // Play a lower tone at 500 Hz for 500 ms
}
void generateWalls(int numberOfWalls)
{
  int placedWalls = 0;
  while (placedWalls < numberOfWalls)
  {
    int randX = random(matrixSize);
    int randY = random(matrixSize);

    // Check if the position is not in the center and is empty
    if (!((randX > 2 && randX < 6) && (randY > 2 && randY < 6)) && matrix[randX][randY] == Empty)
    {
      matrix[randX][randY] = Wall;
      placedWalls++;
    }
  }
}

void updateMatrix()
{
  unsigned long currentTime = millis();
  currentFrame++;
  for (int row = 0; row < matrixSize; row++)
  {
    for (int col = 0; col < matrixSize; col++)
    {
      bool ledState = false;
      switch (matrix[row][col])
      {
      case PlayerType:
        ledState = player.state;
        player.state = (currentFrame / playerBlinkPeriodFrame) % 2;
        break;
      case BulletType:
        // Implement bullet blinking logic here
        ledState = (currentFrame / bulletBlinkPeriodFrame) % 3; // Example: Fast blinking
        break;
      case Wall:
        ledState = true; // Walls are always visible (not blinking)
        break;
      default:
        ledState = false; // Empty space
      }
      lc.setLed(0, row, col, ledState);
    }
  }
}
void shootBullet()
{
  playShootSound();
  if (bulletCount < maxBullets)
  {
    Bullet newBullet;
    newBullet.x = player.x;
    newBullet.y = player.y;
    newBullet.direction = player.lastDirection;

    switch (newBullet.direction)
    {
    case Up:
      if (newBullet.y > 0)
        newBullet.y--; // Move bullet one step up
      break;
    case Down:
      if (newBullet.y < matrixSize - 1)
        newBullet.y++; // Move bullet one step down
      break;
    case Left:
      if (newBullet.x > 0)
        newBullet.x--; // Move bullet one step left
      break;
    case Right:
      if (newBullet.x < matrixSize - 1)
        newBullet.x++; // Move bullet one step right
      break;
    default:
      break;
    }
    matrix[newBullet.y][newBullet.x] = BulletType;
    bullets[bulletCount++] = newBullet;
  }
}

bool gameOver = false;
void checkForRemainingWalls()
{
  for (int row = 0; row < matrixSize; row++)
  {
    for (int col = 0; col < matrixSize; col++)
    {
      if (matrix[row][col] == Wall)
      {
        return; // Wall found, game not finished
      }
    }
  }
  gameOver = true;
  return; // No walls found, game finished
}

void moveBullets()
{
  for (int i = 0; i < bulletCount; i++)
  {
    // Save current position
    byte currentX = bullets[i].x;
    byte currentY = bullets[i].y;

    // Calculate next position based on direction
    switch (bullets[i].direction)
    {
    case Up:
      if (bullets[i].y > 0)
        bullets[i].y--;
      break;
    case Down:
      if (bullets[i].y < matrixSize - 1)
        bullets[i].y++;
      break;
    case Left:
      if (bullets[i].x > 0)
        bullets[i].x--;
      break;
    case Right:
      if (bullets[i].x < matrixSize - 1)
        bullets[i].x++;
      break;
    default:
      break;
    }

    // Check for collision with wall or boundary
    if (bullets[i].x == currentX && bullets[i].y == currentY)
    { // Bullet hit the boundary
      // Remove the bullet
      matrix[currentY][currentX] = Empty;
      bullets[i] = bullets[--bulletCount];
      i--;
    }
    else if (matrix[bullets[i].y][bullets[i].x] == Wall)
    { // Bullet hit a wall
      // Remove the bullet and the wall
      matrix[currentY][currentX] = Empty;         // Clear the old bullet position
      matrix[bullets[i].y][bullets[i].x] = Empty; // Destroy the wall
      bullets[i] = bullets[--bulletCount];        // Replace current bullet with the last one and decrease count
      i--;
    }
    else
    {
      // Update matrix with new bullet position
      matrix[currentY][currentX] = Empty;
      matrix[bullets[i].y][bullets[i].x] = BulletType;
    }
  }
}

int lastShootTime = 0;
int lastFrameTime = 0;
const int periodOfFrames = 10;

void movePlayer()
{
  int xVal = analogRead(joystickX) - joystickCenter;
  int yVal = analogRead(joystickY) - joystickCenter;

  // Check if joystick is back in the center
  if (abs(xVal) < joystickDeadZone && abs(yVal) < joystickDeadZone)
  {
    joystickCentered = true;
  }

  // Update player position based on joystick input
  if (joystickCentered)
  {
    byte newX = player.x;
    byte newY = player.y;

    if (abs(xVal) > joystickDeadZone)
    {
      if (xVal < 0 && player.x > 0)
        newX--;
      else if (xVal > 0 && player.x < matrixSize - 1)
        newX++;
      joystickCentered = false;
    }
    if (abs(yVal) > joystickDeadZone)
    {
      if (yVal < 0 && player.y > 0)
        newY--;
      else if (yVal > 0 && player.y < matrixSize - 1)
        newY++;
      joystickCentered = false;
    }

    // Update direction based on movement

    if (newX > player.x)
      player.lastDirection = Right;
    else if (newX < player.x)
      player.lastDirection = Left;
    else if (newY > player.y)
      player.lastDirection = Down;
    else if (newY < player.y)
      player.lastDirection = Up;
    // Check if the new position is a wall
    if (matrix[newY][newX] != Wall)
    {
      // Update player position
      matrix[player.y][player.x] = Empty; // Clear the old player position
      player.x = newX;
      player.y = newY;
      matrix[player.y][player.x] = PlayerType; // Set the new player position
    }
  }
}

void loop()
{

  if (!startGame && millis() > 1000)
  {
    startGame = true;
  }
  else if (gameOver)
  {
    playWinSound();
    displayImage(smileyFace);
    return;
  }
  else if (startGame)
  { // Check if enough time has passed since the last frame
    if (millis() - lastFrameTime < periodOfFrames)
      return;
    else
    {
      lastFrameTime = millis();
      // Read joystick input
      updateMatrix(); // Refresh the LED matrix
      movePlayer();
      moveBullets();
      if (digitalRead(2) == LOW && millis() - lastShootTime > 200)
      {
        shootBullet();
        lastShootTime = millis();
        // Debounce delay to prevent multiple bullets on a single press
      }
      checkForRemainingWalls();
    }
  }
}
