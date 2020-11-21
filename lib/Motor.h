#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include "IMU.h"
/*-----------------------------------------------------*/
class Motor{
/*********************************
init() : 初始化馬達
SetMotor(byte 號碼(1～4), float 速度(-100～100)) : 控制單顆馬達轉速
Motion(float x分量, float y分量) : 控制車體移動方向
Stop(unsigned 時間(預設1)) : 停止
╔══Test() : 測試馬達轉向
║  Test(int 測試馬力(-100～100)) : 測試馬達轉向
╚══Test(unsigned 時間, int 測試馬力(-100～100)) : 測試馬達轉向
*********************************/
public:
	void init();
	void SetMotor(byte port, float speed);
	void Motion(float x, float y);
	void Stop(unsigned time = 1);
	void Test();
	void Test(int pwr);
	void Test(unsigned time, int pwr);
private:
	byte Motor_D[4][2] = {{2, 14}, {15, 4}, {16, 6}, {17, 8}};
	byte PWM[4] = {3, 5, 7, 9};
	IMU imu;
};
/*-----------------------------------------------------*/

void Motor::init(){
	for(int i = 0; i < 4;i++){
		pinMode(Motor_D[i][0], OUTPUT);
		pinMode(Motor_D[i][1], OUTPUT);
		pinMode(PWM[i], OUTPUT);
	}
}

void Motor::SetMotor(byte port, float speed){
	int16_t sp =  (speed / 100) * 255;
  if(sp > 0){
    digitalWrite(Motor_D[port - 1][0], HIGH);
    digitalWrite(Motor_D[port - 1][1], LOW);
  }else if (sp < 0){
    digitalWrite(Motor_D[port - 1][0], LOW);
    digitalWrite(Motor_D[port - 1][1], HIGH);
  }else{
    digitalWrite(Motor_D[port - 1][0], LOW);
    digitalWrite(Motor_D[port - 1][1], LOW);
  }
  analogWrite(PWM[port - 1], abs(sp));
}

void Motor::Motion(float px, float py){
	int16_t p1, p2, p3, p4, p = 1, ts = 0, com;
	uint8_t range = 90;
	com = imu.getVal();
	if (com >= (360 - range)){
		ts = (com - 360);
		p = 1;
	}
  else if (com <= range - 0) {
    ts = (com - 0);// com - 0 > 0
    p = 1;
  }
  else if (com >= 180 && com < (360 - range)) {
    p = 0;
    ts =  (com - 360);// com - 360 < 0
  }
  else if (com < 180 && com > (range - 0)) {
    p = 0;
    ts = (com - 0);// com - 0 > 0
  }
	else;
	//------------------------------------//
	float angle = PI * ((com + 45) % 360 ) / 180;
	p1 = p3 = int(-1 * (p * (px * cos(angle) - py * sin(angle))));
	p2 = p4 = int(p * (py * cos(angle) + px * sin(angle)));
	//------------------------------------//
	ts = ts * 0.5;
	SetMotor(1, p1 + ts);
	SetMotor(2, p2 + ts);
	SetMotor(3, p3 - ts);
	SetMotor(4, p4 - ts);
}

void Motor::Stop(unsigned time){
	for(int i = 1; i <= 4;i++)SetMotor(i, 0);
	delay(time);
}

void Motor::Test(){
	Test(500, 20);
}

void Motor::Test(int pwr){
	Test(500, pwr);
}

void Motor::Test(unsigned time, int pwr){
	for(int i = 1; i <= 4;i++){
		SetMotor(i, pwr);
		delay(time);
	}
}
#endif