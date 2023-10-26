const int PIN_FLOOR_1_BUTTON = 2;
const int PIN_FLOOR_2_BUTTON = 3;
const int PIN_FLOOR_3_BUTTON = 4;

const int PIN_FLOOR_1_LED = 8;
const int PIN_FLOOR_2_LED = 9;
const int PIN_FLOOR_3_LED = 10;


const int debounceDelay=50; // 50 ms


class Queue {
  private:
    const static int QUEUE_SIZE = 10;
    int queue[QUEUE_SIZE];
    int front;
    int rear;

  public:
    Queue() : front(0), rear(0) {}

    bool enqueue(int value) {
      int nextRear = (rear + 1) % QUEUE_SIZE;
      if (nextRear != front) {  // Check if queue is full
        queue[rear] = value;
        rear = nextRear;
        return true;
      } else {
        Serial.println("Queue is full");
        return false;
      }
    }

    bool dequeue(int *value) {
      if (front != rear) {  // Check if queue is empty
        *value = queue[front];
        front = (front + 1) % QUEUE_SIZE;
        return true;
      } else {
        Serial.println("Queue is empty");
        return false;
      }
    }

    bool isEmpty() {
      return front == rear;
    }

    bool isFull() {
      return ((rear + 1) % QUEUE_SIZE) == front;
    }
};


Queue elevatorCalls;

class Floor {
  private:
    int buttonPin;
    int ledPin;
    int buttonState;
    int lastButtonState;
    unsigned long lastDebounceTime;
    bool toggleState;
    int level;

  public:
    Floor(int _buttonPin, int _ledPin, int _level) : 
        buttonPin(_buttonPin), ledPin(_ledPin), 
        buttonState(LOW), lastButtonState(LOW), 
        lastDebounceTime(0), toggleState(false), level(_level) {
      pinMode(buttonPin, INPUT_PULLUP);
      pinMode(ledPin, OUTPUT);
    }
    void setLED(bool state) {
      digitalWrite(ledPin, state ? HIGH : LOW);
    }
    void update() {
      int reading = !digitalRead(buttonPin);

      if (reading != lastButtonState) {
        lastDebounceTime = millis();
      }

      if ((millis() - lastDebounceTime) > debounceDelay) {
    if (buttonState == HIGH && lastButtonState != buttonState) {
        elevator.callTo(level);
      }
      }
      lastButtonState = reading;
    }

    bool isPressed() {
      return buttonState == HIGH;
    }

    bool isToggled() {
      return toggleState;
    }

    int getButtonPin() {
      return buttonPin;
    }

    int getLedPin() {
      return ledPin;
    }

    void clearCall() {
    setLED(false);
}
};


class Elevator {
  private:
    int currentFloor;
    int targetFloor;
    bool isMoving;
    unsigned long lastMoveTime;
    Floor* floors[3];

  public:
    Elevator(Floor* floor1, Floor* floor2, Floor* floor3) 
        : currentFloor(1), targetFloor(1), isMoving(false), lastMoveTime(0) {
        floors[0] = floor1;
        floors[1] = floor2;
        floors[2] = floor3;
    }

    void callTo(int floor) {
      targetFloor = floor;
      if (currentFloor != targetFloor) {
        isMoving = true;
        lastMoveTime = millis();
      }
    }

    void update() {
      if (isMoving && (millis() - lastMoveTime >= 2000)) {  // 2 seconds interval
        if (currentFloor < targetFloor) {
          currentFloor++;
        } else {
          currentFloor--;
        }
        lastMoveTime = millis();
        if (currentFloor == targetFloor) {
          isMoving = false;
         floors[currentFloor - 1]->clearCall();
        }
      }
    }

    int getCurrentFloor() {
      return currentFloor;
    }
};

Floor floor1(PIN_FLOOR_1_BUTTON, PIN_FLOOR_1_LED,1);
Floor floor2(PIN_FLOOR_2_BUTTON, PIN_FLOOR_2_LED,2);
Floor floor3(PIN_FLOOR_3_BUTTON, PIN_FLOOR_3_LED,3);
Elevator elevator(&floor1, &floor2, &floor3);






void setup() {
  Serial.begin(9600);  
}

void loop() {
  floor1.update();
  floor2.update();
  floor3.update();
  elevator.update();
  delay(10);
}

 
