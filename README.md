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

## Completed Homework: Smart Environment Monitor and Logger

### Deadline Met: November 21th, 2023

### [Showcase Video on YouTube](https://youtu.be/0QjNsEFu4g4)

#### Objectives Achieved

The Smart Environment Monitor and Logger is an Arduino-based system designed to gather and log environmental data. It utilizes various sensors to monitor environmental conditions and logs this data into EEPROM for future reference. The system also provides visual feedback via an RGB LED and offers user interaction through a Serial Menu. This project focuses on integrating sensor readings, efficient memory management, and building a user-friendly menu interface.

## Key Features

- **Environmental Data Monitoring**: Uses an Ultrasonic Sensor and an LDR to monitor environmental conditions.
- **Data Logging**: Logs environmental data into EEPROM for persistence.
- **Visual Feedback**: An RGB LED and a buzzer indicate system status and alerts.
- **User Interaction**: A Serial Menu allows users to interact with and control the system.
- **Bonus Buzzer**: Added as an additional alert mechanism.
- **Data Plotting Option**: Includes functionality to plot the sensor data samples for better visualization.

## Components

- Arduino Uno Board
- Ultrasonic Sensor (HC-SR04)
- Light-Dependent Resistor (LDR)
- RGB LED
- Resistors as needed
- Breadboard and connecting wires
- Buzzer (Bonus Component)
- Optional: Additional sensors/components for extended functionality
  ![Hardware Setup](/Resources/Images/menu-sensor-setup.jpg)

## Menu Structure

1. **Sensor Settings**: Adjust sampling intervals and set alert thresholds for sensors.
2. **Reset Logger Data**: Option to clear the logged data with confirmation steps.
3. **System Status**: View current sensor readings, settings, and display logged data.
4. **RGB LED Control**: Manually control RGB LED colors and toggle automatic mode based on sensor data.
5. **Data Plotting**: (Bonus Feature) Plot and visualize sensor data samples.

## Usage Guide

### Navigating the Menu

- Power on the system. The main menu options are displayed on the Serial Monitor.
- Use the Serial Monitor to input your choice. For example, type '1' to enter the Sensor Settings menu.
- Follow the on-screen instructions to navigate through the submenu options.

### Interpreting LED and Buzzer Alerts

- The RGB LED and buzzer provide real-time feedback based on sensor readings.
- **Green LED**: Indicates all sensor values are within safe thresholds.
- **Yellow LED**: Warns that sensor values are approaching their thresholds.
- **Red LED**: Alerts when sensor values exceed the set thresholds; accompanied by a buzzer tone.

### Sampling and Logging Data

- The system periodically samples sensor data at a user-defined interval.
- Data from the LDR and Ultrasonic sensors are logged into EEPROM.

## Technical Challenges

### EEPROM Management

- **Write Cycles**: Limited EEPROM write cycles were a consideration. To mitigate wear, `EEPROM.update()` was used instead of `EEPROM.write()`, ensuring data is written only if it has changed.
- **Data Overwriting**: After reaching a certain number of samples, data is overwritten in a circular manner to efficiently use EEPROM space.
- **Variable Control**: Rather than using blocking `for` or `while` loops, the `loop()` function and control variables manage the flow of data logging, adhering to Arduino's event-driven architecture.

### Sensor Data Handling

- **Sensor Calibration**: Ensuring accurate sensor readings was essential. Calibration involved testing the sensors in various environmental conditions to determine their operating range.
- **Data Mapping**: The LDR sensor data was mapped from its analog range (0-1023) to a byte range (0-255) before storing it in EEPROM, optimizing the memory usage.

### Serial Communication

- **User Input Validation**: Handling Serial input required validation to ensure users enter appropriate commands.
- **Error Handling**: In case of invalid inputs, the system provides feedback and allows the user to re-enter their choice.

### Additional Functionalities

- **Data Plotting**: Implemented a feature to plot sensor data, enhancing the system’s usability for analysis and monitoring.
- **Buzzer Integration**: Added a buzzer as a secondary alert system, which activates when sensor thresholds are exceeded. The challenge was to integrate it seamlessly with the existing LED feedback mechanism.

## Completed Homework: Smart Environment Monitor and Logger

### Deadline Met: November 28th, 2023

### [Showcase Video on YouTube](https://youtu.be/mWQClNwjNQA)

#### Objectives Achieved

Develop a small game on an 8x8 LED matrix. The game incorporates elements like a player, bombs/bullets, and walls. The player navigates through the matrix, avoiding or destroying walls in a style inspired by classic games like Bomberman and terminator-tanks.

## Key Components

- Arduino Uno Board
- Joystick for movement control
- 8x8 LED Matrix for game display
- MAX7219 LED driver
- Resistors and capacitors as required
- Breadboard and connecting wires
- Buzzer
  <br/>
  <img src="/Resources/Images/matrix-shooter-setup.jpg" alt="Game Setup Image" width="400"/>

## Gameplay Mechanics

1. **Player Movement**: Controlled via joystick, the player character blinks slowly.
2. **Bullets**: Blink rapidly and are used to destroy walls.
3. **Walls**: Occupying 50% - 75% of the matrix, these do not blink and must be navigated around or destroyed.

## User Experience Considerations

### LED Differentiation

- Ensure distinct blinking patterns:
  - **Player**: Blinks slowly for clear identification.
  - **Bullets**: Blink rapidly to signify urgency or danger.
  - **Walls**: Do not blink.

### Control Smoothness

- Joystick control should be intuitive and responsive.
- Game control mechanics should provide a pleasant and engaging experience.

### Game Initialization

- Walls should not be generated on the player's starting position.
- Implement a safe starting zone for the player.

## Additional Features

- **Start/End Animations**: Enhance user engagement with animations at the beginning and end of the game.
- **Buzzer Feedback**: Provide audio feedback for game events like shooting or hitting a wall.

## Technical Challenges

### Matrix Control

- Managing the LED matrix to display game elements dynamically and accurately.

### Joystick Integration

- Smooth integration of the joystick with the game logic for fluid player movement.

### Game Logic

- Implementing robust game logic that handles collisions, shooting mechanics, and game progression.

## Conclusion

This project aims to provide a fun and interactive experience while also serving as an excellent learning opportunity for working with LED matrices, Arduino programming, and game development basics.
