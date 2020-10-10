#ifndef DEBUGMODE_H
#define DEBUGMODE_H

#define ESP Serial1

class DebugMode{
public:
	void init(){Serial.begin(115200)}
private:;

};
#endif