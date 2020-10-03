#ifndef LIGHT_H
#define LIGHT_H

#include <Arduino.h>

class Light{

public:
	void init();
	int ReadVal(byte i){return analogRead(Light_Pin[i]);}
	void SetVal(int *port = NULL);
	void GetRVal(int *port);
	bool GetDVal(bool *port, bool dis = false);
	float LCos[10], LSin[10];
private:
	byte Light_Pin[10] = {A14, A15, A16, A17, A21, A22, A6, A7, A8, A9};
	int Light_Max[10], Light_Mid[10], Light_Min[10];
	int Light_Per = 60;
};
#endif