const int PIN_FLOOR_1_BUTTON = 2;
const int PIN_FLOOR_2_BUTTON = 3;
const int PIN_FLOOR_3_BUTTON = 4;

const int PIN_FLOOR_1_LED = 8;
const int PIN_FLOOR_2_LED = 9;
const int PIN_FLOOR_3_LED = 10;

const int PIN_BUZZER = 11;
const int PIN_CONTROLLED_LED = 12;

const int debounceDelay = 50; // 50 ms

// class Elevator
// {
// private:
//   int currentFloor;
//   int targetFloor;
//   Floor floors[3];

// public:
//   Elevator(
//       int floor1ButtonPin, int floor1LedPin,
//       int floor2ButtonPin, int floor2LedPin,
//       int floor3ButtonPin, int floor3LedPin)
//   {
//     currentFloor = 0;
//     targetFloor = 0;
//     floors[0] = Floor(floor1ButtonPin, floor1LedPin);
//     floors[1] = Floor(floor2ButtonPin, floor2LedPin);
//     floors[2] = Floor(floor3ButtonPin, floor3LedPin);
//   }
//   void update()
//   {
//     for (int i = 0; i < 3; i++)
//     {
//       if (floors[i].read())
//       {
//         targetFloor = i;
//       }
//     }
//     if (currentFloor != targetFloor)
//     {
//       if (currentFloor < targetFloor)
//       {
//         currentFloor++;
//       }
//       else
//       {
//         currentFloor--;
//       }
//     }
//     for (int i = 0; i < 3; i++)
//     {
//       digitalWrite(floors[i].ledPin, i == currentFloor);
//     }
//   }

// }

class ControlPanel
{
private:
  int ledPin;
  bool isStationary;
  int lastFlickerTime;

public:
  ControlPanel(int ledPin)
  {
    this->ledPin = ledPin;
    this->isStationary = true;
    this->lastFlickerTime = 0;
    pinMode(ledPin, OUTPUT);
  }
  void flicker_controlled_led()
  {
    if (milis() - lastFlickerTime > 1000)
    {
      isStationary = !isStationary;
      lastFlickerTime = milis();
      digitalWrite(PIN_CONTROLLED_LED, isStationary);
    }
  }
  void toggleMode()
  {
  }
}

class Floor
{
public:
  int buttonPin;
  int ledPin;

public:
  Floor(int buttonPin, int ledPin)
  {
    this->buttonPin = buttonPin;
    this->ledPin = ledPin;
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(ledPin, OUTPUT);
  }
  bool read()
  {
    return !digitalRead(buttonPin);
  }
  void write(bool value)
  {
    digitalWrite(ledPin, value);
  }
};

Floor floor1(PIN_FLOOR_1_BUTTON, PIN_FLOOR_1_LED);
Floor floor2(PIN_FLOOR_2_BUTTON, PIN_FLOOR_2_LED);

class Elevator
{
private:
  int currentFloor;
  int targetFloor;
  bool isMoving;
  int lastElevatorMoveTime;
  Floor *floors[2];

public:
  Elevator(Floor *floor1, Floor *floor2)
  {
    currentFloor = 0;
    targetFloor = 0;
    floors[0] = floor1;
    floors[1] = floor2;
    lastElevatorMoveTime = 0;
    isMoving = false;
  }
  void update()
  {
    if (!isMoving)
    {
      for (int i = 0; i < 2; i++)
      {
        // in this order we also guarantee that the first floor has priority
        if (floors[i]->read())
        {
          targetFloor = i;
        }
        if (targetFloor != currentFloor)
        {
          isMoving = true;
        }
      }
    }
    else
    {
      if (millis() - lastElevatorMoveTime > 1000)
      {
        if (currentFloor < targetFloor)
        {
          currentFloor++;
        }
        else
        {
          currentFloor--;
        }
        lastElevatorMoveTime = millis();
        if (currentFloor == targetFloor)
        {
          isMoving = false;
        }
      }
    }
    write();
  }

  void write()
  {
    for (int i = 0; i < 2; i++)
    {
      floors[i]->write(i == currentFloor);
    }
  }
}

Elevator elevator(&floor1, &floor2);
void setup()
{
  Serial.begin(9600);
}

void loop()
{
  elevator.update();
  delay(10);
}
