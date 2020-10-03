#ifndef IR_H
#define IR_H

#include <Arduino.h>

class IR{
	
public:
	void init();
	int GetVal(bool offset = true, bool dis = false);
	int GetVal(int *port, bool offset, bool dis);
	void Set_offset(int offset){_offset = offset;}
private:
	byte IR_Pin[10] ={10, 11, 12, 26, 27, 28, 29, 30, 31, 32};
	float Deg[10] = {85, 36, 0, 324, 288, 252, 216, 180, 144, 95};
	float Cos[10], Sin[10];
	int _offset = 90;	
};

#endif