#ifndef ROBOT_H
#define ROBOT_H

#include <Arduino.h>

#include "Motor.h"
#include "IMU.h"
#include "Light.h"
#include "IR.h"
#define LED 13

class Robot{
public:
	void init();
	void SetForward(){cpx.setForward(cpx.getRVal());}
	void Searching();
	
private:
	void Error();
	Motor motor;
	IMU cpx;
	Light light;
	IR ir;
};

void Robot::init(){
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
	motor.init();
	if(!cpx.init()){
		Error();
	}
	light.init();
	ir.init();
}

void Robot::Error(){
	while(1){
		digitalWrite(LED, LOW);
		delay(300);
		digitalWrite(LED, HIGH);
		delay(300);
	}
}
#endif