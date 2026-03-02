#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// ***** Pin Definitions *****
#define FRONT_XSHUT    19  // GPIO for front sensor XSHUT (active low)
#define RIGHT_XSHUT    20  // GPIO for right sensor XSHUT
#define LEFT_XSHUT     18  // GPIO for left sensor XSHUT

// VL6180X I2C addresses after readdressing
#define ADDR_FRONT     0x30
#define ADDR_RIGHT     0x31
#define ADDR_LEFT      0x32

// Motor driver pins (H-bridge inputs)
#define LEFT_MOTOR_M1  14  // Left motor input 1
#define LEFT_MOTOR_M2  15  // Left motor input 2
#define RIGHT_MOTOR_M1 12  // Right motor input 1
#define RIGHT_MOTOR_M2 13  // Right motor input 2

// Encoder pins
#define LEFT_ENC_A     8   // Left encoder channel A
#define LEFT_ENC_B     9   // Left encoder channel B
#define RIGHT_ENC_A    10  // Right encoder channel A
#define RIGHT_ENC_B    11  // Right encoder channel B

// Button and buzzer
#define START_BUTTON   21  // Start button input (use INPUT_PULLUP)
#define BUZZER         17  // Buzzer output

// ***** Maze & Motion Constants *****
#define MAZE_SIZE      16  // Maze dimensions (cells)
#define CELL_SIZE_CM   16  // Cell width in centimeters (approx)
#define WALL_HEIGHT_CM 6   // Wall height (unused in code, for info)
#define WALL_THICK_CM  1.2 // Wall thickness (unused, for info)

// Encoder calibration (adjust for your robot)
#define TICKS_PER_CELL  100  // Encoder ticks per 16cm (forward one cell)
#define TICKS_PER_90    50   // Encoder ticks for a 90-degree turn (wheel rotation)
#define MOTOR_SPEED    200   // Base PWM speed (0-255)

// Sensor thresholds (in mm) to decide if a wall is present
#define WALL_THRESHOLD_FRONT  60
#define WALL_THRESHOLD_SIDE   35

#endif  // CONFIGURATION_H
