#include "lib/IMU.h"
#include "lib/IR.h"
#include "lib/Light.h"
IMU cpx = IMU();
Light li;
IR ir;
void setup() {
  // put your setup code here, to run once:
  if(!cpx.begin()){
    cpx.Calibration();
  }
  cpx.getRVal();
  ir.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  cpx.getRVal();
  ir.GetVal();
  li.init();
}
