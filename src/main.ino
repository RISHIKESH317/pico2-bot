#include <Wire.h>
#include <Adafruit_VL6180X.h>
#include "configuration.h"
#include "maze.h"

// Sensor objects
Adafruit_VL6180X sensorFront = Adafruit_VL6180X();
Adafruit_VL6180X sensorRight = Adafruit_VL6180X();
Adafruit_VL6180X sensorLeft  = Adafruit_VL6180X();

// Encoder tick counters (updated in ISR)
volatile long leftTicks = 0;
volatile long rightTicks = 0;

// Current robot state
int posX = 0, posY = 0;  // Robot starts at maze corner (0,0)
int orientation = 0;     // 0=N,1=E,2=S,3=W (start facing North)

// Flag for mapping vs. best-run
bool mappingDone = false;

// Encoder interrupt service routines
void leftEncoderISR() {
  // If channel B is HIGH, count up; else count down.
  if (digitalRead(LEFT_ENC_B)) leftTicks++;
  else leftTicks--;
}
void rightEncoderISR() {
  if (digitalRead(RIGHT_ENC_B)) rightTicks++;
  else rightTicks--;
}

// Set left motor speed: speed > 0 forward, < 0 backward
void setLeftMotor(int speed) {
  if (speed > 0) {
    analogWrite(LEFT_MOTOR_M1, speed);
    analogWrite(LEFT_MOTOR_M2, 0);
  } else if (speed < 0) {
    analogWrite(LEFT_MOTOR_M1, 0);
    analogWrite(LEFT_MOTOR_M2, -speed);
  } else {
    analogWrite(LEFT_MOTOR_M1, 0);
    analogWrite(LEFT_MOTOR_M2, 0);
  }
}
// Set right motor speed: speed > 0 forward, < 0 backward
void setRightMotor(int speed) {
  if (speed > 0) {
    analogWrite(RIGHT_MOTOR_M1, speed);
    analogWrite(RIGHT_MOTOR_M2, 0);
  } else if (speed < 0) {
    analogWrite(RIGHT_MOTOR_M1, 0);
    analogWrite(RIGHT_MOTOR_M2, -speed);
  } else {
    analogWrite(RIGHT_MOTOR_M1, 0);
    analogWrite(RIGHT_MOTOR_M2, 0);
  }
}
// Stop both motors
void stopMotors() {
  setLeftMotor(0);
  setRightMotor(0);
}

// Drive forward approximately one cell (using encoder counts)
void driveCell() {
  long startLeft = leftTicks;
  long startRight = rightTicks;
  setLeftMotor(MOTOR_SPEED);
  setRightMotor(MOTOR_SPEED);
  // Wait until sum of encoder increments reaches threshold
  while (abs((leftTicks - startLeft) + (rightTicks - startRight)) < 2 * TICKS_PER_CELL) {
    // Busy wait; in a real robot, you might add safety checks here
  }
  stopMotors();
}

// Turn 90 degrees left (90° CCW) or right (CW)
void turn90(bool rightTurn) {
  long startLeft = leftTicks;
  long startRight = rightTicks;
  if(rightTurn) {
    // Left wheel forward, right wheel backward
    setLeftMotor(MOTOR_SPEED);
    setRightMotor(-MOTOR_SPEED);
  } else {
    // Left backward, right forward
    setLeftMotor(-MOTOR_SPEED);
    setRightMotor(MOTOR_SPEED);
  }
  // Rotate until combined encoder ticks indicate ~90 degrees
  while (abs((leftTicks - startLeft) + (rightTicks - startRight)) < 2 * TICKS_PER_90) {
  }
  stopMotors();
  // Update orientation state
  if(rightTurn) orientation = (orientation + 1) % 4;
  else           orientation = (orientation + 3) % 4;
}

// Read sensors and update the walls array at current cell (posX,posY)
void senseWalls() {
  // Read distances (mm)
  uint8_t stat;
  uint16_t dF = sensorFront.readRangeSingleMillimeters();
  uint16_t dR = sensorRight.readRangeSingleMillimeters();
  uint16_t dL = sensorLeft.readRangeSingleMillimeters();
  bool frontWall = (dF < WALL_THRESHOLD_FRONT);
  bool rightWall = (dR < WALL_THRESHOLD_SIDE);
  bool leftWall  = (dL < WALL_THRESHOLD_SIDE);
  // Map sensor readings to global directions based on orientation
  // 0=N,1=E,2=S,3=W
  if (orientation == 0) { // Facing North
    if(frontWall) { walls[posX][posY] |= WALL_N; if(posY<SIZE-1) walls[posX][posY+1] |= WALL_S; }
    if(rightWall) { walls[posX][posY] |= WALL_E; if(posX<SIZE-1) walls[posX+1][posY] |= WALL_W; }
    if(leftWall)  { walls[posX][posY] |= WALL_W; if(posX>0) walls[posX-1][posY] |= WALL_E; }
  }
  else if (orientation == 1) { // Facing East
    if(frontWall) { walls[posX][posY] |= WALL_E; if(posX<SIZE-1) walls[posX+1][posY] |= WALL_W; }
    if(rightWall) { walls[posX][posY] |= WALL_S; if(posY>0) walls[posX][posY-1] |= WALL_N; }
    if(leftWall)  { walls[posX][posY] |= WALL_N; if(posY<SIZE-1) walls[posX][posY+1] |= WALL_S; }
  }
  else if (orientation == 2) { // Facing South
    if(frontWall) { walls[posX][posY] |= WALL_S; if(posY>0) walls[posX][posY-1] |= WALL_N; }
    if(rightWall) { walls[posX][posY] |= WALL_W; if(posX>0) walls[posX-1][posY] |= WALL_E; }
    if(leftWall)  { walls[posX][posY] |= WALL_E; if(posX<SIZE-1) walls[posX+1][posY] |= WALL_W; }
  }
  else { // orientation == 3 (West)
    if(frontWall) { walls[posX][posY] |= WALL_W; if(posX>0) walls[posX-1][posY] |= WALL_E; }
    if(rightWall) { walls[posX][posY] |= WALL_N; if(posY<SIZE-1) walls[posX][posY+1] |= WALL_S; }
    if(leftWall)  { walls[posX][posY] |= WALL_S; if(posY>0) walls[posX][posY-1] |= WALL_N; }
  }
}

bool atGoal() {
  // Check if current cell is one of the center 2x2 (7-8,7-8)
  return (posX == 7 || posX == 8) && (posY == 7 || posY == 8);
}

void setup() {
  Serial.begin(115200);
  // Initialize pins
  pinMode(FRONT_XSHUT, OUTPUT);
  pinMode(RIGHT_XSHUT, OUTPUT);
  pinMode(LEFT_XSHUT, OUTPUT);
  digitalWrite(FRONT_XSHUT, LOW);
  digitalWrite(RIGHT_XSHUT, LOW);
  digitalWrite(LEFT_XSHUT, LOW);
  
  pinMode(LEFT_MOTOR_M1, OUTPUT);
  pinMode(LEFT_MOTOR_M2, OUTPUT);
  pinMode(RIGHT_MOTOR_M1, OUTPUT);
  pinMode(RIGHT_MOTOR_M2, OUTPUT);
  
  pinMode(LEFT_ENC_A, INPUT_PULLUP);
  pinMode(LEFT_ENC_B, INPUT_PULLUP);
  pinMode(RIGHT_ENC_A, INPUT_PULLUP);
  pinMode(RIGHT_ENC_B, INPUT_PULLUP);
  
  pinMode(START_BUTTON, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  
  // Attach encoder interrupts
  attachInterrupt(digitalPinToInterrupt(LEFT_ENC_A), leftEncoderISR, RISING);
  attachInterrupt(digitalPinToInterrupt(RIGHT_ENC_A), rightEncoderISR, RISING);
  
  // Initialize VL6180X sensors one by one, assigning unique I2C addresses:contentReference[oaicite:7]{index=7}
  pinMode(FRONT_XSHUT, OUTPUT);  digitalWrite(FRONT_XSHUT, LOW);
  pinMode(RIGHT_XSHUT, OUTPUT);  digitalWrite(RIGHT_XSHUT, LOW);
  pinMode(LEFT_XSHUT, OUTPUT);   digitalWrite(LEFT_XSHUT, LOW);
  
  // Start front sensor
  pinMode(FRONT_XSHUT, INPUT);
  delay(50);
  sensorFront.init();
  sensorFront.configureDefault();
  sensorFront.setAddress(ADDR_FRONT);
  
  // Start right sensor
  pinMode(RIGHT_XSHUT, INPUT);
  delay(50);
  sensorRight.init();
  sensorRight.configureDefault();
  sensorRight.setAddress(ADDR_RIGHT);
  
  // Start left sensor
  pinMode(LEFT_XSHUT, INPUT);
  delay(50);
  sensorLeft.init();
  sensorLeft.configureDefault();
  sensorLeft.setAddress(ADDR_LEFT);
  
  Serial.println("Sensors initialized. Press the start button to begin.");
  
  initMaze();  // Clear maze structures and set border walls
}

void loop() {
  // Wait for start button press
  if (!mappingDone) {
    while(digitalRead(START_BUTTON) == HIGH) {
      // waiting
    }
    delay(50);
    
    Serial.println("Starting mapping run...");
    // --- Mapping stage ---
    visited[posX][posY] = true;
    senseWalls();
    floodFill();
    
    // Explore until we reach goal
    while(!atGoal()) {
      int moveCmd = chooseMove(posX, posY, orientation);
      if(moveCmd < 0) {
        Serial.println("No valid move found! Stopping.");
        break;
      }
      // Execute turn if needed
      if(moveCmd == 1) { turn90(false); }  // left
      if(moveCmd == 2) { turn90(true); }   // right
      if(moveCmd == 3) {                      // U-turn
        turn90(true);
        turn90(true);
      }
      // Move forward one cell
      driveCell();
      // Update (posX,posY) based on orientation
      if(orientation == 0) posY++;
      else if(orientation == 1) posX++;
      else if(orientation == 2) posY--;
      else if(orientation == 3) posX--;
      visited[posX][posY] = true;
      
      // Sense walls and update flood-fill
      senseWalls();
      floodFill();
    }
    // Goal reached
    Serial.println("Reached goal during mapping.");
    // Beep buzzer to signal end of mapping
    digitalWrite(BUZZER, HIGH);
    delay(200);
    digitalWrite(BUZZER, LOW);
    mappingDone = true;
  }
  
  // --- Best-run stage (fastest path using known map) ---
  // Reset position to start (user would physically place robot at start)
  posX = 0; posY = 0; orientation = 0;
  visited[0][0] = true;
  floodFill();  // recompute distances now that walls are known
  Serial.println("Starting best-run (fastest path)...");
  
  // Follow shortest path
  while(!atGoal()) {
    int moveCmd = chooseMove(posX, posY, orientation);
    if(moveCmd < 0) {
      Serial.println("No valid move in best-run!");
      break;
    }
    // Execute turn if needed
    if(moveCmd == 1) { turn90(false); }
    if(moveCmd == 2) { turn90(true); }
    if(moveCmd == 3) { turn90(true); turn90(true); }
    // Move forward
    driveCell();
    if(orientation == 0) posY++;
    else if(orientation == 1) posX++;
    else if(orientation == 2) posY--;
    else if(orientation == 3) posX--;
  }
  Serial.println("Best-run completed. Maze solved!");
  while(1);  // End of program
}