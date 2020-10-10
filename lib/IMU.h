#ifndef IMU_H
#define IMU_H

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include "WProgram.h"
#endif
#include <EEPROM.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

/*-----------------------------------------------------*/
class IMU : public Adafruit_BNO055{

public:
	IMU() : Adafruit_BNO055(55){}
	bool begin(adafruit_bno055_opmode_t mode = OPERATION_MODE_IMUPLUS){
		_mode = mode; 
		return begin(mode);
	}
	int getRVal();
	int getVal(){return (getRVal() - _Forward + 360) % 360;}
	uint16_t getForward(){return _Forward;}
	void setForward(uint16_t Forward){_Forward = Forward;}
	void Calibration(void);
	void DisplayCalStatus();
	void DisplaySensorOffsets(const adafruit_bno055_offsets_t);
private:
	uint16_t _Forward;
	adafruit_bno055_opmode_t _mode;
};
/*-----------------------------------------------------*/

int IMU::getRVal(){
	return getVector(VECTOR_EULER).x();
}
void IMU::Calibration(){
	setMode(OPERATION_MODE_CONFIG);
	delay(25);
	setMode(OPERATION_MODE_NDOF);
	delay(20);
	int Addr = 0;
	long bnoID;
	bool foundCalib = false;
	sensor_t sensor;
	adafruit_bno055_offsets_t CaliData;
	EEPROM.get(Addr, bnoID);
	
	getSensor(&sensor);
	if(bnoID != sensor.sensor_id){
    Serial.println("\nNo Calibration Data for this sensor exists in EEPROM");
    delay(500);
	}
  else
  {
    Serial.println("\nFound Calibration for this sensor in EEPROM.");
    Addr += sizeof(bnoID);
    EEPROM.get(Addr, CaliData);

    Serial.println("\n\nRestoring Calibration data to the BNO055...");
    setSensorOffsets(CaliData);

    Serial.println("\n\nCalibration data loaded into BNO055");
    foundCalib = true;
  }
	setExtCrystalUse(true);
	
	sensors_event_t event;
	getEvent(&event);
	
	if(foundCalib){
		Serial.println("Move sensor slightly to calibrate magnetometers");
		while(!isFullyCalibrated()){
			getEvent(&event);
			delay(100);
		}
	}
	else{
		Serial.println("Please CaliData Sensor: ");
		while(!isFullyCalibrated()){
			getEvent(&event);
			DisplayCalStatus();
			delay(100);
		}
	}
  Serial.println("\nFully calibrated!");
  Serial.println("--------------------------------");
  adafruit_bno055_offsets_t newCalib;
  getSensorOffsets(newCalib);
  DisplaySensorOffsets(newCalib);
	
	Serial.println("\n\nStoring calibration data to EEPROM...");
	
	Addr = 0;
	getSensor(&sensor);
	bnoID = sensor.sensor_id;
	
	EEPROM.put(Addr, bnoID);
	
	Addr += sizeof(bnoID);
	EEPROM.put(Addr, newCalib);
	Serial.println("Data stored to EEPROM.");
	delay(500);
	setMode(OPERATION_MODE_CONFIG);
	delay(25);
	setMode(_mode);
}
void IMU::DisplayCalStatus(){
	uint8_t sys, gyro, accel, mag;
	sys = gyro = accel = mag = 0;
	getCalibration(&sys, &gyro, &accel, &mag);
	
  Serial.print("\t");
  if (!sys)Serial.print("! ");
	
  /* Display the individual values */
  Serial.printf("Sys:%d", sys);
  Serial.printf(" G:%d", gyro);
  Serial.printf(" A:%d", accel);
  Serial.printf(" M:%d\n", mag);
}
void IMU::DisplaySensorOffsets(const adafruit_bno055_offsets_t calibData){
	Serial.print("Accelerometer: ");
	Serial.print(calibData.accel_offset_x); Serial.print(" ");
	Serial.print(calibData.accel_offset_y); Serial.print(" ");
	Serial.print(calibData.accel_offset_z); Serial.print(" ");

	Serial.print("\nGyro: ");
	Serial.print(calibData.gyro_offset_x); Serial.print(" ");
	Serial.print(calibData.gyro_offset_y); Serial.print(" ");
	Serial.print(calibData.gyro_offset_z); Serial.print(" ");

	Serial.print("\nMag: ");
	Serial.print(calibData.mag_offset_x); Serial.print(" ");
	Serial.print(calibData.mag_offset_y); Serial.print(" ");
	Serial.print(calibData.mag_offset_z); Serial.print(" ");

	Serial.print("\nAccel Radius: ");
	Serial.print(calibData.accel_radius);

	Serial.print("\nMag Radius: ");
	Serial.print(calibData.mag_radius);
}
#endif