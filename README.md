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
