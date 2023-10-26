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

class FlickerLed
{
protected:
  int ledPin;
  int lastFlickerTime;
  bool flickerValue;

public:
  FlickerLed(int ledPin)
  {
    this->ledPin = ledPin;
    pinMode(ledPin, OUTPUT);
    this->lastFlickerTime = 0;
    this->flickerValue = false;
  }
  void update()
  {
    if (millis() - lastFlickerTime > 200)
    {
      flickerValue = !flickerValue;
      lastFlickerTime = millis();
      digitalWrite(ledPin, flickerValue);
    }
  }
};

class ControlPanel : public FlickerLed
{
private:
  bool isStationary;

public:
  ControlPanel(int ledPin) : FlickerLed(ledPin)
  {
    this->isStationary = true;
  }
  void update()
  {
    if (isStationary)
    {
      digitalWrite(ledPin, HIGH);
    }
    else
    {
      FlickerLed::update();
    }
  }
  void toggleMode()
  {
    isStationary = !isStationary;
  }
};

ControlPanel panel(PIN_CONTROLLED_LED);

class FloorControl : public FlickerLed
{
public:
  int buttonPin;

public:
  FloorControl(int buttonPin, int ledPin) : FlickerLed(ledPin)
  {
    this->buttonPin = buttonPin;
    pinMode(buttonPin, INPUT_PULLUP);
  }
  bool read()
  {
    return !digitalRead(buttonPin);
  }
  void write(bool value)
  {
    digitalWrite(ledPin, value);
  }
  void closeDoors()
  {
    Serial.println("closing doors");
    FlickerLed::update();
  }
};

FloorControl floor1(PIN_FLOOR_1_BUTTON, PIN_FLOOR_1_LED);
FloorControl floor2(PIN_FLOOR_2_BUTTON, PIN_FLOOR_2_LED);
FloorControl floor3(PIN_FLOOR_3_BUTTON, PIN_FLOOR_3_LED);

class Elevator
{
private:
  int currentFloor;
  int targetFloor;
  bool isMoving;
  bool closingDoors;
  unsigned long lastElevatorMoveTime;
  FloorControl *floors[3];
  ControlPanel *panel;

public:
  Elevator(FloorControl *floor1, FloorControl *floor2, FloorControl *floor3, ControlPanel *_panel)
  {
    currentFloor = 0;
    targetFloor = 0;
    floors[0] = floor1;
    floors[1] = floor2;
    floors[2] = floor3;
    panel = _panel;
    lastElevatorMoveTime = 0;
    isMoving = false;
    closingDoors = false;
  }
  void update()
  {
    if (!isMoving)
    {

      for (int i = 0; i < 3; i++)
      {
        // in this order we also guarantee that the first floor has priority
        if (floors[i]->read())
        {
          targetFloor = i;
        }
        if (targetFloor != currentFloor)
        {
          isMoving = true;
          closingDoors = true;
          panel->toggleMode();
          lastElevatorMoveTime = millis();
        }
      }
    }
    else if (closingDoors)
    {
      if (millis() - lastElevatorMoveTime > 1000)
      {
        closingDoors = false;
        lastElevatorMoveTime = millis();
      }
    }
    else
    {

      if (millis() - lastElevatorMoveTime > 2000)
      {
        if (currentFloor < targetFloor)
        {
          currentFloor++;
        }
        else
        {
          currentFloor--;
        }
        if (currentFloor == targetFloor)
        {
          isMoving = false;
          panel->toggleMode();
        }
        lastElevatorMoveTime = millis();
      }
    }
    write();
  }

  void write()
  {
    for (int i = 0; i < 3; i++)
    {
      if (closingDoors)
      {
        floors[currentFloor]->closeDoors();
      }
      else if (i == currentFloor)
      {
        floors[i]->write(true);
      }
      else
        floors[i]->write(false);
    }
    panel->update();
  }
};

Elevator elevator(&floor1, &floor2, &floor3, &panel);
void setup()
{
  Serial.begin(9600);
}

void loop()
{
  elevator.update();
  delay(10);
}
