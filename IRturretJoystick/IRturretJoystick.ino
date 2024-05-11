/*

 ************************************************************************************
 * MIT License
 *
 * Copyright (c) 2023 Crunchlabs LLC (IRTurret Control Code)
 * Copyright (c) 2020-2022 Armin Joachimsmeyer (IRremote Library)

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************************
 */

//////////////////////////////////////////////////
               //  LIBRARIES  //
//////////////////////////////////////////////////
#include <Arduino.h>
#include <Servo.h>
#include "PinDefinitionsAndMore.h" // Define macros for input and output pin etc.
#include <IRremote.hpp>
#include <ezButton.h>



//////////////////////////////////////////////////
          //  PINS AND PARAMETERS  //
//////////////////////////////////////////////////
Servo yawServo; //names the servo responsible for YAW rotation, 360 spin around the base
Servo pitchServo; //names the servo responsible for PITCH rotation, up and down tilt
Servo rollServo; //names the servo responsible for ROLL rotation, spins the barrel to fire darts

int yawServoVal = 90; //initialize variables to store the current value of each servo
int pitchServoVal = 100;
int rollServoVal = 0;
int lastPVal = 0;
int lastYVal = 0;
int targetYVal = 90;
int targetPVal = 90;

// Joystick setup
ezButton button1(18); // button handler
//ezButton button2(17); //button handler

bool joyUp;
bool joyDown;
bool joyLeft;
bool joyRight;
int button1State = 0;
int button2State = 0;
int joystickX;
int joystickY;

const int joystickXPin = A5;  // Connect the joystick X-axis to this analog pin
const int joystickYPin = A6;  // Connect the joystick Y-axis to this analog pin
const int joystickButtonThreshold = 200;  // Adjust this threshold value based on your joystick


#define MAX_POSITIONS 6 // Maximum number of positions to record

int recordedPositions[MAX_POSITIONS][2]; // Array to store recorded positions [yaw, pitch]
int numRecordedPositions = 0; // Counter for the number of recorded positions

//////////////////////////////////////////////////
               //  S E T U P  //
//////////////////////////////////////////////////
void setup() {
    Serial.begin(9600);

    yawServo.attach(10); //attach YAW servo to pin 3
    pitchServo.attach(11); //attach PITCH servo to pin 4
    rollServo.attach(12); //attach ROLL servo to pin 5

    button1.setDebounceTime(150);
    //button2.setDebounceTime(50);

    // yawServo.write(90); //setup YAW servo to be STOPPED (90)
    // delay(20);
    // rollServo.write(90); //setup ROLL servo to be STOPPED (90)
    // delay(100);
    // pitchServo.write(100); //set PITCH servo to 100 degree position
    // delay(100);
    // pitchServoVal = 100;
}

////////////////////////////////////////////////
               //  L O O P  //
////////////////////////////////////////////////

void loop() {
    button1.loop();
    //button2.loop();
    button1State = button1.getState();
    //button2State = button2.getState();

    joystickX = analogRead(joystickXPin);
    joystickY = analogRead(joystickYPin);
    targetPVal = map(joystickY, 1023, 0, 30, 160);
    targetYVal = map(joystickX, 0, 1023, 20, 160);

    pitchServoVal = (targetPVal*0.25) + (lastPVal*0.75);
    yawServoVal = (targetYVal*0.3) + (lastYVal*0.7);

    if (yawServoVal != lastYVal){
      yawServo.write(yawServoVal);
      delay(25);
    }

    if (pitchServoVal != lastPVal){
      pitchServo.write(pitchServoVal);
      delay(25);
    }

    if (button1.isPressed()) {
      if (numRecordedPositions != 0){
        executeRecordedPositions();
      }else{
        fire();
        delay(25);
      }
    }

    // if (button2.isPressed()) {
    //   //recordPosition();
    //   Serial.print("YIPPEE");
    // }

    // Serial.print("Yaw: ");
    // Serial.print(joystickY);
    // Serial.print("  Pitch: ");
    // Serial.print(joystickX);
    // Serial.print(" Firing: ");
    // Serial.println(button1State);
    
    lastPVal = pitchServoVal;
    lastYVal = yawServoVal;

    delay(5);
}



void fire(){ //function for firing a single dart
  rollServo.write(180);//start rotating the servo
  delay(120);//time for approximately 60 degrees of rotation
  rollServo.write(90);//stop rotating the servo
  delay(10);
  return;
}

void fireAll(){
  rollServo.write(180);//start rotating the servo
  delay(1500);//time for 360 degrees of rotation
  rollServo.write(90);//stop rotating the servo
  delay(5);
}

void recordPosition() {
  // Record current position of the servo motors
  recordedPositions[numRecordedPositions][0] = yawServoVal;
  recordedPositions[numRecordedPositions][1] = pitchServoVal;
    
  numRecordedPositions++;
  if (numRecordedPositions >= MAX_POSITIONS) {
    numRecordedPositions = MAX_POSITIONS; // Cap the number of recorded positions
  }
}

void executeRecordedPositions() {
    // Iterate through recorded positions and swing to each position while firing
    for (int i = 0; i < numRecordedPositions; i++) {
        // Move to recorded position
        yawServo.write(recordedPositions[i][0]);
        pitchServo.write(recordedPositions[i][1]);
        delay(150); // Adjust delay as needed
        
        // Fire a dart
        fire();
        delay(150); // Delay between firing at each position, adjust as needed
    }
    numRecordedPositions = 0;
}
