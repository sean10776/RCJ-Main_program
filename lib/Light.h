#ifndef LIGHT_H
#define LIGHT_H

#include <Arduino.h>

/*-----------------------------------------------------*/
class Light{
/****************************
init() : 初始化光感
ReadVal(byte 光感號碼(0～9)) : 回傳光感值
SetVal(int *外部取值變數(預設NULL)):設定光值
Satus():顯示設定的光值
GetRVal(int *外部取值變數):讀取Raw值
GetDVal(bool *外部取值變數,bool 顯示(預設false)) : 讀取過半的True False值
LCos[光感號碼] : 對應cos值
LSin[光感號碼] : 對應sin值
****************************/
public:
	void init();
	int ReadVal(byte i){return analogRead(Light_Pin[i]);}
	void SetVal(int *port = NULL, bool insert = false);
	void Status();
	void SetPer(int per){Light_Per = per;}
	void GetRVal(int *port);
	bool GetDVal(bool *port, bool dis = false);
	float LCos[10], LSin[10];
private:
	byte Light_Pin[10] = {A14, A15, A16, A17, A21, A22, A6, A7, A8, A9};
	int shift = 18;
	uint16_t Light_Max[10], Light_Mid[10], Light_Min[10];
};
/*-----------------------------------------------------*/

void Light::init(){
	for(int i = 0; i < 10;i++){
		Light_Min[i] = 1024;Light_Max[i] = 0;
		pinMode(Light_Pin[i], INPUT);
		float deg = ((int(10 - i) * 36 + shift + 270) % 360) * PI / 180;
		LCos[i] = cos(deg); LSin[i] = sin(deg);
	}
	delay(100);
}

void Light::SetVal(int *port, bool insert){
	if(insert){
		for(int i= 0; i < 10; i++){
			Light_Mid[i] = *(port + i);
		}
	}
	else{
		for(int i = 0; i < 10;i++){
			int val = ReadVal(i);
			Light_Max[i] = max(Light_Max[i], val);
			Light_Min[i] = min(Light_Min[i], val);
			Light_Mid[i] = (Light_Min[i]*(100-Light_Per) + Light_Max[i]*Light_Per)/100;
			if(port)*(port + i) = Light_Mid[i];
		}
	}
}

void Light::Status(){
	Serial.println("Port\t Min\t Mid\t Max\t Raw");
	for(int i = 0; i < 10;i++){
		Serial.printf("%4d\t%4d\t%4d\t%4d\t%4d\n", i, Light_Min[i], Light_Mid[i], Light_Max[i], ReadVal(i));
	}
	Serial.flush();
}

void Light::GetRVal(int *port){
	for(int i = 0;i < 10;i++){
		*(port + i) = ReadVal(i);
	}
}

bool Light::GetDVal(bool *port, bool dis){
	bool counter = 0;
  for (int i = 0; i < 10; i++) { //判斷元件壓在白線上
    if (ReadVal(i) > Light_Mid[i]) {
      if(dis) Serial.print(String(i) + "\t");
      if(port != NULL)*(port + i) = true;
      counter++;
    }
  }
  if(dis)Serial.println();
  if(counter == 0)return false;
  return true;
}
/*
unsigned int Light::GetBVal(bool dis){
	int data = 0;
	for(int i = 0; i < 10; i++) {
		if (ReadVal(i) > Light_Mid[i]) {
      if(dis) Serial.print(String(i) + "\t");
      data = data | (1 << i);
    }
	}
}*/
#endif