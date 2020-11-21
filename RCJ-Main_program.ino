#include "lib/Robot.h"
Robot Attack;
void setup() {
  Attack.init();
  Serial.println("Start!!");
  //Attack.debugmode.Debug();
}

void loop() {
  Attack.Searching();/*
  if(!Attack.Border() and false){
    Attack.Searching();
  }*/
  Serial.flush();
}
