#ifndef ROBOT_H
#define ROBOT_H

#include <Arduino.h>

#include "Value.h"//預設參數
#include "Motor.h"
#include "IMU.h"
#include "Light.h"
#include "IR.h"
#include "DebugMode.h"
#define LED 13

class Robot{
/****************************
void init()：初始化
void SetForward()：設定當前車頭方向為Goal
void Searching()：追球
bool Border()：出界判斷，出界回傳flase，否則true
bool Kicker(uint16_t 通電時間, uint16_t 斷電時間)：踢球 回傳當前是否踢過
char key()：讀取Serial input
****************************/
public:
	void init();
	void SetForward(){
		for(byte i = 0; i < 3;i++){
			cpx.setForward(cpx.getRVal());
		}
	}
	void Searching();
	bool Border();
	char key();
	bool Kicker(uint16_t, uint16_t);
	/************/
	Motor motor;
	IMU cpx;
	Light light;
	IR ir;
	DebugMode debugmode;
private:
	byte Kick_Pin = A19;
	void Error();
	/*******************/
	float A_Ldegree[10] = {90, 54, 18, 342, 306, 270, 234, 198, 162, 126};
	float A_LCos[10], A_LSin[10];
};

void Robot::init(){
  Serial.begin(115200);
  pinMode(LED, OUTPUT);			 digitalWrite(LED, HIGH);
	pinMode(Kick_Pin, OUTPUT); digitalWrite(Kick_Pin, LOW);
	debugmode.init(debug, &ir, &light, NULL);
	motor.init();
	if(!cpx.init()){
		if(Motor_debug)Error();
	}
	light.init();
	light.SetVal(Mid, true);
	ir.init();
	/***********************/
	for (int i = 0; i < 10; i++) {
    float temp = (int(A_Ldegree[i] - 18 + 360) % 360) * PI / 180;
    A_LCos[i] = cos(temp);
    A_LSin[i] = sin(temp);
  }
	SetForward();
}

char Robot::key(){
	if(Serial.available()){
		return char(Serial.read());
	}
	return 'N';
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
	float x, y, ratio, y_range = 0.77/*, x_range = 0.2, catch_deg = 45*/;
	bool Ball = ir.GetVector(x, y, ratio, debug);
	static unsigned long timer = millis();
	static bool Kicked = false;
	if(Ball){
		if((abs(x) < 0.15 and y >= 0.97 and millis() - timer > 300) or Kicked){
			Kicked = Kicker(100, 5000);
		}
		if( y > 0){
			if(y < y_range) motor.Motion(x * abs(3 - y) * maxspeed, -1 * y * maxspeed);
			else motor.Motion( x * maxspeed, y * maxspeed);
		}
		else{
			x = 0.3 * (y / x) * maxspeed; y = -1 * abs(x) / sqrt(x*x + y*y) * maxspeed;
			motor.Motion(x, y);
		}
	}
	else{
		motor.Motion(0, 0);
	}
}

bool Robot::Border(){
	const bool dis = debug;
	static bool Reset = false, Half = false, Pass_port[10] = {false};
	static float Pass_deg = -1;
	float x = 0, y = 0;
	int Pass_num = 0;
	
	bool Touch, Port[10];
	memset(Port, false, sizeof(Port));
	Touch = light.GetDVal(Port, dis);
	for(int i = 0; i < 10; i++){
		if(Port[i]){
			if(!Half)Pass_port[i] = true;
			x += light.LCos[i]; y += light.LSin[i];
		}
		if(Pass_port[i])Pass_num++;
	}
	
	if(abs(x) < 0.0001)x = 0;
	if(abs(y) < 0.0001)y = 0;
	
	if ((!Half && !Touch && Reset) || key() != 'N') {
    Reset = Half = false;
    Pass_deg = -1;
    memset(Pass_port, false, sizeof(Pass_port));
  }
	
	float current_deg = -1;
	if(x != 0 or y != 0){
		current_deg = (int(atan2(y, x) / PI * 180) + 360) % 360;
	}
	else if(Pass_deg >= 0){
		current_deg = Pass_deg;
	}
	if (Pass_deg < 0){
		Pass_deg = current_deg;
	}
	
	//過半判斷
  int con = -1, temp_deg[2] = {-1, -1};
  if (current_deg >= 0 && Touch) {
    con = 10 - (((int(current_deg) / 18 + 15) % 21) / 2);
    temp_deg[0] = abs(A_Ldegree[con] - current_deg);
    temp_deg[1] = abs(A_Ldegree[(con - 1 + 10) % 10] - current_deg);
    temp_deg[0] = min(temp_deg[0], 360 - temp_deg[0]);
    temp_deg[1] = min(temp_deg[1], 360 - temp_deg[1]);
    if (temp_deg[0] > temp_deg[1]) con = (con - 1 + 10) % 10;
  }
	
	if ( Pass_port[con] && con >= 0) {
    Half = true;
  }
  else if(!Pass_port[con] && con >= 0){
    Reset = true;
		Half = false;
	}
	else{
		if(Half) Reset = true;
		Half = false;
	}
	
  if (x == 0 and y == 0 && Touch) {
    Half = true;
  }

  int Dif_deg = abs(Pass_deg - current_deg);
  Dif_deg = min(Dif_deg, 360 - Dif_deg);
  if (Dif_deg <= 36 && current_deg >= 0 && !Half) Pass_deg = current_deg;
  else if(Half)current_deg = Pass_deg;

  if(dis){
    for(int i  = 0; i < 10;i++)Serial.printf("%d: %d  ", i, Pass_port[i]);
    Serial.printf("\nHalf: %d Reset: %d Touch: %d", Half, Reset, Touch);
    Serial.printf("\nX:%1.2f Y:%1.2f\tCon: %d", x, y, con);
    Serial.printf("\nCD: %d, PD: %d\n", (int)current_deg, (int)Pass_deg);
  }

  //回場
  if(Half || Reset){
    float mx = 0, my = 0;
    mx = maxspeed * cos(current_deg * PI / 180);
    my = maxspeed * sin(current_deg * PI / 180);
    motor.Motion(mx, my);
    return true;
  }
  return false;
}

bool Robot::Kicker(uint16_t charge,uint16_t discharge){
	static unsigned long timer = millis();
	static bool kicked = false;
	if(millis() - timer > discharge){
		if(!kicked){
			digitalWrite(Kick_Pin, HIGH);
			kicked = true;
		}
		timer = millis();
	}
	if(kicked && millis() - timer > charge){
		digitalWrite(Kick_Pin, LOW);
		kicked = false;
		timer = millis();
	}
	return kicked;
}
#endif