#include <Wire.h>                 
// I2C communication library 
#include "M5_UNIT_8SERVO.h"       
// M5Stack 8-servo driver library 
#include <M5Unified.h>            
// M5Stack core functions 
// Create servo controller object 
M5_UNIT_8SERVO walk_bot; 
// I2C address of the joystick module 
#define JOY_ADDR 0x53 
// Servo channel assignments for the left leg 
#define L_foot 0 
#define L_knee 1 
#define L_hip  2 
// Servo channel assignments for the right leg 
#define R_foot 4 
#define R_knee 5 
#define R_hip  6 
// Current servo positions 
// {Left Foot, Left Knee, Left Hip, Right Foot, Right Knee, Right Hip} 
int pos[6] = {90, 80, 180, 95, 110, 20}; 
// Read the Y-axis value from the joystick 
int readJoyY() { 
// Select joystick Y-axis register 
Wire.beginTransmission(JOY_ADDR); 
Wire.write(0x10); 
Wire.endTransmission(false); 
 
  // Request 2 bytes from joystick 
  Wire.requestFrom(JOY_ADDR, 2); 
  
  int yL = Wire.read();   // Low byte 
  int yH = Wire.read();   // High byte 
  
  // Combine bytes into a 16-bit value 
  return yL | (yH << 8); 
} 
  
  
// Read joystick button state 
// Returns true when pressed 
bool readJoyButton() { 
  
  // Select button register 
  Wire.beginTransmission(JOY_ADDR); 
  Wire.write(0x20); 
  Wire.endTransmission(false); 
  
  // Request button status byte 
  Wire.requestFrom(JOY_ADDR, 1); 
  
  int btn = Wire.read(); 
  
  // Button returns 0 when pressed 
  return btn == 0; 
} 
  
  
// Move all servos smoothly to target positions 
// speedDelay controls movement speed 
 
void setLegs(int lf, int lk, int lh, 
             int rf, int rk, int rh, 
             int speedDelay) 
{ 
  // Store target positions 
  int target[6] = {lf, lk, lh, rf, rk, rh}; 
  
  // Interpolate movement over 50 small steps 
  for (int step = 0; step <= 50; step++) 
  { 
    // Left leg movement 
    walk_bot.setServoAngle( 
      L_foot, 
      pos[0] + (target[0] - pos[0]) * step / 50); 
  
    walk_bot.setServoAngle( 
      L_knee, 
      pos[1] + (target[1] - pos[1]) * step / 50); 
 
  
    walk_bot.setServoAngle( 
      L_hip, 
      pos[2] + (target[2] - pos[2]) * step / 50); 
  
    // Right leg movement 
    walk_bot.setServoAngle( 
      R_foot, 
      pos[3] + (target[3] - pos[3]) * step / 50); 
  
    walk_bot.setServoAngle( 
      R_knee, 
      pos[4] + (target[4] - pos[4]) * step / 50); 
  
    walk_bot.setServoAngle( 
      R_hip, 
      pos[5] + (target[5] - pos[5]) * step / 50); 
  
    // Delay between each small movement step 
    delay(speedDelay); 
  } 
  
  // Save new servo positions 
  for (int i = 0; i < 6; i++) 
    pos[i] = target[i]; 
} 
  
  
// Neutral standing position 
// Robot returns to this pose when idle 
void neutralPose() 
{ 
  setLegs(100, 80, 180, 
          85, 110, 20, 
          20); 
} 
  
  
// Single walking step 
// Moves legs forward then returns to neutral 
 
void walkStep() 
{ 
  // Walking pose 
  setLegs(120, 20, 150, 60, 160, 40,20); 
  
  // Return to standing pose 
  setLegs(100, 80, 180, 85, 110, 20, 20); 
} 
  
  
 
// Squatting pose 
// Activated by joystick button 
 
void squatPose() 
{ 
  setLegs(95, 40, 165, 
          90, 150, 35, 
          20); 
} 
  
  
// Initial setup 
// Runs once when robot powers on 
 
void setup() 
{ 
  // Initialise M5Stack system 
  M5.begin(); 
  
  // Start I2C communication 
  Wire.begin(9, 10); 
  
  // Initialise servo controller 
  walk_bot.begin( 
    &Wire, 
    9, 
    10, 
    M5_UNIT_8SERVO_DEFAULT_ADDR); 
  
  // Set all channels to servo output mode 
  walk_bot.setAllPinMode(SERVO_CTL_MODE); 
  
  // Move robot to starting position 
  neutralPose(); 
} 
  
  
// Main control loop 
// Continuously checks joystick input 
 
void loop() 
{ 
  // Update M5 system 
  M5.update(); 
  
  // Read joystick Y-axis value 
  int joyY = readJoyY(); 
  
  // Read joystick button state 
  bool buttonPressed = readJoyButton(); 
  
// Button pressed = squat 
if (buttonPressed) 
{ 
squatPose(); 
} 
// Joystick pushed forward = walk 
else if (joyY > 3000) 
{ 
walkStep(); 
} 
// Otherwise stand still 
else 
{ 
} 
} 
neutralPose();