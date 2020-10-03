#include "Light.h"

void Light::init(){
	memset(Light_Max, 0, sizeof(Light_Max));
	memset(Light_Min, 0, sizeof(Light_Min));
	for(int i = 0; i < 10;i++){
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
		*(port + i) = Light_Mid[i] = (Light_Min[i]*(100-Light_Per) + Light_Max[i]*Light_Per)/100;
		
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