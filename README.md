# Arduino Tetris Arcade Game

## Overview
This repository contains the source code for a custom Tetris arcade game, created as my sophomore honors project at Scottsdale Community College. This game is designed to be run on an Arduino platform and uses some physical controls and an LED display to create a custom arcade game experience based on the classic game of Tetris.

## Functionality

The Tetris arcade game developed for this project offers a comprehensive implementation of the classic Tetris game, adapted for a custom hardware setup.

- **Game Mechanics**: The core mechanics follow traditional Tetris gameplay, where players must align falling tetrominoes to complete and clear rows. The game accelerates as the player advances, increasing the challenge.
- **Control Scheme**: Players use the custom-built controls, including a joystick for moving tetrominoes horizontally and rotating them, and buttons for instant drop and pause/resume functionality.
- **Display System**: The game uses an LED matrix display to render the game visuals vividly. Each tetromino is displayed in different colors, making the game visually appealing and easy to follow.
- **Scoring and Levels**: The game includes a scoring system that rewards players for completing single or multiple lines simultaneously. The difficulty level increases as the player scores more points, adjusting the speed of falling tetrominoes accordingly.
- **Memory Management**: The project utilizes EEPROM storage to save high scores, allowing players to track their progress over time and set new high-score targets.

## Hardware Requirements
- 5M WS2812 LED’s 
- Arduino MEGA
- Breadboard
- Jumper Wires
- Joystick
- 6x Single-Pole Momentary Contact Push-Button Switch
- MAX7219 8-Digit LED Display 7-Segment Digital Tube

## Arduino Pin Diagram
![Arduino Pin Diagram Medium](https://github.com/naluthi/Arduino-Tetris-Arcade-Game/assets/116135231/3d3accac-642c-4168-957b-4e825acb2651)

## Software Requirements
- Arduino IDE for uploading the code to the Arduino board
- Adafruit NeoPixel Library
- EEPROM Library for Arduino
