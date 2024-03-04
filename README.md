# Arduino Tetris Arcade Game

## Overview
This repository contains the source code for a custom Tetris arcade game, created as my sophomore honors project at Scottsdale Community College. This game is designed to be run on an Arduino platform, and uses some physical controls and an LED display to create a custom arcade game experience based on the classic game of Tetris.

# Features
**Custom Hardware Controls:** Immensely improve the play experience with your game with built-in joystick and button controls.
**LED Matrix Display:** Gameplay display with an LED strip housed in the game grid; that is, the LED strip will display the game grid and Tetris pieces.
**Scoring System:** Scoring system using EEPROM storage that saves the 4-digit top score which is saved using ROM storage and a 4-digit current score for the current game being played. 
**Adjustable Difficulty:** EvaluateS a player's performance with dynamic difficulty adjustment, such as increasing the drop speed as the game progresses.

## Functionality and How It Works

The Tetris arcade game developed for this project offers a comprehensive implementation of the classic Tetris game, adapted for a custom hardware setup. Here’s a detailed overview of its functionality and operation:

- **Game Mechanics**: The core mechanics follow traditional Tetris gameplay, where players must align falling tetrominoes to complete and clear rows. The game accelerates as the player advances, increasing the challenge.
- **Control Scheme**: Players use the custom-built controls, including a joystick for moving tetrominoes horizontally and rotating them, and buttons for instant drop and pause/resume functionality. This setup provides an intuitive and engaging user experience.
- **Display System**: The game uses an LED matrix display to render the game visuals vividly. Each tetromino is displayed in different colors, making the game visually appealing and easy to follow.
- **Scoring and Levels**: The game includes a scoring system that rewards players for completing single or multiple lines simultaneously. The difficulty level increases as the player scores more points, adjusting the speed of falling tetrominoes accordingly.
- **Sound Effects**: Although not detailed in the initial README draft, if your project includes sound effects for line clears, game over, and other game events, it significantly enhances the arcade experience.
- **Memory Management**: The project utilizes EEPROM storage to save high scores, allowing players to track their progress over time and set new high score targets.

## Hardware Requirements
- Arduino Uno or compatible board
- Adafruit NeoPixel LED strip
- Joystick and button set
- EEPROM for score storage

## Software Requirements
- Arduino IDE for uploading the code to the Arduino board
- Adafruit NeoPixel Library
- EEPROM Library for Arduino

## Installation
1. Ensure all required hardware components are correctly wired to your Arduino board.
2. Install the Arduino IDE on your computer.
3. Download and install the Adafruit NeoPixel and EEPROM libraries using the Library Manager in the Arduino IDE.
4. Clone this repository to your local machine.
5. Open `Tetris.cpp` with the Arduino IDE, adjust pin configurations as necessary, and upload it to your Arduino board.

## Contributing
Contributions to this project are welcome! Please fork the repository and submit a pull request with your improvements.

## License
This project is open-source and available under the MIT License. See the LICENSE file for more information.

## Acknowledgments
Special thanks to Scottsdale Community College for providing the environment and support needed to undertake this project. Additional thanks to the Arduino and Adafruit communities for their invaluable resources and libraries.
