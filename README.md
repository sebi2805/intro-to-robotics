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
**Homework #1 - Setting Up the Environment**

#### Deadline (hard): 20th of October, 23:59.

### Objective

The primary goal of this homework is to set up the development environment for the course. This involves installing the Arduino IDE, as we will be extensively working with it throughout the semester. Additionally, this homework requires the creation of a private GitHub repository that adheres to specific guidelines.

**Homework #2: Controlling RGB LEDs with Potentiometers**

#### Completed By: [Your Name]

#### Deadline: 23rd October 2023

#### Showcase: [YouTube Video](https://www.youtube.com/shorts/dX5UbsVYVTg)

### Objectives Achieved

Successfully controlled each channel (Red, Green, and Blue) of an RGB LED using individual potentiometers. This allowed me to gain practical experience in interfacing potentiometers with an Arduino board and dynamically controlling RGB LEDs based on real-time analog readings.

### Components Used

- 1 RGB LED
- 3-4 Potentiometers (An extra one for implementing a minimal signal value)
- Resistors (330Ω) and necessary wiring

### Technical Implementation

I employed separate potentiometers to individually control each color channel of the RGB LED: Red, Green, and Blue. I used the Arduino's analog inputs to read the potentiometers' values and the `map()` function to convert these to appropriate PWM values for the LED pins.

### Code Quality

- I've used well-named constants instead of "magic numbers."
- I've implemented filters, such as moving averages and a minimal signal display value, to smooth out the LED's color transitions and reduce flicker.

### Best Practices Followed

1. Used the `map()` function for precise value mapping.
2. Avoided inline calculations within the `analogWrite` function.
3. Employed descriptive variable names to improve code readability.
4. Replaced "magic numbers" with well-named constants.
5. Ensured the correct number of components were used in the circuit.

### Additional Hardware Filtering

To combat signal noise and further smooth out the LED's flickering, I added capacitors, this was meant only for my testing part.
