#ifndef CAMERA_H
#define CAMERA_H

#include <Arduino.h>

class Camera{

public:
	void init();
	void init(int rate);
	bool Uart_read(String &title, int &val);
	int GetVal(float ratio, bool dis = false); 
private:
};

#endif