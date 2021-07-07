#include <Servo.h>
#include <IRremote.h>

// 3 feet on ground: https://www.youtube.com/watch?v=hDyqtbeffLg
// 4 feet on ground: https://www.youtube.com/watch?v=qDp4ZnVyZ3E

const int move_angle = 15;
const int lift_angle = 45;
Servo leftLeg[3];
Servo rightLeg[3];
Servo left_mid1;
Servo right_mid1;

int pose = 0;
int alternation = 0;
int looping = 0;
IRrecv irrecv(8);
//decode_results results;
//int potpin = 0;  // analog pin used to connect the potentiometer
//int val;    // variable to read the value from the analog pin

void leftLeg_walk(int reset);
void walk(int reset);
void left_pose(int pose_code, Servo muscle1, Servo muscle2, Servo muscle3);
void right_pose(int pose_code, Servo muscle1, Servo muscle2, Servo muscle3);
void right_mid_pose(int pose_code, Servo muscle1);
void left_mid_pose(int pose_code, Servo muscle1);

void setup() {
    leftLeg[0].attach(9);
    leftLeg[1].attach(10);
    leftLeg[2].attach(11);
    
    Serial.begin(9600); // Start
    irrecv.enableIRIn();// Enable
}

void loop() {
    //val = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
    //val = map(val, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
    if (irrecv.decode()) {
        irrecv.resume();
        Serial.println(irrecv.decodedIRData.command);
        if (alternation == 0 && irrecv.decodedIRData.command == 90) {
            //Right
            alternation = 1;
            walk(0);
            delay(100);
        } else if (alternation == 1 && irrecv.decodedIRData.command == 8) {
            //Left
            alternation = 0;
            walk(0);
            delay(100);
        } else if (irrecv.decodedIRData.command == 28) {
            //OK
            alternation = 0;
            looping = 0;
            walk(1);
            delay(100);
        } else if (irrecv.decodedIRData.command == 24) {
            //UP
            alternation = 2;
            looping = 1;
        } else if (irrecv.decodedIRData.command == 82) {
            //DOWN
            alternation = 2;
            looping = 2;
        }
        
    }
    if (looping == 1) {
        leftLeg_walk(0);
        delay(200);
    } else if (looping == 2) {
        walk(0);
        delay(200);
    }
}

// ------------------------- HELPER FUNCTIONS ----------------------------
void leftLeg_walk(int reset) {
    if (reset == 0)
        left_pose(pose, leftLeg[0], leftLeg[1], leftLeg[2]);
    else if (reset == 1) {
        left_pose(4, leftLeg[0], leftLeg[1], leftLeg[2]);
        pose = 3;
    }
    pose = (pose + 1) % 4;
}

void walk(int reset) {
    if (reset == 0) {
        int pose_r = (pose + 2) % 4;
        left_pose(pose, leftLeg[0], leftLeg[1], leftLeg[2]);
        //right_pose(pose_r, rightLeg[0], rightLeg[1], rightLeg[2]);
        //left_mid_pose(pose_r, left_mid1);
        //right_mid_pose(pose, right_mid1);
    } else if (reset == 1) {
        left_pose(4, leftLeg[0], leftLeg[1], leftLeg[2]);
        //right_pose(4, rightLeg[0], rightLeg[1], rightLeg[2]);
        //left_mid_pose(4, left_mid1);
        //right_mid_pose(4, right_mid1);
        pose = 3;
    }
    pose = (pose + 1) % 4;
}

void left_pose(int pose_code, Servo muscle1, Servo muscle2, Servo muscle3) {
    if (pose_code == 0) {
        // Forward, touch ground
        muscle1.write(90 - move_angle);
        muscle2.write(15);
        muscle3.write(105);
        Serial.println("left_pose: 0");
    } else if (pose_code == 1) {
        // Backward, touch ground
        muscle1.write(90 + move_angle);
        muscle2.write(15);
        muscle3.write(105);
        Serial.println("left_pose: 1");
    } else if (pose_code == 2) {
        // Backward, off ground
        muscle1.write(90 + move_angle);
        muscle2.write(15 + lift_angle);
        muscle3.write(105 + lift_angle);
        Serial.println("left_pose: 2");
    } else if (pose_code == 3) {
        // Forward, off ground
        muscle1.write(90 - move_angle);
        muscle2.write(15 + lift_angle);
        muscle3.write(105 + lift_angle);
        Serial.println("left_pose: 3");
    } else if (pose_code == 4) {
        // Resest 
        muscle1.write(90);
        muscle2.write(90);
        muscle3.write(90);
        Serial.println("left_pose: 4");
    }
}

void right_pose(int pose_code, Servo muscle1, Servo muscle2, Servo muscle3) {
    if (pose_code == 0) {
        // Forward, touch ground
        muscle1.write(90 + move_angle);
        muscle2.write(15);
        muscle3.write(105);
        Serial.println("right_pose: 0");
    } else if (pose_code == 1) {
        // Backward, touch ground
        muscle1.write(90 - move_angle);
        muscle2.write(15);
        muscle3.write(105);
        Serial.println("right_pose: 1");
    } else if (pose_code == 2) {
        // Backward, off ground
        muscle1.write(90 - move_angle);
        muscle2.write(15 + lift_angle);
        muscle3.write(105 + lift_angle);
        Serial.println("right_pose: 2");
    } else if (pose_code == 3) {
        // Forward, off ground
        muscle1.write(90 + move_angle);
        muscle2.write(15 + lift_angle);
        muscle3.write(105 + lift_angle);
        Serial.println("right_pose: 3");
    } else if (pose_code == 4) {
        // Resest 
        muscle1.write(90);
        muscle2.write(90);
        muscle3.write(90);
        Serial.println("right_pose: 4");
    }
}

void right_mid_pose(int pose_code, Servo muscle1) {
    if (pose_code == 0) {
        // Forward, touch ground
        muscle1.write(90 + move_angle);
        Serial.println("right_mid: 0");
    } else if (pose_code == 1) {
        // Backward, touch ground
        muscle1.write(90 - move_angle);
        Serial.println("right_mid: 1");
    } else if (pose_code == 2) {
        // Backward, off ground
        muscle1.write(90 - move_angle);
        Serial.println("right_mid: 2");
    } else if (pose_code == 3) {
        // Forward, off ground
        muscle1.write(90 + move_angle);
        Serial.println("right_mid: 3");
    } else if (pose_code == 4) {
        // Resest 
        muscle1.write(90);
        Serial.println("right_mid: 4");
    }
}

void left_mid_pose(int pose_code, Servo muscle1) {
    if (pose_code == 0) {
        // Forward, touch ground
        muscle1.write(90 - move_angle);
        Serial.println("left_mid: 0");
    } else if (pose_code == 1) {
        // Backward, touch ground
        muscle1.write(90 + move_angle);
        Serial.println("left_mid: 1");
    } else if (pose_code == 2) {
        // Backward, off ground
        muscle1.write(90 + move_angle);
        Serial.println("left_mid: 2");
    } else if (pose_code == 3) {
        // Forward, off ground
        muscle1.write(90 - move_angle);
        Serial.println("left_mid: 3");
    } else if (pose_code == 4) {
        // Resest 
        muscle1.write(90);
        Serial.println("left_mid: 4");
    }
}
