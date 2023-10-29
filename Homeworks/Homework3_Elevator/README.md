## Completed Homework #4: Elevator Simulator

### Deadline Met: (Date of Completion)

### [Showcase Video on YouTube](https://www.youtube.com/shorts/1ydJD84eGQU)

#### Objectives Achieved

- Developed a simulation of a 3-floor elevator using Arduino.
- Implemented a custom queue data structure for floor management.
- Used object-oriented principles to manage elevator and floor states.

#### Components Utilized

- **LEDs**: At least 4 (3 for floors and 1 for the elevator's operational state)
- **Buttons**: At least 3 (for floor calls)
- **Buzzer**: 1 (optional for Computer Science, mandatory for CTI)
- **Resistors and Wires**: As needed
  ![Hardware Setup](/Resources/Images/elevator-setup.jpeg)

#### Technical Highlights

- Created a custom `FloorQueue` class to manage elevator floor requests in an organized manner.
- Designed classes like `FloorControlPanel`, `ControlPanel`, and `FlickerLed` for modular code.
- Managed different elevator states such as moving, stationary, and closing doors.

#### Coding Standards

- Followed object-oriented principles for modular and maintainable code.
- Replaced "magic numbers" with well-named constants.
- Utilized polymorphism through an `Updatable` interface.

##### Best Practices and Common Mistakes Avoided

1. Utilized `millis()` instead of `delay()` for non-blocking behavior.
2. Used descriptive variable names, e.g., `front`, `rear` in custom queue.
3. Managed button debounce to avoid unintentional button presses.
4. Effectively used method overriding for specialized behavior in derived classes.
5. Included sound feedback for specific elevator events through the `ControlPanel`.

#### Additional Measures: Technical Depth

- **Custom Queue**: Implemented a custom queue for effective floor management.
- **Polymorphism**: Used polymorphism for easy management of updatable components.
- **Sound Feedback**: Incorporated buzzer tones to simulate real-world elevator sounds.
