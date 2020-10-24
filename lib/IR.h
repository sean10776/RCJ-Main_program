#ifndef IR_H
#define IR_H

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include "WProgram.h"
#endif
/*-----------------------------------------------------*/
class IR{
/**********************	
init() : 初始化IR
GetVal(float 外部取值(強度), bool 校正(預設true),bool 顯示(預設false)) : 回傳IR值(0～360)
GetVal(float 外部取值(強度), int 外部取值變數, bool 校正,bool 顯示) : 回傳IR值(0～360)
GetVector(float 外部取值(x軸), float 外部取值(y軸)) : 回傳ture有球，false則無
Set_offset(int 校正值) : 設定校正值
**********************/
public:
	void init();
	int GetVal(bool offset = true, bool dis = false);
	int GetVal(float &str, bool offset = true, bool dis = false);
	int GetVal(float &str, int *port, bool offset, bool dis);
	bool GetVector(float &x, float &y, float &str, bool dis = false);
	void Set_offset(int offset){_offset = offset;}
private:
	byte IR_Pin[10] ={10, 11, 12, 26, 27, 28, 29, 30, 31, 32};
	float Deg[10] = {85, 36, 0, 324, 288, 252, 216, 180, 144, 95};
	float Cos[10], Sin[10];
	int _offset = 90;
};
/*-----------------------------------------------------*/

void IR::init(){
	for(int i = 0; i < 10;i++){
		pinMode(IR_Pin[i], INPUT);
		Cos[i] = cos(Deg[i] * PI / 180);
		Sin[i] = sin(Deg[i] * PI / 180);
	}
	_offset = 90;
}

int IR::GetVal(bool offset, bool dis){
	float str;
	return GetVal(str, NULL, offset, dis);
}

int IR::GetVal(float &str, bool offset, bool dis){
	return GetVal(str, NULL, offset, dis);
}

int IR::GetVal(float &str, int *port, bool offset, bool dis){
	int width[10] = {0}, counter = 0;
	for(uint8_t i = 0; i < 5;i++){
		unsigned long timer = micros();
		do{
			for(uint8_t i = 0; i < 10;i++){
				width[i] += !digitalReadFast(IR_Pin[i]);
				counter += width[i];
				if(port) *(port + i) = width[i];
			}
		}while(micros() - timer <= 833);
	}
	
	int deg = -1;
	if(counter){
		float x = 0, y = 0;
		for(uint8_t i = 0; i < 10;i++){
			x += width[i] * Cos[i];
			y += width[i] * Sin[i];
			deg = int(atan2(y, x) * 180 / PI + 360) % 360;
		}
		str = sqrt(x * x + y * y);
		if(60 < deg && deg < 140 && offset) deg = deg - (_offset - 90);
	}
	
	if(dis){
		uint32_t sum = 0;
		for (int i = 0;i < 10; i++){
			sum += width[i];
			Serial.printf("%d = %4d", i, width[i]);
			Serial.println();
		}
		Serial.printf("Maxport = %d , sum = %d, mu = %d \n", deg, sum, sum / counter);
		Serial.println("--------------");
		Serial.flush();
	}
	return deg;
}

bool IR::GetVector(float &x, float &y,float &ratio, bool dis){
	int width[10] = {0}, sum, counter = 0;
	static float _max;
	for(uint8_t i = 0; i < 5;i++){
		sum = 0, counter = 0;
		unsigned long timer = micros();
		do{
			for(uint8_t i = 0; i < 10;i++){
				width[i] += !digitalReadFast(IR_Pin[i]);
				sum += width[i];
				counter += (width[i] > 0);
			}
		}while(micros() - timer <= 833);
	}
	
	float tx = 0, ty = 0;
	if(counter){
		for(uint8_t i = 0; i < 10;i++){
			tx += width[i] * Cos[i];
			ty += width[i] * Sin[i];
		}
		sum = sqrt((tx * tx) + (ty * ty));
		x = tx / sum; y = ty / sum;
		ratio = sum;
		if(dis) Serial.print(String("x: ") + x + "y: "+ y + "\nratio: " + ratio +"\n");
		return true;
	}
	if(dis)Serial.print(String("No Ball\n"));
	return false;
}
#endif