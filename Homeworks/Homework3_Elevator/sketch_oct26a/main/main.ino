const int numberOfFloors = 3;

const int PIN_FLOOR_BUTTON[numberOfFloors] = {2, 3, 4};
const int PIN_FLOOR_LED[numberOfFloors] = {8, 9, 10};

const int PIN_BUZZER = 11;
const int PIN_CONTROLLED_LED = 12;

const int debounceDelay = 100;       // 100 ms
const int doorsOperationTime = 1000; // 1 second

const int CLOSING_TONE_FREQ = 1000;
const int ARRIVAL_TONE_FREQ = 2000;
const int MOVING_TONE_FREQ = 1500;

enum ElevatorState
{
  CLOSING_DOORS,
  MOVING,
  STATIONARY
};

class FloorQueue
{
private:
  int queue[numberOfFloors];
  int front;
  int rear;

public:
  FloorQueue() : front(-1), rear(-1) {}

  bool isFull()
  {
    return (rear + 1) % numberOfFloors == front;
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
    for (int i = front; i != rear; i = (i + 1) % numberOfFloors)
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
    rear = (rear + 1) % numberOfFloors;
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
      front = (front + 1) % numberOfFloors;
    }
    return floor;
  }
};

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
    tone(buzzerPin, ARRIVAL_TONE_FREQ, doorsOperationTime);
  }

  void playClosingTone()
  {
    tone(buzzerPin, CLOSING_TONE_FREQ, doorsOperationTime);
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

class Elevator : public Updatable
{
private:
  int currentFloor;
  int targetFloor;
  ElevatorState state;
  unsigned long lastElevatorMoveTime;
  unsigned long lastElevatorOpeningTime;
  FloorControl *floors[numberOfFloors];
  ControlPanel *panel;
  FloorQueue floorQueue;

public:
  Elevator(FloorControl *_floors[numberOfFloors], ControlPanel *_panel)
  {
    currentFloor = 0;
    targetFloor = 0;
    for (int i = 0; i < numberOfFloors; i++)
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
    if (millis() - lastElevatorMoveTime > doorsOperationTime)
    {
      state = ElevatorState::MOVING;
      panel->startMovingTone();
      lastElevatorMoveTime = millis();
    }
  }

  void moveElevator()
  {
    if (millis() - lastElevatorMoveTime > doorsOperationTime)
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
    for (int i = 0; i < numberOfFloors; i++)
    {
      if (floors[i]->read())
      {
        floorQueue.enqueue(i);
      }
    }

    if (millis() - lastElevatorOpeningTime > doorsOperationTime * 2

        && !floorQueue.isEmpty()

        && (state == ElevatorState::STATIONARY || (state == ElevatorState::MOVING && currentFloor == targetFloor)))
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
    for (int i = 0; i < numberOfFloors; i++)
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
  FloorControl *floors[numberOfFloors];

  for (int i = 0; i < numberOfFloors; i++)
  {
    floors[i] = new FloorControl(PIN_FLOOR_BUTTON[i], PIN_FLOOR_LED[i]);
  }

  ControlPanel panel(PIN_CONTROLLED_LED, PIN_BUZZER);
  elevator = new Elevator(floors, &panel);
  Serial.begin(9600);
}

void loop()
{
  elevator->update();
  delay(10);
}
