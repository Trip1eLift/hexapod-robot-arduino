/*
PCA9685...........Uno/Nano
GND...............GND
OE................N/A
SCL...............A5
SDA...............A4
VCC...............5V
*/


/* Include the HCPCA9685 library */
#include "HCPCA9685.h"

/* I2C slave address for the device/module. For the HCMODU0097 the default I2C address
   is 0x40 */
#define  I2CAdd 0x40

/* Create an instance of the library */
//HCPCA9685 HCPCA9685(I2CAdd);

class Hexapod_Muscle {
private:
HCPCA9685 HCPCA9685(I2CAdd);

public:
void setup() {
  HCPCA9685.Init(SERVO_MODE);
  HCPCA9685.Sleep(false);
}

void muscle_move(int leg, int muscle_no, int angle) {
  
}

};

Hexapod_Muscle hex_muscle;


void setup() 
{
  hex_muscle.setup();
}


void loop() 
{
  unsigned int Pos;

  
  for(Pos = 10; Pos < 450; Pos+=10)
  {
    /* This function sets the servos position. It takes two parameters, 
     * the first is the servo to control, and the second is the servo 
     * position. */
    HCPCA9685.Servo(15, Pos);
    HCPCA9685.Servo(0, Pos);
    
    delay(10);
  }
  
  for(Pos = 450; Pos >= 10; Pos-=10)
  {
    HCPCA9685.Servo(15, Pos);
    HCPCA9685.Servo(0, Pos);
    delay(10);
  }
}
