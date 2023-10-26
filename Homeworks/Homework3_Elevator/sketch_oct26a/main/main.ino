const int PIN_FLOOR_1_BUTTON = 2;
const int PIN_FLOOR_2_BUTTON = 3;
const int PIN_FLOOR_3_BUTTON = 4;

const int PIN_FLOOR_1_LED = 8;
const int PIN_FLOOR_2_LED = 9;
const int PIN_FLOOR_3_LED = 10;

const int PIN_BUZZER = 11;
const int PIN_CONTROLLED_LED = 12;

const int debounceDelay = 100; // 100 ms

enum ElevatorState
{
  CLOSING_DOORS,
  MOVING,
  STATIONARY
};

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
class Updatable
{
public:
  virtual void update() = 0;
};

class FlickerLed : public Updatable
{
protected:
  int ledPin;
  unsigned long lastFlickerTime;
  bool flickerValue;

public:
  FlickerLed(int ledPin)
  {
    this->ledPin = ledPin;
    this->lastFlickerTime = 0;
    this->flickerValue = false;
    pinMode(ledPin, OUTPUT);
  }
  void update() override
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
  int buzzerPin;

public:
  ControlPanel(int ledPin, int buzzerPin) : FlickerLed(ledPin)
  {
    this->isStationary = true;
    this->buzzerPin = buzzerPin;
  }
  void update() override
  {
    if (isStationary)
    {
      digitalWrite(ledPin, HIGH);
      digitalWrite(buzzerPin, LOW);
    }
    else
    {
      digitalWrite(ledPin, HIGH);
      FlickerLed::update();
    }
  }
  void toggleMode()
  {
    isStationary = !isStationary;
  }
};

ControlPanel panel(PIN_CONTROLLED_LED, PIN_BUZZER);

class FloorControl : public FlickerLed
{
private:
  int buttonPin;
  bool lastReading;
  unsigned long lastDebounceTime;

public:
  FloorControl(int buttonPin, int ledPin) : FlickerLed(ledPin)
  {
    this->buttonPin = buttonPin;
    this->lastDebounceTime = 0;
    lastReading = false;
    pinMode(buttonPin, INPUT_PULLUP);
  }
  bool read()
  {
    bool reading = !digitalRead(buttonPin);

    if (reading != lastReading)
    {
      if ((millis() - lastDebounceTime) > debounceDelay)
      {
        lastReading = reading;
         lastDebounceTime = millis();
      }
    }

    return lastReading;
  }
  void write(bool value)
  {
    digitalWrite(ledPin, value);
  }
  void closeDoors()
  {
    FlickerLed::update();
  }
};

FloorControl floor1(PIN_FLOOR_1_BUTTON, PIN_FLOOR_1_LED);
FloorControl floor2(PIN_FLOOR_2_BUTTON, PIN_FLOOR_2_LED);
FloorControl floor3(PIN_FLOOR_3_BUTTON, PIN_FLOOR_3_LED);

class Elevator : public Updatable
{
private:
  int currentFloor;
  int targetFloor;
  ElevatorState state;
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
    state = ElevatorState::STATIONARY;
  }
  void update() override
  {

    switch (state)
    {
    case ElevatorState::STATIONARY:
      read();
      break;
    case ElevatorState::CLOSING_DOORS:
      closeDoors();
      break;
    case ElevatorState::MOVING:
      moveElevator();
      break;
    default:
      break;
    }
    write();
  }

  void closeDoors()
  {
    if (millis() - lastElevatorMoveTime > 1000)
    {
      state = ElevatorState::MOVING;
      lastElevatorMoveTime = millis();
    }
  }

  void moveElevator()
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
        state = ElevatorState::STATIONARY;
        panel->toggleMode();
      }
      lastElevatorMoveTime = millis();
    }
  }

  void read()
  {
    for (int i = 0; i < 3; i++)
    {
      if (floors[i]->read())
      {
        targetFloor = i;
      }
      if (targetFloor != currentFloor)
      {
        state = ElevatorState::CLOSING_DOORS;
        panel->toggleMode();
        lastElevatorMoveTime = millis();
      }
    }
  }

  void write()
  {
    for (int i = 0; i < 3; i++)
    {

      if (state == ElevatorState::CLOSING_DOORS)
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
