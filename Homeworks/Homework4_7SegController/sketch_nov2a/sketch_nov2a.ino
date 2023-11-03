int joystickPins[] = {A0, A1, 2};

int displayPins[] = {4, 5, 6, 7, 8, 9, 10, 11};

typedef void (*callback_t)();

// Enum to represent each segment and a special value for N/A
enum SegmentName
{
    a = 0,
    b,
    c,
    d,
    e,
    f,
    g,
    dp,
    NA = -1
};

// Define the structure for a segment
struct Segment
{
    SegmentName name;
    SegmentName up;
    SegmentName down;
    SegmentName left;
    SegmentName right;
};

enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE
};

// Define the array of segments
Segment segmentsTable[] = {
    {a, NA, g, f, b},
    {b, a, g, f, NA},
    {c, g, d, e, dp},
    {d, g, NA, e, c},
    {e, g, d, NA, c},
    {f, a, g, NA, b},
    {g, a, d, NA, NA},
    {dp, NA, NA, c, NA}};

class FlickerLed
{
protected:
    int ledPin;
    unsigned long lastFlickerTime;
    bool flickerValue;
    bool isChecked;
    bool isSelected;

public:
    FlickerLed(int ledPin)
    {
        this->ledPin = ledPin;
        this->lastFlickerTime = 0;
        this->flickerValue = true;
        this->isChecked = false;
        this->isSelected = false;
        pinMode(ledPin, OUTPUT);
    }

    void toggleCheck()
    {
        isChecked = !isChecked;
    }

    void setIsSelected(bool _val)
    {
        isSelected = _val;
    }
    void update()
    {
        if (isChecked)
        {
            digitalWrite(ledPin, HIGH);
        }
        else
        {
            if (!isSelected)
            {
                digitalWrite(ledPin, LOW);
            }
            else if (millis() - lastFlickerTime > 200)
            {
                flickerValue = !flickerValue;
                lastFlickerTime = millis();
                digitalWrite(ledPin, flickerValue);
            }
        }
    }
};

class SevenSegmentDisplay
{
protected:
    FlickerLed *segments[8];
    SegmentName currentSegment;

public:
    SevenSegmentDisplay(int segmentPins[8])
    {

        for (int i = 0; i < 8; i++)
        {
            segments[i] = new FlickerLed(segmentPins[i]);
        }
        currentSegment = dp;
    }

    void moveSegment(Direction dir)
    {
        Segment current = segmentsTable[currentSegment];
        SegmentName nextSegment = NA;

        if (dir == Direction::UP)
        {
            Serial.println(current.up);
            nextSegment = current.up;
        }
        else if (dir == Direction::DOWN)
        {
            Serial.println(current.down);
            nextSegment = current.down;
        }
        else if (dir == Direction::LEFT)
        {
            Serial.println(current.left);
            nextSegment = current.left;
        }
        else if (dir == Direction::RIGHT)
        {
            Serial.println(current.right);
            nextSegment = current.right;
        }

        if (nextSegment != NA)
        {
            currentSegment = nextSegment;
        }
    }

    void toggleSegment()
    {
        segments[currentSegment]->toggleCheck();
    }

    void resetDisplay()
    {
        for (int i = 0; i < 8; i++)
        {
            segments[i]->setIsSelected(false);
        }
        currentSegment = dp;
    }
    void updateDisplay()
    {
        // Turn off all segments
        for (int i = 0; i < 8; i++)
        {
            // Serial.println("Current" + String(currentSegment));
            // Serial.println("Updating" + String(i == currentSegment));
            segments[i]->setIsSelected(i == currentSegment);
            segments[i]->update();
        }

        // Keep it off for half a second
    }
};

class Joystick
{
protected:
    int PIN_JOY_X;
    int PIN_JOY_Y;
    int PIN_JOY_BUTTON;

    const int JOY_THRESHOLD = 100;
    const int JOY_CENTER = 512;
    unsigned long lastButtonPressTime = 0;

public:
    Joystick(int pinX, int pinY, int pinButton) : PIN_JOY_X(pinX), PIN_JOY_Y(pinY), PIN_JOY_BUTTON(pinButton)
    {
        pinMode(PIN_JOY_X, INPUT);
        pinMode(PIN_JOY_Y, INPUT);
        pinMode(PIN_JOY_BUTTON, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(PIN_JOY_BUTTON), handleButtonPress, FALLING);
    }

    Direction getDirection()
    {
        int x = analogRead(PIN_JOY_X);
        int y = analogRead(PIN_JOY_Y);

        if (x < JOY_CENTER - JOY_THRESHOLD)
            return Direction::LEFT;
        else if (x > JOY_CENTER + JOY_THRESHOLD)
            return Direction::RIGHT;
        else if (y < JOY_CENTER - JOY_THRESHOLD)
            return Direction::UP;
        else if (y > JOY_CENTER + JOY_THRESHOLD)
            return Direction::DOWN;
        else
            return Direction::NONE;
    }

    // Function to handle button press using interrupts
    static void handleButtonPress(callback_t callback)
    {
        static unsigned long lastInterruptTime = 0;
        unsigned long interruptTime = millis();

        // If interrupts come faster than 200ms, assume it's a bounce and ignore
        if (interruptTime - lastInterruptTime > 200)
        {
            callback(); // This will call the passed-in callback function
        }
        lastInterruptTime = interruptTime;
    }

    // Other methods for the Joystick class...
};

class Gameboard : public SevenSegmentDisplay, public Joystick
{
public:
    Gameboard(int segmentPins[8], int pinX, int pinY, int pinButton)
        : SevenSegmentDisplay(segmentPins), Joystick(pinX, pinY, pinButton)
    {
    }
    void update()
    {
        Direction direction = controller.getDirection();

        if (direction != Direction::NONE)
        {

            switch (direction)
            {
            case Direction::UP:
                Serial.println("UP");
                gameboard.moveSegment(Direction::UP);
                break;
            case Direction::DOWN:
                Serial.println("DOWN");
                gameboard.moveSegment(Direction::DOWN);
                break;
            case Direction::LEFT:
                Serial.println("LEFT");
                gameboard.moveSegment(Direction::LEFT);
                break;
            case Direction::RIGHT:
                Serial.println("RIGHT");
                gameboard.moveSegment(Direction::RIGHT);
                break;

            default:
                break;
            }
            gameboard.updateDisplay();
            // You can add logic here to change the current segment based on the direction
        }
    }
};

Gameboard gameboard(displayPins, joystickPins[0], joystickPins[1], joystickPins[2]);

void setup()
{
    Serial.begin(9600); // Initialize the serial communication for debugging
    Serial.println("Elevator Controller Initialized");
}
void loop()
{
    // Check the joystick movement
    gameboard.update();
    // Here you can add other logic like handling button presses or updating the display

    delay(10); // Delay for a short period to make the output readable
}
