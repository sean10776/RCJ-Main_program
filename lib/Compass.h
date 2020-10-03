#ifndef COMPASS_H
#define COMPASS_H

#include <Arduino.h>
#include <EEPROM.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

class Compass : public Adafruit_BNO055{

public:
	Compass() : Adafruit_BNO055(55){}
	bool begin():
	int getRVal();
	int getVal(){return (getRVal() - _Forward + 360) % 360;}
	uint16_t getForward(){return _Forward;}
	void setForward(uint16_t Forward){_Forward = Forward;};
	void calibration();
	void DisplayCalStatus();
	void DisplaySensorOffsets(const adafruit_bno055_offsets_t);
private:
	uint16_t _Forward;
	adafruit_bno055_opmode_t _mode;
};

#endif