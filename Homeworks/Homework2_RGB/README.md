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
  ![My Hardware Setup](Resources/Images/RGB-setup.jpeg)

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
