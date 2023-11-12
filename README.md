# Robotics Fundamentals: Academic Year 2023-2024

## Overview

This repo serves as the central hub for all laboratory tasks and assignments associated with my 3rd-year "Introduction to Robotics" course at the University of Bucharest. Weekly updates will include homework details, coding implementations, and supporting visual materials.

## Folder Structure

The repository is organized as follows to ensure ease of navigation and maintainability:

- **intro-to-robotics/**
  - **README.md**: Main README file containing repository overview and other details.
  - **Homeworks/**: Folder containing all homework assignments.
    - **Homework[x]\_[TOPIC]/**: X'st homework assignment focused on TOPIC.
      - **README.md**: Specifics about Homework #x
      - **Instructions.md**: Installation guide, if applicable.
      - **Screenshots/**: Videos or pictures related to Homework #x.
      - **src/**: Code files for Homework #x
      - **Assets/**: Any additional resources for Homework
      - **TBD**: Others
  - **Projects/**: Folder for projects.
    - **Project1\_[Topic]/**: First project assignment (example).
      - **README.md**: Specifics about Project #1.
      - **Code/**: Code files for Project #1.
      - **Assets/**: Any additional resources for Project #1.
      - **TBD**: Others
  - **Resources/**: Folder containing additional resources and materials.
    - **Useful_Links.md**: Compilation of useful external links.
    - **TBD**: Others

Each folder and file in the structure serves a specific purpose and is described next to it for quick reference.

## Homework #1 - Setting Up the Environment

### Deadline (hard): 20th of October, 23:59.

### Objective

The primary goal of this homework is to set up the development environment for the course. This involves installing the Arduino IDE, as we will be extensively working with it throughout the semester. Additionally, this homework requires the creation of a private GitHub repository that adheres to specific guidelines.

## Completed Homework #2: Controlling RGB LEDs with Potentiometers

### Deadline Met: 23rd October 2023

### [Showcase Video on YouTube](https://www.youtube.com/shorts/dX5UbsVYVTg)

#### Objectives Achieved

- Controlled each channel (Red, Green, Blue) of RGB LED using individual potentiometers.
- Gained hands-on experience interfacing potentiometers with Arduino.
- Dynamic control of RGB LEDs based on real-time analog readings.

#### Components Utilized

- **RGB LED**: 1
- **Potentiometers**: 4 (extra one for minimal signal value implementation)
- **Resistors and Wires**: 330Ω resistors and necessary wiring.
  ![Hardware Setup](/Resources/Images/RGB-setup.jpeg)

#### Technical Highlights

- Individual control of RGB LED colors using separate potentiometers.
- Utilization of Arduino's analog-to-digital conversion to read potentiometer values.
- Mapped potentiometer readings to appropriate PWM values for LED control.

#### Coding Standards

- Clean, well-structured code.
- Use of meaningful constants instead of "magic numbers."
- Implementation of filters like moving averages and minimal displayable values.

##### Best Practices and Common Mistakes Avoided

1. Utilized `map()` function for accurate mapping.
2. Calculations separated from `analogWrite()` function.
3. Used meaningful variable names.
4. Replaced "magic numbers" with well-named constants.
5. Ensured the correct number of components for the task.

#### Additional Measures: Hardware Filtering

- Capacitors added between the VCC and GND of potentiometers for hardware-level noise reduction.

## Completed Homework #3: Elevator Simulator

### Deadline Met: 31rd October 2023

### [Showcase Video on YouTube](https://www.youtube.com/shorts/1ydJD84eGQU)

#### Objectives Achieved

- Developed a simulation of a 3-floor elevator using Arduino.
- Implemented a custom queue data structure for floor management.
- Used object-oriented principles to manage elevator and floor states.

#### Components Utilized

- **LEDs**: At least 4 (3 for floors and 1 for the elevator's operational state)
- **Buttons**: At least 3 (for floor calls)
- **Buzzer**: 1
- **Resistors and Wires**: As needed
  ![Hardware Setup](/Resources/Images/elevator-setup.jpg)

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

## Completed Homework #4: 7-Segment Display Drawing

### Deadline Met: November 7th, 2023

### [Showcase Video on YouTube](https://youtube.com/shorts/8kp4Yvexm8I?feature=share)

#### Objectives Achieved

- Developed an interactive drawing application on a 7-segment display using Arduino.
- Utilized a joystick for navigation and drawing on the display segments.
- Enhanced user experience by adding a buzzer and an LED for auditory and visual feedback.

#### Components Utilized

- **7-Segment Display**: 1
- **Joystick**: 1
- **LED**: 1 (For visual feedback on button press)
- **Buzzer**: 1 (For auditory feedback)
- **Resistors and Wires**: As needed
  ![Hardware Setup](/Resources/Images/7-segment-setup.jpg)

#### Technical Highlights

- **Segment Movement**: Implemented a mapping table for intuitive movement across segments using a joystick.
- **Interrupts**: Used interrupts for detecting button presses without blocking the main loop.
- **Feedback Mechanisms**: Incorporated a buzzer and an LED to provide feedback on button presses.

#### Coding Standards

- Followed clean coding principles and used enums for better code readability.
- Replaced "magic numbers" with well-named constants.
- Implemented debounce logic to avoid unintentional button presses.

##### Best Practices and Common Mistakes Avoided

1. Utilized `millis()` instead of `delay()` for non-blocking behavior.
2. Used descriptive variable names and enums for clarity.
3. Managed button debounce and state changes effectively.

#### Additional Measures: Technical Depth

- **Segment Movement Table**: Implemented a table to manage movement across segments.
- **Interrupts**: Efficiently used interrupts for button presses.
- **Buzzer and LED Feedback**: Added creative elements for user interaction.

#### Segment Movement Table

| Segment Name | Up  | Down | Left | Right |
| ------------ | --- | ---- | ---- | ----- |
| a            | N/A | g    | f    | b     |
| b            | a   | g    | f    | N/A   |
| c            | g   | d    | e    | dp    |
| d            | g   | N/A  | e    | c     |
| e            | g   | d    | N/A  | c     |
| f            | a   | g    | N/A  | b     |
| g            | a   | d    | N/A  | N/A   |
| dp           | N/A | N/A  | c    | N/A   |

## Completed Homework: Stopwatch Timer with Dual 7-Segment Displays

### Deadline Met: November 13th - 19th, 2023

### [Showcase Video on YouTube](https://youtube.com/shorts/hoOf8aKgjPQ?feature=share)

#### Objectives Achieved

- Implemented a stopwatch timer using two 4-digit 7-segment displays, with one dedicated to showing the main timer and the other for lap times.
- Developed lap saving functionality, allowing for tracking and displaying up to 4 lap times.
- Enhanced user experience with clear separation of timer and lap time displays, and added auditory feedback for user interactions.

#### Components Utilized

- **7-Segment Displays**: 2 (One with 4 digits for main timer, one for lap times)
- **Buttons**: 3 (Start/Pause, Reset, Save Lap)
- **Buzzer**: 1 (For auditory feedback on button interactions)
- **Shift Register**: 1 (For efficient display control)
- **Resistors and Wires**: As required for the circuit
  ![Hardware Setup](/Resources/Images/stopwatch-setup.jpg)

#### Button Functionalities

- **Button 1 (Start/Pause):** Toggles the stopwatch between running and paused states.
- **Button 2 (Reset):** Resets the timer to "000.0" in pause mode, and resets saved laps in lap viewing mode.
- **Button 3 (Save Lap/Cycle Laps):** In counting mode, saves the current lap time. When the stopwatch is paused, cycles through the last saved laps.

#### Technical Highlights

- **Dual Display Control**: Utilized one 7-segment display for the stopwatch timer and a separate display for showing lap times.
- **Lap Functionality**: Enabled saving and cycling through up to 4 lap times for detailed time tracking.
- **Interrupts**: Used interrupts for Start/Pause and Save Lap buttons to ensure precise timing.
- **Auditory Feedback**: Integrated a buzzer to provide distinct tones for each button press, enhancing user feedback.

#### Coding Standards

- Followed clean coding principles for readability and maintainability.
- Replaced "magic numbers" with well-named constants for better clarity.
- Implemented debounce logic for accurate and reliable button press detection.

##### Best Practices and Common Mistakes Avoided

1. Used `millis()` for non-blocking code execution, allowing for simultaneous display updates and button monitoring.
2. Employed descriptive variable names and structured code for ease of understanding and modification.
3. Effectively managed button debounce to prevent accidental triggers, enhancing user experience.

#### Additional Technical Details

- **Dot Separator**: Added a dot on the display to distinguish seconds from tenths of a second.
- **Non-Volatile Memory**: Utilized to retain lap times until reset, ensuring data persistence across stopwatch cycles.
- **Buzzer Tones**: Allocated unique frequencies and durations for each button's tone to differentiate their functions audibly.

#### Conclusion

This stopwatch project successfully integrates dual 7-segment displays for an intuitive and user-friendly time tracking experience. The clear delineation between the main timer and lap times, combined with the distinct button functionalities, makes this stopwatch a practical and efficient tool. The additional auditory feedback further enriches the user interaction with the device.
