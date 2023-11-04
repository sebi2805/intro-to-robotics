## Completed Homework #4: 7-Segment Display Drawing

### Deadline Met: November 7th, 2023

### [Showcase Video on YouTube](https://www.youtube.com/x)

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
