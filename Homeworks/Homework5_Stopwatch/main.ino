#include "LedControl.h"

// Define enum for game elements
enum GameElement {
  Empty,
  Player,
  BulletType,
  Wall
};
enum Direction {
  Up, Down, Left, Right, None
};
struct Bullet {
  byte x;
  byte y;
  Direction direction;
};
const int maxBullets = 10;
Bullet bullets[maxBullets];
int bulletCount = 0;
Direction lastDirection = Up;
// Pins and matrix configuration
const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;
const byte matrixSize = 8;
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);

// Matrix state
GameElement matrix[matrixSize][matrixSize] = {Empty}; // 8x8 matrix of GameElements

// Player configuration
byte playerX = 4;
byte playerY = 4;
unsigned long playerBlinkInterval = 1000;
unsigned long lastBlinkTime = 0;
bool playerVisible = true;

// Joystick configuration
const int joystickX = A0;
const int joystickY = A1;
int joystickCenter = 512;
int joystickDeadZone = 100;
bool joystickCentered = true;

// Brightness
byte matrixBrightness = 2;

void setup() {
   pinMode(2, INPUT_PULLUP);
  lc.shutdown(0, false);
  lc.setIntensity(0, matrixBrightness);
  lc.clearDisplay(0);
  matrix[playerY][playerX] = Player; // Initialize player position
}

void updateMatrix() {
  unsigned long currentTime = millis();

  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      bool ledState = false;
      switch(matrix[row][col]) {
        case Player:
          ledState = playerVisible;
          break;
        case BulletType:
          // Implement bullet blinking logic here
          ledState = (currentTime / 250) % 2; // Example: Fast blinking
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
void shootBullet() {
  if (bulletCount < maxBullets) {
    Bullet newBullet;
    newBullet.x = playerX;
    newBullet.y = playerY;
    newBullet.direction = lastDirection;

    // Adjust the initial position based on the direction
    switch(newBullet.direction) {
      case Up:
        if(newBullet.y > 0) newBullet.y--;
        break;
      case Down:
        if(newBullet.y < matrixSize - 1) newBullet.y++;
        break;
      case Left:
        if(newBullet.x > 0) newBullet.x--;
        break;
      case Right:
        if(newBullet.x < matrixSize - 1) newBullet.x++;
        break;
      default:
        break;
    }

    // Place the bullet if the position is empty
    if(matrix[newBullet.y][newBullet.x] == Empty) {
      matrix[newBullet.y][newBullet.x] = BulletType;
      bullets[bulletCount++] = newBullet;
    }
  }
}

void moveBullets() {
  for (int i = 0; i < bulletCount; i++) {
    // Save current position
    byte currentX = bullets[i].x;
    byte currentY = bullets[i].y;

    // Calculate next position based on direction
    switch (bullets[i].direction) {
      case Up:
        if (bullets[i].y > 0) bullets[i].y--;
        break;
      case Down:
        if (bullets[i].y < matrixSize - 1) bullets[i].y++;
        break;
      case Left:
        if (bullets[i].x > 0) bullets[i].x--;
        break;
      case Right:
        if (bullets[i].x < matrixSize - 1) bullets[i].x++;
        break;
      default:
        break;
    }

    // Check for collision with wall or boundary
    if (bullets[i].x == currentX && bullets[i].y == currentY || // Didn't move (boundary hit)
        matrix[bullets[i].y][bullets[i].x] == Wall) { // Hit a wall
      // Remove the bullet
      matrix[currentY][currentX] = Empty; // Clear the old bullet position
      bullets[i] = bullets[--bulletCount]; // Replace current bullet with the last one and decrease count
      i--; // Adjust loop index since bullets array is now shorter
    } else {
      // Update matrix with new bullet position
      matrix[currentY][currentX] = Empty; // Clear the old bullet position
      matrix[bullets[i].y][bullets[i].x] = BulletType; // Set new bullet position
    }
  }
}


void loop() {
  // Player blink logic
  if (millis() - lastBlinkTime > playerBlinkInterval) {
    playerVisible = !playerVisible;
    lastBlinkTime = millis();
  }

  // Read joystick input
  int xVal = analogRead(joystickX) - joystickCenter;
  int yVal = analogRead(joystickY) - joystickCenter;

  // Check if joystick is back in the center
  if (abs(xVal) < joystickDeadZone && abs(yVal) < joystickDeadZone) {
    joystickCentered = true;
  }
  if (digitalRead(2) == LOW) {
    shootBullet();
    // Debounce delay to prevent multiple bullets on a single press
    delay(200);
  }
  // Update player position based on joystick input
  if (joystickCentered) {
    byte newX = playerX;
    byte newY = playerY;

    if (abs(xVal) > joystickDeadZone) {
      if (xVal < 0 && playerX > 0) newX--;
      else if (xVal > 0 && playerX < matrixSize - 1) newX++;
      joystickCentered = false;
    }
    if (abs(yVal) > joystickDeadZone) {
      if (yVal < 0 && playerY > 0) newY--;
      else if (yVal > 0 && playerY < matrixSize - 1) newY++;
      joystickCentered = false;
    }

    if(newX != playerX || newY != playerY) {
        if(newX > playerX) lastDirection = Right;
        else if(newX < playerX) lastDirection = Left;
        else if(newY > playerY) lastDirection = Down;
        else if(newY < playerY) lastDirection = Up;
      matrix[playerY][playerX] = Empty; // Clear the old player position
      playerX = newX;
      playerY = newY;
      matrix[playerY][playerX] = Player; // Set the new player position
    }
  }
 moveBullets();
  updateMatrix(); // Refresh the LED matrix
  delay(100); // Small delay for stability
}
