#include//-----------------------------------------------------------------------------------
// LIBRARIES
//-----------------------------------------------------------------------------------
#include <Adafruit_NeoPixel.h>    // Library for LED control
#include <LedControl.h>           // Library for 7-segment display
#include <EEPROM.h>               // Library for ROM memory
#include <SoftwareSerial.h>       // Library to print output to Arduino IDE console

//-----------------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------------

// LED STRIP SETUP
#define GRID_W                (10)                  // # Columns
#define GRID_H                (15)                  // # Rows
#define LED_LENGTH            (GRID_W * GRID_H)     // Length of the strip
#define START_BUTTON          (8)                   // Start button pin
#define LED_DATA              (6)                   // LED data pin
#define DISPLAY_DATA          (5)                   // Digital display data pin
#define CHIP_SELECT           (4)                   // Digital display chip select pin
#define DISPLAY_CLK           (3)                   // Digital display clock pin
#define BACKWARDS             (0)                   // direction LED connection is wired in 

// Tetris SHAPES
#define SHAPE_W               (4)                   // shape width
#define SHAPE_H               (4)                   // shape height
#define NUM_SHAPES            (7)                   // # of shapes

// Joystick
#define NEUTRAL_JOYSTICK      (45)                  // Area of Joystick considered no movement
#define JOYSTICK_PIN          (2)                   // joystick has 2 arduino pins A0 and A1

// PULL OF GRAVITY
#define START_DROP_SPEED      (25)                  // top speed gravity can reach
#define INCREASE_DROP_SPEED   (20)                  // how fast gravity increases
#define INITIAL_MOVE_DELAY    (100)                 // time it takes for player move to occur
#define INITIAL_DROP_DELAY    (500)                 // initial speed shapes fall at
#define INITIAL_DRAW_DELAY    (25)                  // time it takes for new shape to appear

//-----------------------------------------------------------------------------------
// VARIABLES
//-----------------------------------------------------------------------------------

// BUTTON CONTROLS
int leftButton = 9;    // Left
int rightButton = 12;   // Right
int upButton = 11;     // Up --> Rotate
int downButton = 10;   // Down
int startButton = 8;  // Start


// JOYSTICK
int joystick_x = A0;  // X-Axis
int joystick_y = A1;  // Y-Axis


// SCORE DISPLAY
int topScore;     // topScore ever achieved on game
int score = 0;    // sets current score of the game to 0
int tenPoints = 10; // beggining points earned for a deleted row


// PAUSE BUTTON
int pauseButton = 7;    // initializes pause button and sets to arduino pin 7
bool pause = false;     // variable to return pause to false at each new game
bool pause_once = false;  // sets pause as active
bool pause_pressed = false; // pause button pressed to resume game


// SET TIME
unsigned long previousMillis = 0; // time reference to how long the program has been running
unsigned long currentMillis = 0;  // current time since the program began running


int oldButton = 0;    // keeps track of old state of button pushed or not pushed
int oldSide = 0;    // keeps track of the x-axis position of a shape
int oldWantTurn = 0;    // keeauus track of old shape rotation


// Falling shape memory
int shapeID;        // Index of shape in the shapes array
int shapeRotation;  // Orientation each shape starts at 
int shape_x;        // Location of shape on x-axis
int shape_y;        // Location of shape on y-axis 


// Shape Variables
char shape_sequence[NUM_SHAPES];    // Shapes declared
char shapesLeft = NUM_SHAPES;       // Shapes left to play

// Controls the speed shapes move from player controls
long lastMove;   
long moveDelay;   


// Controls the automatic drop of the shapes with no player input
long lastDrop;    
long dropDelay; 


long oldGridLayout;    // saved layout of shapes on the grid
long drawDelay;        // time delay between shape hitting bottom of the grid and new shape added to game


long grid[GRID_W * GRID_H]; // this is how Arduino remembers where shapes are on the grid.

//-----------------------------------------------------------------------------------
// TETRIS SHAPES
//-----------------------------------------------------------------------------------

// 4x1 shape
const char shape_I[] = {
  0, 0, 0, 0,
  1, 1, 1, 1,
  0, 0, 0, 0,
  0, 0, 0, 0,

  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0,

  0, 0, 0, 0,
  0, 0, 0, 0,
  1, 1, 1, 1,
  0, 0, 0, 0,

  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 1, 0, 0,
};

// Square Shape
const char shape_O[] = {
  1, 1, 0, 0,
  1, 1, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,

  1, 1, 0, 0,
  1, 1, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,

  1, 1, 0, 0,
  1, 1, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,

  1, 1, 0, 0,
  1, 1, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
};

// L Shape
const char shape_L[] = {
  0, 0, 1, 0,
  1, 1, 1, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,

  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 1, 1, 0,
  0, 0, 0, 0,

  0, 0, 0, 0,
  1, 1, 1, 0,
  1, 0, 0, 0,
  0, 0, 0, 0,

  1, 1, 0, 0,
  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 0, 0, 0,
};

// J Shape
const char shape_J[] = {
  1, 0, 0, 0,
  1, 1, 1, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,

  0, 1, 1, 0,
  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 0, 0, 0,

  0, 0, 0, 0,
  1, 1, 1, 0,
  0, 0, 1, 0,
  0, 0, 0, 0,

  0, 1, 0, 0,
  0, 1, 0, 0,
  1, 1, 0, 0,
  0, 0, 0, 0,
};

// Upside Down T Shape
const char shape_T[] = {
  0, 1, 0, 0,
  1, 1, 1, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,

  0, 1, 0, 0,
  0, 1, 1, 0,
  0, 1, 0, 0,
  0, 0, 0, 0,

  0, 0, 0, 0,
  1, 1, 1, 0,
  0, 1, 0, 0,
  0, 0, 0, 0,

  0, 1, 0, 0,
  1, 1, 0, 0,
  0, 1, 0, 0,
  0, 0, 0, 0,
};

// Zig Zag Shape
const char shape_S[] = {
  0, 1, 1, 0,
  1, 1, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,

  0, 1, 0, 0,
  0, 1, 1, 0,
  0, 0, 1, 0,
  0, 0, 0, 0,

  0, 0, 0, 0,
  0, 1, 1, 0,
  1, 1, 0, 0,
  0, 0, 0, 0,

  1, 0, 0, 0,
  1, 1, 0, 0,
  0, 1, 0, 0,
  0, 0, 0, 0,
};

// Zig Zag Shape
const char shape_Z[] = {
  1, 1, 0, 0,
  0, 1, 1, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,

  0, 0, 1, 0,
  0, 1, 1, 0,
  0, 1, 0, 0,
  0, 0, 0, 0,

  0, 0, 0, 0,
  1, 1, 0, 0,
  0, 1, 1, 0,
  0, 0, 0, 0,

  0, 1, 0, 0,
  1, 1, 0, 0,
  1, 0, 0, 0,
  0, 0, 0, 0,
};


// Array of pointers to the shape types 
const char *shapes[NUM_SHAPES] = {
  shape_I,
  shape_O,
  shape_L,
  shape_J,
  shape_T,
  shape_S,
  shape_Z,
};

//GRB Hex Colors
const long shape_colors[NUM_SHAPES] = {
  0xFF00FF,  // Light Blue - I
  0xFFFF00,  // Yellow - 0
  0x50FF00,  // Purple - L
  0x0D15F7,  // Dark Blue - J
  0x00FFFF,  // Pink - T
  0xFF0000,  // Green - S
  0x00FF00,  // Red - Z
};


//-----------------------------------------------------------------------------------
//     GLOBALS
//-----------------------------------------------------------------------------------

// Assigns LED length, type, and data pin # to LED library
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_LENGTH, 6, NEO_RGB + NEO_KHZ800);

// Assigns Segment Display Data-In, Clock, Chip-Selector, and Display # to Arduino data pins
LedControl lc = LedControl(5, 3, 4, 1);
LedControl ts = LedControl(5, 3, 4, 1);


//-----------------------------------------------------------------------------------
//     EEPROM STORAGE
//-----------------------------------------------------------------------------------

// This function uses Arduino ROM memory to store the top score even without power
void writeTopScore(int address, int number)
{
  // Writes and splits topScore into two numbers since Arduino can only store numbers up to 8 bits (0- 255)
  EEPROM.write(address, (number >> 8) & 0xFF);    // first 8 bits of topScore
  EEPROM.write(address + 1, number & 0xFF);  // second 8 bits of topSscore
}

// Reads and combines the two topScore numbers to be correctly display on 7-segment display
int readTopScore(int address)
{
  return (EEPROM.read(address) << 8) +
         (EEPROM.read(address + 1));
}

//-----------------------------------------------------------------------------------
//     ARDUINO FUNCTIONS
//-----------------------------------------------------------------------------------

// Arduino Setup Function
void setup()
{
  Serial.begin(9600);
  delay(200);

  // Sets the arduino pins the buttons are wired to as inputs
  pinMode(pauseButton, INPUT_PULLUP);
  pinMode(START_BUTTON, INPUT_PULLUP);
  pinMode(leftButton, INPUT_PULLUP);
  pinMode(rightButton, INPUT_PULLUP);
  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);


  // Setup for 7-Segment Display (current score )
  lc.shutdown(0, false);  // Shut down the display register
  lc.setScanLimit(0, 3);  // Set current score bit display limit
  lc.setIntensity(0, 1);  // Set brightness (0-15)
  
  // Setup for topScore portion of display 
  ts.shutdown(0, false);  // Shut down the display register
  ts.setScanLimit(0, 7);  // Set current score bit display limit
  ts.setIntensity(0, 1);  // Set brightness (0-15)

  topScore = readTopScore(1); // read the last topScore saved to EEPROM at register 1

  strip.begin();  // setup the LEDs
  strip.show();  // Initialize all pixels to 'off'

  // set up joystick button
  pinMode(JOYSTICK_PIN, INPUT);
  digitalWrite(JOYSTICK_PIN, HIGH);

  // make sure arduino knows the grid is empty.
  for (int i = 0; i < GRID_W * GRID_H; ++i) {
    grid[i] = 0;
  }

  // make the game a bit more random - pull a number from space and use it to 'seed' a crop of random numbers.
  randomSeed(analogRead(joystick_y) + analogRead(2) + analogRead(3));

  // get ready to start the game.
  newShape();

  // Set the initial delays for moving, dropping, and drawing the Tetris blocks.
  // These values determine the pace of the game and are adjusted for difficulty as game progresses. 
  moveDelay = INITIAL_MOVE_DELAY;
  dropDelay = INITIAL_DROP_DELAY;
  drawDelay = INITIAL_DRAW_DELAY;

  // start the game clock after everything else is ready.
  oldGridLayout = lastDrop = lastMove = millis();

  startGame();  // wait for start button to be pressed before starting game
  
  score = 0;    // Set the initial score to 0. This ensures that each new game starts at 0.
  lc.setDigit(0, 0, 0x000, false);  // This sets the currentScore display as 0.
  displayScore(readTopScore(1), 4); // This reads the stored topScore and displays it
}

// Arduino Loop Function
void loop()
{
  pauseGame();  // calls the pauseGame function and pauses game if button is pressed throughout the game
}


//-----------------------------------------------------------------------------------
//     HELPER FUNCTIONS
//-----------------------------------------------------------------------------------

// Function: setGrid();
// Description: loops through the rows and columns to store current grid and falling shape
void setGrid()
{
  int x, y;                                 // Counter Variables for Loop

  for (y = 0; y < GRID_H; y++) {            // Loop that will iterate through the height of grid
    for (x = 0; x < GRID_W; x++) {          // Loop that will iterate through the gridWith
      if (grid[y * GRID_W + x] != 0) {      //Checks to see if it has reached the bottom of grid
        checkMove(x, y, grid[y * GRID_W + x]);  //Set current point of shape on grid and updates LED color at that point
      } else {
        checkMove(x, y, 0);                     // point of shape is still called but with 0 as color
      }
    }
  }
  strip.show();
}
//-----------------------------------------------------------------------------------------------------

// Function: newShape()
// Description: finds a new shape to add and organizes which shapes are added
void newShape()
{
  int i, j, k;

  if (shapesLeft >= NUM_SHAPES) {     // all shapes in array have been used

    for (i = 0; i < NUM_SHAPES; ++i) {
      do {
        j = rand() % NUM_SHAPES;     // pick a random shape
        for (k = 0; k < i; ++k) {    // make sure it isn't already in the sequence
          if (shape_sequence[k] == j) break;  // already in sequence
        }
      } while (k < i);    // add shape if it is in the sequence
        shape_sequence[i] = j;
      }
     shapesLeft = 0;  // rewind sequence counter
    }
  shapeID = shape_sequence[shapesLeft++];   // get the next piece in the sequence
  
  // always start the piece top center
  shape_y = -4;        // -4 squares off the top of the screen
  shape_x = 4;         // 4 squares from the right side of the grid
  shapeRotation = 0;   // always start in the same orientation
}
//-----------------------------------------------------------------------------------------------------

// Function: addShape()
// Description: This function adds a new shape to the game
void addShape() 
{
  int x, y;
  
  // Determines which new shape is added and its location and rotation
  const char *shape = shapes[shapeID] + (shapeRotation * SHAPE_H * SHAPE_W);

  // Iterate through each piece of the shape
  for (y = 0; y < SHAPE_H; ++y) {     
    for (x = 0; x < SHAPE_W; ++x) {
      // Calculate the shape's absolute position on the main grid.     
      // x and y are the coordinates of the shape's origin (top left cell)
      int nx = shape_x + x;            
      int ny = shape_y + y;      
      // Ignore cells that are outside the games grid boundaries
      if (ny < 0 || ny > GRID_H) continue;
      if (nx < 0 || nx > GRID_W) continue;
      // If that cell is already filled then it is part of the shape
      if (shape[y * SHAPE_W + x] == 1) {
        // Once moved or dropped erase old position and turn off lights
        grid[ny * GRID_W + nx] = shape_colors[shapeID];
      }
    }
  }
}
//-----------------------------------------------------------------------------------------------------

// Function: checkMove()
// Description: needed since LED strips are wired in a Z formation. Makes sure shape moves in the right direction
// Parameters: x and y are axis directions and color is the color of shape to be displayed
void checkMove(int x, int y, long color)
{
  int a = (GRID_H - 1 - y) * GRID_W;  // Set's a so that heigh and width of point is defined

  if ((y % 2) == BACKWARDS) {     // Checks if current row shape is on is odd or even
    a += x;                       // Checks even row move shape to the right
  } else {
    a += GRID_W - 1 - x;          // if odd move shape to the left
  }
  a %= LED_LENGTH;                // makes sure that shape stays within bounds of the game grid
  strip.setPixelColor(a, color);  // Turn LED to shape color that was moved so the piece moves
}
//-----------------------------------------------------------------------------------------------------

// Function: clearLED()
// Description: clears the LEDs by setting each color value to zero
void clearLED()
{
  for (int i = 0; i < LED_LENGTH; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
}


//-----------------------------------------------------------------------------------
//    SHAPE CONTROL FUNCTIONS
//-----------------------------------------------------------------------------------

// Function: shapeCanFit()
// Description: This function checks if the shape can fit in the new location and calls
// Parameters: px, py are the coordinates of the shape, and pr is the rotation of the shape.
int shapeCanFit(int px, int py, int pr)
{
  if (hittingEdge(px, py, pr)) return 0;
  if (hittingPieces(px, py, pr)) return 0;
  return 1;
}
//-----------------------------------------------------------------------------------------------------

// Function: hittingEdge()
// Description: Checks if the current shape is hitting the edge of the game grid
// Parameters: px, py are the coordinates of the shape, and pr is the rotation of the shape
int hittingEdge(int px, int py, int pr)
{
  int x, y;
  
  // loop variables and pointer to the current shape
  const char *shape = shapes[shapeID] + (pr * SHAPE_H * SHAPE_W);
  for (y = 0; y < SHAPE_H; ++y) {     // iterate over each cell in the shape
    for (x = 0; x < SHAPE_W; ++x) { 
      int nx = px + x;        // nx is the x-axis location of the shape plus current increment of x;
      int ny = py + y;        // ny is the y-axis location of the shape plus current increment of y;
      if (ny < 0) continue;   // Ignore cells above the grid
      if (shape[y * SHAPE_W + x] > 0) {
        if (nx < 0) return 1;        // check if shape is off the left side of grid
        if (nx >= GRID_W) return 1;  // check if shape is off off right side of grid
      }
    }
  }
  return 0;  // returns 0 if shape is within the game grid
}
//-----------------------------------------------------------------------------------------------------

// Function: hittingPieces()
// Description: Checks if the current shape is colliding with any shape pieces on the grid
// Parameters: px, py are the coordinates of the shape, and pr is the rotation of the shape.
int hittingPieces(int px, int py, int pr)
{
  // loop variables and pointer to the current shape
  int x, y;
  const char *shape = shapes[shapeID] + (pr * SHAPE_H * SHAPE_W);

  // iterate over each cell in the shape
  for (y = 0; y < SHAPE_H; ++y) {
    for (x = 0; x < SHAPE_W; ++x) {
    int nx = px + x;        // nx is the x-axis location of the shape plus current increment of x
    int ny = py + y;        // ny is the y-axis location of the shape plus current increment of y
    if (ny < 0) continue;   // Ignore cells above the grid
    if (shape[y * SHAPE_W + x] > 0) {
      if (ny >= GRID_H) return 1;                 // Return 1 if shape hits bottom of the board grid
      if (grid[ny * GRID_W + nx] != 0) return 1;  // Return 1 if shape hits another shape piece
      }
    }
  }
  return 0;  // Return 0 if the shape does not hit the bottom or another piece
}
//-----------------------------------------------------------------------------------------------------

// Function: eraseShape()
// Description: Erases the current shape from the grid
void eraseShape()
{
  // loop variables and pointer to the current shape
  int x, y;
  const char *shape = shapes[shapeID] + (shapeRotation * SHAPE_H * SHAPE_W);
  
  // iterate over each cell in the shape
  for (y = 0; y < SHAPE_H; ++y) {
    for (x = 0; x < SHAPE_W; ++x) {
      int nx = shape_x + x;        
      int ny = shape_y + y;        
      // Ignore cells outside of the grid
      if (ny < 0 || ny > GRID_H) continue;
      if (nx < 0 || nx > GRID_W) continue;
      // if the cell in the shape is filled, erase the corresponding grid location
      if (shape[y * SHAPE_W + x] == 1) {
        grid[ny * GRID_W + nx] = 0; 
      }
    }
  }
}
//-----------------------------------------------------------------------------------------------------

// Function: fallFaster()
// Description: increases the speed shapes drop at as game progress and makes deleted rows worth more points
void fallFaster()
{
  // Check if the current drop delay is greater than the defined start drop speed.
  if (dropDelay > START_DROP_SPEED) {
    // If it is, decrease the drop delay by a defined increment (INCREASE_DROP_SPEED)
    dropDelay -= INCREASE_DROP_SPEED; 
  }
  tenPoints = 20;
}
//-----------------------------------------------------------------------------------------------------

// Function: moveSideways
// Description: This function is responsible for moving the game piece sideways based on user input.
void moveSideways()
{
  int dx = map(analogRead(joystick_x), 0, 1023, 512, -512); // Horizontal position of the joystick
  int newSide = 0; 
  
  // Check if the joystick is being pushed left or right beyond the neutral position
  if (dx > NEUTRAL_JOYSTICK ) {  // If the joystick is pushed to the left, set newSide to -1
    newSide = -1;                 
  }
  if (dx < -NEUTRAL_JOYSTICK ) { // If the joystick is pushed to the right, set newSide to 1
    newSide = 1;                  
  }
  
  // Check if the left or right button is pressed, and adjust newSide accordingly
  if (!digitalRead(leftButton)) {
    newSide = -1;
  }
  if (!digitalRead(rightButton)) {
    newSide = 1;
  }
  
  // If newSide is different from oldSide and the shape can fit in the new position, move the shape
  if (newSide != oldSide && shapeCanFit(shape_x + newSide, shape_y, shapeRotation) == 1) {
    shape_x += newSide;
  }
  oldSide = newSide;  // update oldSide to reflect the sideways move
}
//-----------------------------------------------------------------------------------------------------

// Function: rotateShape
// Description: This function is responsible for rotating the game piece based on user input.
void rotateShape()
{
  int dy = map(analogRead(joystick_y), 0, 1023, 512, -512); // Vertical position of the joystick
  int newButton = digitalRead(upButton);  // upButton is set to false or not being pressed
  int newTurn = 0;
  
  // Check if the joystick is being pushed up passed the neutral position
  if (dy > NEUTRAL_JOYSTICK ) newTurn = 1;

  // If the button state has changed and it is released, set newTurn to 1
  if (newButton > 0 && oldButton != newButton) {
    newTurn = 1;
  }
  
  oldButton = newButton;  // Store the button state for the next cycle
  
  // If newTurn is 1 and it is different from oldWantTurn, then rotate the shape
  if (newTurn == 1 && newTurn != oldWantTurn) {
    int newRotation = (shapeRotation + 1) % 4;    // Calculate the new rotation of the shape
    
    if (shapeCanFit(shape_x, shape_y, newRotation)) { // Check if shape can fit at location with the new rotation
       shapeRotation = newRotation;      // If it fits, apply the new rotation
    } else {
      if (shapeCanFit(shape_x - 1, shape_y, newRotation)) { // if it doesn't fit kick the shape left 
        shape_x = shape_x - 1;
        shapeRotation = newRotation;
        
      } else if (shapeCanFit(shape_x + 1, shape_y, newRotation)) { // if it doesn't fit kick the shape right
        shape_x = shape_x + 1;
        shapeRotation = newRotation;
      }
    }
  }
  oldWantTurn = newTurn;  // Store the new Turn value 
}


//-----------------------------------------------------------------------------------
//    PLAYER INPUT FUNCTIONS
//-----------------------------------------------------------------------------------

// Function: dropShape()
// Description: drops shape one cell at a time and adds a new shape when the current shape hits the bottom
void dropShape()
{
  eraseShape();
  if (shapeCanFit(shape_x, shape_y + 1, shapeRotation)) { // Check if the shape can fit in its new location
    shape_y++;          // Move the shape down
    addShape();
  } else {              // If the shape hit the bottom or a shap epiece
    addShape();         // add a new shape
    deleteFullRows();   // delete row if its full
    if (checkGameOver() == 1) { // check if the game is over
      gameOver();   // game is over so run game over sequence
    }

    newShape(); // if the game is not over continue and add a new shape
  }
}
//-----------------------------------------------------------------------------------------------------

// Function: dropFaster()
// Description: this function drops the shape faster when joystick or button is held down
void dropFaster()
{  // Set the y-coordinate of the joystick's current position to the range (-512, 512)
  int y = map(analogRead(joystick_y), 0, 1023, 512, -512);

  if (!digitalRead(downButton)) { // Check if the down button is pressed
    dropShape();
  }

  if (y < -NEUTRAL_JOYSTICK ) { // the joystick is being held down
    dropShape();
  }
}
//-----------------------------------------------------------------------------------------------------

// Function: startGame()
// Description: displays "hit start" on LEDS and waits for start button to be pressed to start game
void startGame() 
{
  hitLED();     // display "hit" on LEDS
  delay(1000);  
  startLEDS();  // display "start" on LEDS
  delay(1000);

  while (digitalRead(START_BUTTON) == HIGH) {   // Wait until the start button is pressed 
    delay(10); 
  }
  while (digitalRead(START_BUTTON) == LOW) {  // Wait for button release (debouncing)
    delay(10);
    countdownLEDS();  // 3...2...1...GO leds
  }
}
//-----------------------------------------------------------------------------------------------------

// Function: pauseGame
// Description: pauses the game when the pause button is pressed and resumes when pressed again.
void pauseGame() {
long t = millis();  // time since the program has started running

  // If the game is not currently paused
  if (!pause) {
    if (!digitalRead(pauseButton) && !pause_pressed) { // pause button is pressed and was not already pressed
      pause = !pause;
      pause_pressed = true;
      pause_once = false;
    }
    
    // pause button is released and was previously pressed
    if (digitalRead(pauseButton) && pause_pressed) {
      pause_pressed = false;
    }

    // move the shape sideways if player inputs to do so before or after pause was pressed
    if (t - lastMove > moveDelay) {
      lastMove = t;
      playerInputs();
    }

    // drop the shape down if its time to do so
    if (t - lastDrop > dropDelay) {
      lastDrop = t;
      dropShape();
    }

    // refresh the LED grid if shapes arent being moved or dropping down
    if (t - oldGridLayout > drawDelay) {
      oldGridLayout = t;
      setGrid();
    }
  }  //end of !pause

  // if game is paused
  else {
    if (!digitalRead(pauseButton) && !pause_pressed) { // pause button is pressed and was not already pressed
      pause = !pause;
      pause_pressed = true;
    }
    if (digitalRead(pauseButton) && pause_pressed) { // pause button is released and was previously pressed
      pause_pressed = false;
    }
    pauseLEDS();  // show player game is paused with pause LED sequence
    delay(5);
  }
}
//-----------------------------------------------------------------------------------------------------

// Function: playerInputs()
// Description: Handles all player inputs and updates the state of the shape accordingly
void playerInputs()
{
  eraseShape();   // Erase the current shape from the grid
  moveSideways(); // Check for lateral movement input and move the shape if possible
  rotateShape();  // Check for rotation input and rotate the shape if possible
  addShape();     // Add the updated shape back to the grid
  dropFaster();   // Check for fast drop input and drop the shape if pressed
}


//-----------------------------------------------------------------------------------
//    GAME STATE FUNCTIONS
//----------------------------------------------------------------------------------

// Function: deleteRow()
// Description: Deletes a row from the grid and moves the rows above it down.
// Parameters: y is the row number to delete 
void deleteRow(int y)
{
  int x;
  score += tenPoints;         // Row has been filled add 10 points to score

  if (score > topScore) {       // Check if the current score beats the top score
    writeTopScore(1, score);    // If yes, write the new top score to EEPROM 
    topScore = score;           // Update top score variable so that it is displayed on 7 segment
  }

  displayScore(readTopScore(1), 4);   // Display new top score to 7-Segment Display

  // Move all the rows above the deleted row down
  for (; y > 0; --y) {
    for (x = 0; x < GRID_W; ++x) {
      grid[y * GRID_W + x] = grid[(y - 1) * GRID_W + x];
    }
  }
  
  // clear the now empty top row
  for (x = 0; x < GRID_W; ++x) {
    grid[x] = 0;
  }
  
  displayScore(score, 0); // display the new current score
  delay(100);
  displayScore(topScore, 4);  // display the new topScore
  delay(100);
}
//-----------------------------------------------------------------------------------------------------

// Function: deleteFullRows()
// Description: Checks the entire grid for full rows and delete them
void deleteFullRows()
{
  int x, y, c;
  char row_removed = 0;
  
  for (y = 0; y < GRID_H; y++) {     // Iterate through all rows
    c = 0;  
    for (x = 0; x < GRID_W; ++x) {
      if (grid[y * GRID_W + x] > 0) c++;  // count the full spaces in this row
    }
    if (c == GRID_W) {   // If the row is full, delete it and speed up the game
      deleteRow(y);
      fallFaster();
    }
  }
}
//-----------------------------------------------------------------------------------------------------

// Function: checkGameOver
// Description: Checks if the current shape has gone off the top of the grid, indicating a game over.
int checkGameOver()
{
  int x, y;
  // Get the shape in its current rotation
  const char *shape = shapes[shapeID] + (shapeRotation * SHAPE_H * SHAPE_W);

  // Iterate through each cell in the shape
  for (y = 0; y < SHAPE_H; ++y) {
    for (x = 0; x < SHAPE_W; ++x) {
      int ny = shape_y + y;
      int nx = shape_x + x;
      
      // If the cell is part of the shape and has moved off the top of the grid, the game is over
      if (shape[y * SHAPE_W + x] > 0) {
        if (ny < 0) return 1;  
      }
    }
  }
  return 0; // return 0 if shape is not over the top and continue the game
}
//-----------------------------------------------------------------------------------------------------

// Function: gameOver
// Description: handles all game over logic like LED sequences and resetting the game
void gameOver()
{
  int x, y;
  long stopTime = millis(); // records the time the game ended
  int led_number = 1;
  topScore = readTopScore(1); // reads the current top score in case it has changed

  gameOverLEDS(); // display game over LEDS
  delay(200);

  restart();  // restart the game
  
  currentMillis = millis(); // reset the current time
  previousMillis = currentMillis; // reset the previous time

  // For the next 8 seconds, display the game over sequence and wait for the start button to be pressed
  while(millis() - stopTime < 8000) 
  {
    currentMillis = millis();
    if(currentMillis - previousMillis >= 1000){
      previousMillis += 1000;
      
      // Display the countdown on the LEDs
      strip.setPixelColor(55-led_number, strip.Color(150,150,150)); 
      strip.show();
      led_number += 1; 
    }
    
    // Continue the countdown and setup the game for a new round
    setup();
    return;
 }
}
  
//-----------------------------------------------------------------------------------
//    DISPLAY AND LED SEQUENCE FUNCTIONS
//-----------------------------------------------------------------------------------

// Function: gameOverLEDS()
// Description: This sequence lights the LED's red one by one once game has ended
void gameOverLEDS()
{
  for (int i = 0; i < LED_LENGTH; i++) {
    strip.setPixelColor(i, strip.Color(0, 150, 0)); // sets strip color
    strip.show();
    delay(10);
  }
}
//-----------------------------------------------------------------------------------------------------

// Function: pauseLEDS()
// Description: This sequence lights the LED's white to form a 'P'
void pauseLEDS()
{
  // This clears all the LEDS so the shapes do not show while paused
  clearLED();

  // Array of LED's to light up when paused
  int pauseArray[] = { 32, 47, 52, 67, 72, 82, 83, 84, 85, 86, 87,
                       92, 97, 102, 107, 112, 113, 114, 115, 116, 117 };

  // Function that lights up the array with letter "P"
  for (int i = 0; i < 21; i++) {
    strip.setPixelColor(pauseArray[i], strip.Color(150, 150, 150));  // sets LED color to white
  }
  strip.show();

  // Holds the LEDS until the pause button is pressed again
  if (!pause_once) {  
    pause_once = true;
    delay(100);
  }
}
//-----------------------------------------------------------------------------------------------------

// Function: displayScore()
// Description: this function correctly adds points to the score and display it
void displayScore(int score, int startDigit)
{
  for (int i = 0; i < 4; i++) {
    int digit = score % 10;  // add 10 points to the right digit place
    lc.setDigit(0, startDigit + i, digit, false); // display the score
    score /= 10;  // Shift all digits to the right
  }
}

//-----------------------------------------------------------------------------------------------------

// Function: restart()
// Description: restarts the game once game has ended
void restart()
{
  strip.begin();  // sets up the LEDs
  strip.show();
  lc.clearDisplay(0); // clears the current score from display
  ts.clearDisplay(0); // clears the topScore from display
}
//-----------------------------------------------------------------------------------------------------

// Function: countdownLEDS
// Description: calls the countdown LED sequences in order to be displayed
void countdownLEDS() {
  show3();    
  delay(1000);
  show2();
  delay(1000);
  show1();
  delay(1000);
  showGO();
  delay(350);
}
//-----------------------------------------------------------------------------------------------------

// Function: show3()
// Description: LED sequence to display '3' in white lights
void show3()
{
  clearLED();

  // Array of LED's to show the number 3
  int array3[] = { 32, 33, 34, 35, 36, 37, 42, 57, 62, 72, 73,
                   74, 75, 76, 77, 82, 97, 102, 112, 113, 114, 115, 116, 117};

  // Function that lights up the array with letter '3'
  for (int i = 0; i < 24; i++) {
    strip.setPixelColor(array3[i], strip.Color(150, 150, 150)); // sets color of LEDS to white
  }
  strip.show();
}
//-----------------------------------------------------------------------------------------------------

// Function: show2()
// Description: LED sequence to display '2' in white lights
void show2()
{
  clearLED();

  // Array of LED's to show the number 2
  int array2[] = {32, 33, 34, 35, 36, 37, 47, 52, 67, 72, 73,
                  74, 75, 76, 77, 82, 97, 102, 112, 113, 114, 115, 116, 117
                 };

  // Function that lights up the array with letter "2"
  for (int i = 0; i < 24; i++) {
    strip.setPixelColor(array2[i], strip.Color(150, 150, 150)); // sets color of LEDS to white
  }
  strip.show();
}
//-----------------------------------------------------------------------------------------------------

// Function: show1()
// Description: LED sequence to display '1' in white lights
void show1()
{
  clearLED();

  // Array of LED's to show the number 1
  int array1[] = {34, 35, 36, 44, 55, 64, 75, 84, 95, 104, 114, 115};

  // Function that lights up the array with letter "1"
  for (int i = 0; i < 12; i++) {
    strip.setPixelColor(array1[i], strip.Color(150, 150, 150)); // sets color of LEDS to white
  }
  strip.show();
}
//-----------------------------------------------------------------------------------------------------

// Function: showGO()
// Description: LED sequence to display 'GO' in green lights
void showGO() {
  
  clearLED();

  // Array of LED's to show "GO"
  int arrayGO[] = {23, 24, 25, 26, 33, 36, 43, 46, 53, 56, 63, 64, 65,
                   66, 83, 84, 85, 86, 93, 96, 103, 104, 106, 113, 123, 124, 125, 126
                  };

  for (int i = 0; i < 28; i++) {
    strip.setPixelColor(arrayGO[i], strip.Color(150, 0, 0));  // sets color of LEDS to green
    strip.show();
  }
  strip.show();
}

void hitLED() {
  
  clearLED();
  
  int arrayHit[] {4, 15, 24, 34, 35, 36, 54, 55, 56, 64, 75, 83, 84, 85, 103, 105, 
                  114, 116, 123, 124, 125, 134, 136, 143, 145 
  };
    for (int i = 0; i < 25; i++) {
    strip.setPixelColor(arrayHit[i], strip.Color(150, 0, 0));  // sets color of LEDS to green
    strip.show();
  }
  strip.show();
}

void startLEDS() {
  
  clearLED();
  
  int arrayStart[] {4, 15, 23, 24, 25, 41, 43, 46, 48, 51, 53, 56, 57, 
      61, 62, 63, 66, 67, 68, 71, 73, 76, 78, 81, 82, 83, 86, 87, 88, 
      102, 106, 107, 108, 113, 117, 122, 126, 127, 128, 131, 137, 141, 142, 
      143, 146, 147, 148 
};

  for (int i = 0; i < 48; i++) {
    strip.setPixelColor(arrayStart[i], strip.Color(150, 0, 0));  // sets color of LEDS to green
    strip.show();
  }
  strip.show();
}





