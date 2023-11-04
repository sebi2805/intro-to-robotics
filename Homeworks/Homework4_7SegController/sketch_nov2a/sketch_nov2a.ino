const int PIN_JOY_X = A0;
const int PIN_JOY_Y = A1;
const int PIN_JOY_BTN = 2;

const int JOY_THRESHOLD = 100;
const int JOY_CENTER = 512;

const int displayPins[] = {4, 5, 6, 7, 8, 9, 10, 11};

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

int currentSegment = dp;
int checkedSegments[] = {0, 0, 0, 0, 0, 0, 0, 0};

const int numberOfSegments = 8;

const int longPressDuration = 1000;

const int BUZZER_PIN = 12;
const int SHORT_PRESS_FREQ = 1000;    // Frequency for short press, in Hz
const int LONG_PRESS_FREQ = 500;      // Frequency for long press, in Hz
const int SHORT_PRESS_DURATION = 100; // Duration for short press, in ms
const int LONG_PRESS_DURATION = 1000; // Duration for long press, in ms

const int flickerTime = 200;       // Flicker time in ms
const int debounceButtonTime = 50; // Debounce time in ms

volatile unsigned long pressStartTime = 0;
volatile bool buttonPressed = false;

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

void flickerCurrentLed(int _pin)
{
    static unsigned long lastFlickerTime = 0;
    static bool flickerValue = true;
    unsigned long currentMillis = millis();

    // Flicker every 500 milliseconds
    if (currentMillis - lastFlickerTime > flickerTime)
    {
        digitalWrite(displayPins[_pin], flickerValue);
        lastFlickerTime = currentMillis;
        flickerValue = !flickerValue;
    }
}

void updateDisplay()
{

    for (int i = 0; i < numberOfSegments; i++)
    {
        if (i == currentSegment)
        {
            // Flicker the current segment
            flickerCurrentLed(i);
        }
        else
        {
            // Set the segment to the value from checkedSegments
            digitalWrite(displayPins[i], checkedSegments[i] ? HIGH : LOW);
        }
    }
}

void buttonISR()
{
    static unsigned long lastInterruptTime = 0;
    unsigned long interruptTime = millis();

    // Debounce logic
    if (interruptTime - lastInterruptTime > debounceButtonTime)
    {
        if (digitalRead(PIN_JOY_BTN) == LOW) // Button pressed
        {
            pressStartTime = interruptTime;
            buttonPressed = true;
        }
        else if (buttonPressed) // Button released
        {
            unsigned long pressDuration = interruptTime - pressStartTime;
            buttonPressed = false;

            if (pressDuration < longPressDuration)
            { // Short press
                checkedSegments[currentSegment] = !checkedSegments[currentSegment];
                tone(BUZZER_PIN, SHORT_PRESS_FREQ, SHORT_PRESS_DURATION);
            }
            else
            { // Long press
                for (int i = 0; i < numberOfSegments; i++)
                {
                    checkedSegments[i] = 0;
                }
                currentSegment = dp;
                tone(BUZZER_PIN, LONG_PRESS_FREQ, LONG_PRESS_DURATION);
            }
        }
    }
    lastInterruptTime = interruptTime;
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

void moveSegment(Direction dir)
{
    static bool joyMoved = false;
    Segment current = segmentsTable[currentSegment];
    SegmentName nextSegment = NA;
    if (!joyMoved)
    {
        if (dir == Direction::UP)
        {
            nextSegment = current.up;
        }
        else if (dir == Direction::DOWN)
        {
            nextSegment = current.down;
        }
        else if (dir == Direction::LEFT)
        {
            nextSegment = current.left;
        }
        else if (dir == Direction::RIGHT)
        {
            nextSegment = current.right;
        }

        if (nextSegment != NA)
        {
            currentSegment = nextSegment;
            joyMoved = true;
        }
    }
    else if (dir == Direction::NONE)
    {
        joyMoved = false;
    }
}

void update()
{
    Direction direction = getDirection();

    if (direction != Direction::NONE)
    {

        switch (direction)
        {
        case Direction::UP:
            moveSegment(Direction::UP);
            break;
        case Direction::DOWN:
            moveSegment(Direction::DOWN);
            break;
        case Direction::LEFT:
            moveSegment(Direction::LEFT);
            break;
        case Direction::RIGHT:
            moveSegment(Direction::RIGHT);
            break;

        default:
            break;
        }
        // You can add logic here to change the current segment based on the direction
    }
    else
        moveSegment(Direction::NONE);
    updateDisplay();
}

void setup()
{
    Serial.begin(9600); // Initialize the serial communication for debugging
    pinMode(PIN_JOY_X, INPUT);
    pinMode(PIN_JOY_Y, INPUT);
    pinMode(PIN_JOY_BTN, INPUT_PULLUP);
    pinMode(BUZZER_PIN, OUTPUT);

    // Set the pin modes for the display pins
    for (int i = 0; i < numberOfSegments; i++)
    {
        pinMode(displayPins[i], OUTPUT);
    }
    attachInterrupt(digitalPinToInterrupt(PIN_JOY_BTN), buttonISR, CHANGE);
}
void loop()
{

    update();
    delay(10); // Delay for a short period to make the output readable
}
