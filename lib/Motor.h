#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

class Motor{

public:
	void init();
	void SetMotor(byte port, int speed);
	void Motion(float x, float y);
	void Stop(unsigned time = 1);
	void Test();
	void Test(int pwr);
	void Test(unsigned time, int pwr);
private:
	byte Motor_D[4][2] = {{14, 2}, {15, 4}, {6, 16}, {8, 17}};
	byte PWM[4] = {3, 5, 7, 9};
	
};

#endif