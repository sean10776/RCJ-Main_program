#include "Compass.h"

int Compass::getRVal(){
	return getVector(VECTOR_EULER).x();
}
void Compass::Calibration(){
	setMode(OPERATION_MODE_CONFIG);
	delay(25);
	setMode(OPERATION_MODE_NDOF);
	delay(20);
	int Addr = 0;
	long bnoID;
	bool foundCalib = false;
	sensort_t sensor;
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
void Compass::DisplayCalStatus();
void Compass::DisplaySensorOffsets(const adafruit_bno055_offsets_t CaliData);