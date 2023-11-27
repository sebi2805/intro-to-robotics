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
