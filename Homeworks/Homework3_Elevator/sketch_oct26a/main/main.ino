const int NUMBER_OF_FLOORS = 3;

// THE ORDER OF PINS IS IMPORTANT
// I WILL USE THESE PINS TO INITIALIZE THE FLOOR BUTTONS AND LEDS
const int PIN_FLOOR_BUTTON[NUMBER_OF_FLOORS] = {2, 3, 4};
const int PIN_FLOOR_LED[NUMBER_OF_FLOORS] = {8, 9, 10};

// I WILL USE THESE PINS TO INITIALIZE THE CONTROL PANEL
// THE CONTROL PANEL HAS A LED AND A BUZZER TO INDICATE THE STATE OF THE ELEVATOR
const int PIN_BUZZER = 11;
const int PIN_CONTROLLED_LED = 12;

const int DEBOUNCE_DELAY = 100; // 100 ms
// FOR EITHER OPENING OR CLOSING DOORS THE TIME IS THE SAME
const int DOORS_OPERATING_TIME = 1000; // 1 second

// THE FREQUENCIES OF THE TONES ARE ARBITRARY
const int CLOSING_TONE_FREQ = 1000;
const int ARRIVAL_TONE_FREQ = 2000;
const int MOVING_TONE_FREQ = 1500;

enum ElevatorState
{
  CLOSING_DOORS,
  MOVING,
  STATIONARY
};

// TO KEEP TRACK OF THE PRESSES OF THE BUTTONS
// IT DOESNT MATTER HOW MANY TIMES A USER PRESSES THE SAME LEVEL BUTTON
// THAT IS WHY I MAKE SURE I KEEP THEM UNIQUE AND ALSO IN THE ORDER THEY WERE PRESSED
class FloorQueue
{
private:
  int queue[NUMBER_OF_FLOORS];
  int front;
  int rear;

public:
  FloorQueue() : front(-1), rear(-1) {}

  bool isFull()
  {
    return (rear + 1) % NUMBER_OF_FLOORS == front;
  }

  bool isEmpty()
  {
    return front == -1;
  }

  bool contains(int floor)
  {
    if (isEmpty())
    {
      return false;
    }
    for (int i = front; i != rear; i = (i + 1) % NUMBER_OF_FLOORS)
    {
      if (queue[i] == floor)
      {
        return true;
      }
    }
    return queue[rear] == floor;
  }

  void enqueue(int floor)
  {
    if (isFull() || contains(floor))
    {
      return;
    }
    if (isEmpty())
    {
      front = 0;
    }
    rear = (rear + 1) % NUMBER_OF_FLOORS;
    queue[rear] = floor;
  }

  int dequeue()
  {
    if (isEmpty())
    {
      return -1;
    }
    int floor = queue[front];
    if (front == rear)
    {
      front = rear = -1;
    }
    else
    {
      front = (front + 1) % NUMBER_OF_FLOORS;
    }
    return floor;
  }
};

// JUST AN ABSTRACT CLASS
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

// IN MY VISION A CONTROL PANEL IS INSIDE THE ELEVATOR, SO IT IS A SPEAKER WITH A LED THAT SHOWS
// THE PEOPLE INSIDE WHAT TO DO
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
    pinMode(buzzerPin, OUTPUT);
  }
  void update() override
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
  void playArrivalTone()
  {
    tone(buzzerPin, ARRIVAL_TONE_FREQ, DOORS_OPERATING_TIME);
  }

  void playClosingTone()
  {
    tone(buzzerPin, CLOSING_TONE_FREQ, DOORS_OPERATING_TIME);
  }

  void startMovingTone()
  {
    tone(buzzerPin, MOVING_TONE_FREQ);
  }

  void stopTone()
  {
    noTone(buzzerPin);
  }
};

// ON THE OTHER HAND, A FLOOR CONTROL PANEL IS OUTSIDE THE ELEVATOR, SO IT IS JUST A BUTTON AND A LED
// SO THE USER CAN SEE IF IT HAS ARRIVED AND CALL IT
class FloorControlPanel : public FlickerLed
{
private:
  int buttonPin;
  bool lastReading;
  unsigned long lastDebounceTime;

public:
  FloorControlPanel(int buttonPin, int ledPin) : FlickerLed(ledPin)
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
      if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY)
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

// THE ELEVATOR IS THE MAIN CLASS, IT HAS A CONTROL PANEL AND A FLOOR CONTROL PANEL
class Elevator : public Updatable
{
private:
  // PRETTY OBVIOUS
  int currentFloor;
  int targetFloor;
  ElevatorState state;

  unsigned long lastElevatorMoveTime;
  unsigned long lastElevatorOpeningTime;

  // THE GADGETS
  FloorControlPanel *floors[NUMBER_OF_FLOORS];
  ControlPanel *panel;

  FloorQueue floorQueue;

public:
  Elevator(FloorControlPanel *_floors[NUMBER_OF_FLOORS], ControlPanel *_panel)
  {
    currentFloor = 0;
    targetFloor = 0;
    for (int i = 0; i < NUMBER_OF_FLOORS; i++)
    {
      floors[i] = _floors[i];
    }
    panel = _panel;
    lastElevatorMoveTime = 0;
    lastElevatorOpeningTime = 0;
    state = ElevatorState::STATIONARY;
  }
  void update() override
  {
    switch (state)
    {
    case ElevatorState::STATIONARY:
      // in case I change my mind, bcs until now it was necessary to read single input, but wiht queue it is not
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

    read();
    write();
  }

  void closeDoors()
  {
    if (millis() - lastElevatorMoveTime > DOORS_OPERATING_TIME)
    {
      state = ElevatorState::MOVING;
      panel->startMovingTone();
      lastElevatorMoveTime = millis();
    }
  }

  void moveElevator()
  {
    if (millis() - lastElevatorMoveTime > DOORS_OPERATING_TIME)
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
        lastElevatorOpeningTime = millis();
        state = ElevatorState::STATIONARY;
        panel->stopTone();
        panel->playArrivalTone();
        panel->toggleMode();
      }

      lastElevatorMoveTime = millis();
    }
  }

  void read()
  {
    for (int i = 0; i < NUMBER_OF_FLOORS; i++)
    {
      if (floors[i]->read())
      {
        floorQueue.enqueue(i);
      }
    }

    // MAYBE YOU ARE ASKING WHY DOUBLE THE TIME? BCS THE DOORS ARE OPENING AND CLOSING
    if (millis() - lastElevatorOpeningTime > DOORS_OPERATING_TIME * 2

        && !floorQueue.isEmpty()

        && (state == ElevatorState::STATIONARY))
    {
      targetFloor = floorQueue.dequeue();
      if (targetFloor != currentFloor)
      {
        state = ElevatorState::CLOSING_DOORS;
        panel->toggleMode();
        panel->playClosingTone();
        lastElevatorMoveTime = millis();
      }
    }
  }

  void write()
  {
    Serial.println(currentFloor);
    for (int i = 0; i < NUMBER_OF_FLOORS; i++)
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

Elevator *elevator;

void setup()
{
  FloorControlPanel *floors[NUMBER_OF_FLOORS];

  for (int i = 0; i < NUMBER_OF_FLOORS; i++)
  {
    floors[i] = new FloorControlPanel(PIN_FLOOR_BUTTON[i], PIN_FLOOR_LED[i]);
  }

  ControlPanel panel(PIN_CONTROLLED_LED, PIN_BUZZER);
  elevator = new Elevator(floors, &panel);
  Serial.begin(9600);
}

void loop()
{
  elevator->update();
  // I use this display just so I dont broke the arduino
  delay(10);
}
