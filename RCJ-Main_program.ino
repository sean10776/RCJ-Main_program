#include "lib/Robot.h"
Robot Attack;
void setup() {
  Attack.init();
  Serial.println("Start!!");
  Serial.begin(115200);
  //Attack.debugmode.Debug();
  Attack.SetForward();
  pinMode(A19, OUTPUT);
}

void loop() {
  Attack.Searching();/*
  if(!Attack.Border() and false){
    Attack.Searching();
  }*/
  Serial.flush();
}
