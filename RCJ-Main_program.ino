#include "lib/Robot.h"
Robot Attack;
void setup() {
  Attack.init();
  Attack.debugmode.Debug();
  Attack.SetForward();
  Serial.println("Start!!");
  while(Attack.key() == 'N'){
    Attack.light.SetVal();
    Attack.light.Status();
  }
}

void loop() {
  //Attack.Searching();
  if(!Attack.Border()){
    Attack.Searching();
  }
  Serial.flush();
}
