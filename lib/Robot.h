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
	int maxspeed = 40;
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
	float x, y, ratio, x_range = 0.23;
	bool Ball = ir.GetVector(x, y, ratio, debug);
	if(Ball){
		if(abs(x) < cos(60 * PI / 180) and y > 0){
			if(abs(x) < x_range) motor.Motion( 0, maxspeed);
			else motor.Motion(maxspeed * x * (1 + x_range), maxspeed * y);
		}
		else{
			x = 0.25 * (y / x) * maxspeed; y = -1 * (abs(x) / sqrt(x*x + y*y)) * maxspeed;
			motor.Motion(x, y);
		}
	}
	else{
		motor.Motion(0, 0);
	}
}
#endif