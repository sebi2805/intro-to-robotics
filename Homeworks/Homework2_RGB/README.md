**Homework #2: Controlling RGB LEDs with Potentiometers**

#### Completed By: [Your Name]

#### Deadline: 23rd October 2023

#### Showcase: [YouTube Video](https://www.youtube.com/shorts/dX5UbsVYVTg)

### Objectives Achieved

Successfully controlled each channel (Red, Green, and Blue) of an RGB LED using individual potentiometers. This allowed me to gain practical experience in interfacing potentiometers with an Arduino board and dynamically controlling RGB LEDs based on real-time analog readings.

### Components Used

- 1 RGB LED
- 4 Potentiometers (An extra one for implementing a minimal signal value)
- Resistors (330Ω) and necessary wiring

### Technical Implementation

I employed separate potentiometers to individually control each color channel of the RGB LED: Red, Green, and Blue. I used the Arduino's analog inputs to read the potentiometers' values and the `map()` function to convert these to appropriate PWM values for the LED pins.

### Code Quality

- My code is clean and well-structured, adhering to best coding practices.
- I've used well-named constants instead of "magic numbers."
- I've implemented filters, such as moving averages and a minimal signal display value, to smooth out the LED's color transitions and reduce flicker.

### Best Practices Followed

1. Used the `map()` function for precise value mapping.
2. Avoided inline calculations within the `analogWrite` function.
3. Employed descriptive variable names to improve code readability.
4. Replaced "magic numbers" with well-named constants.
5. Ensured the correct number of components were used in the circuit.

### Additional Hardware Filtering

To combat signal noise and further smooth out the LED's flickering, I added capacitors between the VCC and GND terminals of the potentiometers. This hardware-level filter significantly improved performance.
