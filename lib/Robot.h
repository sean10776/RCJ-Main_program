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
	void SetForward(){
		for(byte i = 0; i < 3;i++){
			cpx.setForward(cpx.getRVal());
		}
	}
	void Searching();
	
private:
	void Error();
	Motor motor;
	IMU cpx;
	Light light;
	IR ir;
	bool debug = true;
	int maxspeed = 20;
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

void Robot::Searching(){
	float x, y, ratio, x_range = 0.3;
	bool Ball = ir.GetVector(x, y, ratio, debug);
	if(Ball){
		if(abs(x) < x_range * (1 / ratio) and y > 0){
			motor.Motion( 0, maxspeed);
		}
		else{
			x = 0.2 * (y / x) * maxspeed; y = -1 * (abs(x) / sqrt(x*x + y*y)) * maxspeed;
			motor.Motion(x, y);
		}
	}
	else{
		motor.Motion(0, 0);
	}
}
#endif