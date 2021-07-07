/*
PCA9685...........Uno/Nano
GND...............GND
OE................N/A
SCL...............A5
SDA...............A4
VCC...............5V
*/


/* Include the HCPCA9685 library */
#include <Servo.h>
#include <IRremote.h>
#include "HCPCA9685.h"

// 3 feet on ground: https://www.youtube.com/watch?v=hDyqtbeffLg
// 4 feet on ground: https://www.youtube.com/watch?v=qDp4ZnVyZ3E

/* I2C slave address for the device/module. For the HCMODU0097 the default I2C address
   is 0x40 */
#define  I2CAdd 0x40

/* Create an instance of the library */
HCPCA9685 HCPCA9685(I2CAdd);

/* Hexapod_Muscle
  An abstract level for the control of 18 muscles.
  Call muscle_move to write angle on a specific muscle.*/

int angle_map(int input) {
  // Assume input range 0-180
  return 30 + 2*input;
}

class Hexapod_Muscle {
private:
  Servo muscle_16;
  Servo muscle_17;
public:
  void setup() {
    HCPCA9685.Init(SERVO_MODE);
    HCPCA9685.Sleep(false);
    muscle_16.attach(3);
    muscle_17.attach(5);
  }

  void muscle_move(int leg, int muscle_no, int angle) {
    // leg range 0-5
    // muscle_no range 0-2
    // angle range 0-180
    if (angle > 180 || angle < 0)
      return;
      
    if (leg < 5) {
      int muscle = leg * 3 + muscle_no;
      HCPCA9685.Servo(muscle, angle_map(angle));
    } else if (leg == 5) {
      if (muscle_no == 0)
        HCPCA9685.Servo(15, angle_map(angle));
      else if (muscle_no == 1)
        muscle_16.write(angle);
      else if (muscle_no == 2)
        muscle_17.write(angle);
    }
  }
};

/* Hexapod_Pose
  An abstract level to control hexapod to move to a pre-programed pose
  */
class Hexapod_Pose {
private:
  Hexapod_Muscle hex_muscle;
  const int move_angle = 15;
  const int lift_angle = 60;
public:
  void setup() {
    hex_muscle.setup();
  }

  void reset() {
    for (int leg = 0; leg < 6; leg++)
      leg_pose(leg, 0);
  }

  // Walk mode: Tripod Gait
  void walk_pose(int walk_code) {
    // walk_code range 0-3
    int walk_code_0 = (walk_code % 4) + 1;
    int walk_code_1 = ((walk_code + 2) % 4) + 1;
    leg_pose(0, walk_code_0);
    leg_pose(1, walk_code_1);
    leg_pose(2, walk_code_0);
    leg_pose(3, walk_code_1);
    leg_pose(4, walk_code_0);
    leg_pose(5, walk_code_1);
  }

  void jump_pose(int jump_code) {
    // walk_code range 0-1
    int jump = (jump_code % 2) + 5;
    for (int leg = 0; leg < 6; leg++)
        leg_pose(leg, jump);
  }

  void leg_pose(int leg, int pose_code) {
    // leg 0-2 left side
    // leg 3-5 right side
    int muscle_zero_side_angle;
    int muscle_zero_neutral = 90;
    int muscle_one_ground = 15;
    int muscle_two_ground = 105;
    int lift_side_angle;
    int netural_pull_one = 90;
    int netural_pull_two = 180;
    if (leg <= 2) {
      // left side
      muscle_zero_side_angle = move_angle;
      lift_side_angle = lift_angle;
    } else {         
      // right side
      muscle_zero_side_angle = -move_angle;
      lift_side_angle = -lift_angle;
      muscle_one_ground = 180-muscle_one_ground;
      muscle_two_ground = 180-muscle_two_ground;
      netural_pull_one  = 180-netural_pull_one;
      netural_pull_two  = 180-netural_pull_two;
    }
    

    switch(pose_code) {
      case 0:
        // Reset pose
        hex_muscle.muscle_move(leg, 0, 90);
        hex_muscle.muscle_move(leg, 1, 90);
        hex_muscle.muscle_move(leg, 2, 90);
        break;
      case 1:
        // Forward, touch ground (treat left side as standard side)
        hex_muscle.muscle_move(leg, 0, muscle_zero_neutral - muscle_zero_side_angle);
        hex_muscle.muscle_move(leg, 1, muscle_one_ground);
        hex_muscle.muscle_move(leg, 2, muscle_two_ground);
        break;
      case 2:
        // Backward, touch ground
        hex_muscle.muscle_move(leg, 0, muscle_zero_neutral + muscle_zero_side_angle);
        hex_muscle.muscle_move(leg, 1, muscle_one_ground);
        hex_muscle.muscle_move(leg, 2, muscle_two_ground);
        break;
      case 3:
        // Backward, off ground
        hex_muscle.muscle_move(leg, 0, muscle_zero_neutral + muscle_zero_side_angle);
        hex_muscle.muscle_move(leg, 1, muscle_one_ground + lift_side_angle);
        hex_muscle.muscle_move(leg, 2, muscle_two_ground + lift_side_angle);
        break;
      case 4:
        // Forward, off ground
        hex_muscle.muscle_move(leg, 0, muscle_zero_neutral - muscle_zero_side_angle);
        hex_muscle.muscle_move(leg, 1, muscle_one_ground + lift_side_angle);
        hex_muscle.muscle_move(leg, 2, muscle_two_ground + lift_side_angle);
        break;
      case 5:
        // Neutral, pull
        hex_muscle.muscle_move(leg, 0, muscle_zero_neutral);
        hex_muscle.muscle_move(leg, 1, netural_pull_one);
        hex_muscle.muscle_move(leg, 2, netural_pull_two);
        break;
      case 6:
        // Netural, push
        hex_muscle.muscle_move(leg, 0, muscle_zero_neutral);
        hex_muscle.muscle_move(leg, 1, muscle_one_ground);
        hex_muscle.muscle_move(leg, 2, muscle_two_ground);
        break;
    }
    //Serial.println("Leg: " + to_string(leg) + ", Pose: " + to_string(pose_code));
  }
};


IRrecv irrecv(8);
Hexapod_Pose hex_pose;
int mode = 0;
int walk = 0;
int jump = 0;
int sign_pause = 0;

void setup() 
{
  // Attach muscle 16 to port D3, 17 to port D5
  Serial.begin(9600);
  irrecv.enableIRIn(); // Enable
  hex_pose.setup();
}


void loop() 
{
  if (irrecv.decode()) {
    irrecv.resume();
    Serial.println(irrecv.decodedIRData.command);
    if (sign_pause == 0 && irrecv.decodedIRData.command == 28) {
      //OK
      sign_pause = 1;
      mode = 0;
      walk = 0;
      jump = 0;
      hex_pose.reset();
    }
    else if (sign_pause == 0 && irrecv.decodedIRData.command == 90) {
      //Right
      sign_pause = 1;
      mode = 2;
    } else if (sign_pause == 0 && irrecv.decodedIRData.command == 8) {
      //Left
    } else if (sign_pause == 0 && irrecv.decodedIRData.command == 24) {
      //UP
      sign_pause = 1;
      mode = 1;
    } else if (sign_pause == 0 && irrecv.decodedIRData.command == 82) {
      //DOWN
    }
    delay(200);
    sign_pause = 0;
  }
  
  if (mode == 1) {
    // Walk loop
    hex_pose.walk_pose(walk);
    walk = (walk + 1) % 4;
    delay(200);
  } else if (mode == 2) {
    // Jump once
    hex_pose.jump_pose(jump);
    jump = jump + 1;
    delay(500);
    if (jump > 1) {
      jump = 0;
      mode = 0;
    }
  }
}
