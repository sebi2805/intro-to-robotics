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
  <img src="/Resources/Images/menu-sensor-setup.jpg" alt="Hardware Setup" width="300"/>

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
