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

unsigned long bulletBlinkPeriodFrame = 2;
unsigned long playerBlinkPeriodFrame = 50; // at every 50th frame it should blink
unsigned long lastBlinkTime = 0;

// Joystick configuration
const int joystickX = A0;
const int joystickY = A1;
int joystickCenter = 512;
int joystickDeadZone = 100;
bool joystickCentered = true;

// Brightness
byte matrixBrightness = 2;
unsigned long currentFrame = 0;
void setup()
{
  pinMode(2, INPUT_PULLUP);
  lc.shutdown(0, false);
  lc.setIntensity(0, matrixBrightness);
  lc.clearDisplay(0);
  matrix[player.x][player.y] = PlayerType; // Initialize player position
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
        ledState = (currentFrame / bulletBlinkPeriodFrame) % 2; // Example: Fast blinking
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

    // Place the bullet if the position is empty
    if (matrix[newBullet.y][newBullet.x] == Empty)
    {
      matrix[newBullet.y][newBullet.x] = BulletType;
      bullets[bulletCount++] = newBullet;
    }
  }
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
    if (bullets[i].x == currentX && bullets[i].y == currentY || // Didn't move (boundary hit)
        matrix[bullets[i].y][bullets[i].x] == Wall)
    { // Hit a wall
      // Remove the bullet
      matrix[currentY][currentX] = Empty;  // Clear the old bullet position
      bullets[i] = bullets[--bulletCount]; // Replace current bullet with the last one and decrease count
      i--;                                 // Adjust loop index since bullets array is now shorter
    }
    else
    {
      // Update matrix with new bullet position
      matrix[currentY][currentX] = Empty;              // Clear the old bullet position
      matrix[bullets[i].y][bullets[i].x] = BulletType; // Set new bullet position
    }
  }
}

int lastShootTime = 0;
int lastFrameTime = 0;
const int periodOfFrames = 10;

void loop()
{
  // Check if enough time has passed since the last frame
  if (millis() - lastFrameTime < periodOfFrames)
    return;
  else
  {
    lastFrameTime = millis();
    // Read joystick input
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

      if (newX != player.x || newY != player.y)
      {
        if (newX > player.x)
          player.lastDirection = Right;
        else if (newX < player.x)
          player.lastDirection = Left;
        else if (newY > player.y)
          player.lastDirection = Down;
        else if (newY < player.y)
          player.lastDirection = Up;
        matrix[player.y][player.x] = Empty; // Clear the old player position
        player.x = newX;
        player.y = newY;
        matrix[player.y][player.x] = PlayerType; // Set the new player position
      }
    }
    updateMatrix(); // Refresh the LED matrix
    moveBullets();
    if (digitalRead(2) == LOW && millis() - lastShootTime > 200)
    {
      shootBullet();
      lastShootTime = millis();
      // Debounce delay to prevent multiple bullets on a single press
    }
    // Small delay for stability
  }
}
