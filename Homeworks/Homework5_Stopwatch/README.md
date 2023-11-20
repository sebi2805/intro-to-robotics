## Completed Homework: Stopwatch Timer with Dual 7-Segment Displays

### Deadline Met: November 14th, 2023

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
  <br/>
  <img src="/Resources/Images/menu-sensor-setup.jpg" alt="Hardware Setup" width="400"/>

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
