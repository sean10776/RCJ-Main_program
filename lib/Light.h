#ifndef LIGHT_H
#define LIGHT_H

#include <Arduino.h>
/*-----------------------------------------------------*/
class Light{

public:
	void init();
	int ReadVal(byte i){return analogRead(Light_Pin[i]);}
	void SetVal(int *port = NULL);
	void Status();
	void GetRVal(int *port);
	bool GetDVal(bool *port, bool dis = false);
	float LCos[10], LSin[10];
private:
	byte Light_Pin[10] = {A14, A15, A16, A17, A21, A22, A6, A7, A8, A9};
	int Light_Max[10], Light_Mid[10], Light_Min[10];
	int Light_Per = 60;
};
/*-----------------------------------------------------*/

void Light::init(){
	for(int i = 0; i < 10;i++){
		Light_Min[i] = 1024;Light_Max[i] = 0;
		pinMode(Light_Pin[i], INPUT);
		float deg = ((int(10 - i) * 36 + 270) % 360) * PI / 180;
		LCos[i] = cos(deg); LSin[i] = sin(deg);
	}
}

void Light::SetVal(int *port){
	for(int i = 0; i < 10;i++){
		int val = ReadVal(i);
		Light_Max[i] = max(Light_Max[i], val);
		Light_Min[i] = min(Light_Min[i], val);
		Light_Mid[i] = (Light_Min[i]*(100-Light_Per) + Light_Max[i]*Light_Per)/100;
		if(port)*(port + i) = Light_Mid[i];
	}
}

void Light::Status(){
	Serial.println("Port\t Min\t Mid\t Max\t Raw");
	for(int i = 0; i < 10;i++){
		Serial.printf("%4d\t%4d\t%4d\t%4d\t%4d\n", i, Light_Min[i], Light_Mid[i], Light_Max[i], ReadVal(i));
	}
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
#endif