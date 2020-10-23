#include "lib/Robot.h"
Robot Attack;
void setup() {
  Attack.init();
  Attack.SetForward();
  Serial.println("Start!!");
  delay(1000);
}

void loop() {
  Attack.Searching();
  Serial.flush();
}
