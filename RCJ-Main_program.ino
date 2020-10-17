#include "lib/IMU.h"
#include "lib/IR.h"
#include "lib/Light.h"
#include "lib/Motor.h"
IMU cpx = IMU();
Light li;
IR ir;
Motor motor;
void setup() {
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  // put your setup code here, to run once:
  motor.init();
  if(!cpx.init()){
    while(1){
      digitalWrite(13, LOW);
      delay(500);
      digitalWrite(13, HIGH);
      delay(500);
    }
  }
  ir.init();
  li.init();
}

void loop() {
  //ir.GetVal(0,1);
  li.SetVal();
  li.Status();
  Serial.flush();
}
