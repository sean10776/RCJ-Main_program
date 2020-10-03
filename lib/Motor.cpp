#include "Motor.h"

void Motor::init(){
	for(int i = 0; i < 4;i++){
		pinMode(Motor_D[i][0], OUTPUT);
		pinMode(Motor_D[i][1], OUTPUT);
		pinMode(PWM[i], OUTPUT);
	}
}

void Motor::SetMotor(byte port, int speed){
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

void Motor::Motion(float x, float y){
	int16_t p1, p2, p3, p4, p = 1, ts = 0, com;
	uint8_t range = 90;
	com = cpx.getVal();
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
	angle = PI * ((com + 45) % 360 ) / 180;
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
	Test(500, 30);
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
