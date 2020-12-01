/* THE ARDUINO CAT ENTERTAINMENT SYSTEM
 * Version 1.0
 * 28 November 2020
 * Justin E. Oxford
 * www.justinoxford.com
 * 
 * 
 * A simple Arduino program that randomly moves a laser around on the floow, ideally your cat will chase the laser. Less ideally, 
 * your cat will attack the Arduino motors as they buzz and zip in the sweet rhythm of pseudo-random integers. 
 * 
 * 
 * HARDWARE:
 * 3 x SG 90, 180 degree servo motors
 * 1 x Arduino CPU, at kind will do
 * 1 x Breadboard
 * 3 x Wood Blocks, or anything else you would like to use to mount the motors
 * 1 x Styrofoam Pad, or anything else you would like to use to mound the entire system
 * & enough wires for everything you need.
 * 
 * Schematics will not be provided as wiring up 3 servo motors is a fairly simple task.
 * 
 * 
 * MIT License
 *
 * Copyright (c) 2020 Justin E. Oxford
  
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * 
 * 
 * SYSTEM ORIENTATION
 * As I speak often in terms of left/right servos below, it's importation to visualize what I am talking about. 
 * Below is the layout of this system, I am not an ASCII artist:
 * 
 *  ---------------------------------------
 *  |                                     | 
 *  |  ---------------------   |-----|    | 
 *  |  |  BREADBOARD       |   |  C  |    | 
 *  |  |                   |   |  P  |    | 
 *  |  ---------------------   |  U  |    | 
 *  |                          |-----|    |
 *  |                                     |
 *  |                                     |
 *  |                                     |
 *  |                                     |
 *  |                                     |
 *  |   |-------|  |-------|  |-------|   |
 *  |   | LEFT  |--|  MID  |--| RIGHT |   |
 *  |   | servo |  | servo |  | servo |   |
 *  ---------------------------------------
 *                   laser
 *                     |
 */


// INCLUDES
// -----------------------------------------------------
#include <Servo.h>
// -----------------------------------------------------


// CONSTANTS
// -----------------------------------------------------
// Maximum Range, in degrees, the system can rotate up or down. 90 is HIGHLY recommended as this means straight down to horizon.
const int MAX_UP_DOWN = 90;

// Maximum Range, in degrees, the system can rotate left/right. The hardware is limited to 180, but this can be changed
// based on how high the system is mounted. 120 is a good middle ground.
const int MAX_LEFT_RIGHT = 120;


// OBJECTS & setup variables
// -----------------------------------------------------
// the servo's
Servo servo_MAIN;  // the top servo that holds the laser mount itself
Servo servo_LEFT;  // the servo on the base that is on the LEFT per the system orientation, above
Servo servo_RIGHT; // the servo on the base that is on the RIGHT per the system orientation, above

/* set inital servo angles
 * 0 - LEFT; 1 - TOP/MID; 2 - RIGHT;
 * !! IMPORTANT: note that the left and right servos are opposite of each other, this means that when one increases
 *               the angle, the other must decrease. If you set [0] and [2] to the same value, you can damage the
 *               motors as they will be working against each other if they are pulling opposite directions.
 * SEE LAYOUT in the notes above to understand which is LEFT
  */
int servo_angles[3] = {135, 90, 45};

int new_up_down;     // init for the random vertical (up-down) movement
int new_left_right;  // init for the random horizontal (left-right) movement
int new_speed;       // init for the random speed

int incomingByte = 0; // for incoming serial data


// FCNS
// -----------------------------------------------------

/*
 * MOVE UP DOWN function controls the two servos that rotate the middle block
 * This function takes in the parameter new_up_down which denotes the target
 * that the motor should move towards. This parameter is a random value.
 * When the motors move, they move opposite of each other. This is because
 * they are mounted facing the center. Do not change the incrementers unless 
 * you are changing the physical setup as this can damage the motors, if they
 * do not rotate in unison.
 * 
 * NOTE, all rotation is in terms of the LEFT motor, so
 * if LEFT => at its MAX then the systems net rotation is 0. the laser points out to the horizon
 * if LEFT => at its MIN then the systems net rotation is MAX_UP_DOWN. the laser points down to the ground, if this is 90
 * 
 * You are free to expand these ranges as you wish, just know you may rotate back into your model.
 */
void move_up_down(int new_up_down)
{
  /
  if (new_up_down < servo_angles[0])
  {
    servo_angles[0] -= 1; // increment LEFT motor in range:  [135 --->  45]
    servo_angles[2] += 1; // increment RIGHT motor in range: [45  ---> 135]
  }
  else if (new_up_down > servo_angles[0])
  {
    servo_angles[0] += 1; // increment LEFT motor in range:  [45  ---> 135]
    servo_angles[2] -= 1; // increment RIGHT motor in range: [135 --->  45]
  }
  
  // add some delay to the rotation as the angle gets closer to the horizon to keep the laser's ground speed consistent
  delay(servo_angles[0] / 4);

  // write the new angles to the servo
  servo_LEFT.write(servo_angles[0]);
  servo_RIGHT.write(servo_angles[2]);
}

/*
 * MOVE LEFT RIGHT function controls the middle servo that actually holds the laser
 * This function takes in the parameter new_left_right which denotes the target
 * that the motor should move towards. This parameter is a random value passed from the main loop.
 * 
 * You can change the range in the constants above. The 'middle'/'initial'/'centered'  rotational value is always 90 degrees
 */
void move_left_right(int new_left_right)
{
  if (new_left_right > servo_angles[1])
  {
    servo_angles[1] += 1; // increment TOP motor in range:  [30  --->  150]
  }
  else if (new_left_right < servo_angles[1])
  {
    servo_angles[1] -= 1; // increment TOP motor in range:  [150 --->  30]
  }

  // write the new angle to the servo
  servo_MAIN.write(servo_angles[1]);
}

// SETUP FCN
// -----------------------------------------------------
void setup() {
  // attach the servos
  servo_LEFT.attach(5);
  servo_MAIN.attach(6);
  servo_RIGHT.attach(7);

  // initalize servos to their default positions
  servo_LEFT.write(servo_angles[0]);
  servo_MAIN.write(servo_angles[1]);
  servo_RIGHT.write(servo_angles[2]);

  // init the serial
  Serial.begin(9600);

  // get a random seed going
  randomSeed(analogRead(0));
}


// LOOP FCN
// -----------------------------------------------------
void loop() {
  // set the new up-down rotation as an angle the size of MAX_UP_DOWN
  new_up_down = (90 + (MAX_UP_DOWN / 2)) - random(MAX_UP_DOWN);
  
  // set the new left-right rotation as an angle the size of MAX_LEFT_RIGHT
  new_left_right = (90 + (MAX_LEFT_RIGHT / 2)) - random(MAX_LEFT_RIGHT);

  // set the speed to a random value, stay about 50ms as any faster cause the cat to lose sight
  new_speed = 75 + random(50);

  // while the servo isn't set to the new directions, run the iterators
  while(
    servo_angles[0] != new_up_down && 
    servo_angles[1] != new_left_right
    )
  {
    move_up_down(new_up_down);       // call function to step towards up-down target
    move_left_right(new_left_right); // call function to step towards left-right target
    delay(new_speed);                // wait before stepping again
  }

  // WAIT between 1 second and 3 seconds before moving again
  // I found this was the sweet spot as it gives the cat time to zero in
  delay(1000 + random(2000));
}
