#include "lib/Robot.h"
Robot Attack;
IMU cpx;
void setup() {
  Attack.init();
  Attack.SetForward();
}

void loop() {
  Serial.flush();
}
