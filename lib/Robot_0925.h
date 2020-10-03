#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <EEPROM.h>

#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
Adafruit_BNO055 bno = Adafruit_BNO055(55);
Adafruit_BNO055::adafruit_bno055_opmode_t _mode = bno.OPERATION_MODE_IMUPLUS;
int Forward = 0; //Heading

// LED
#define LED 13

//Compass
int Shift = 0;
int Eul_y, Eul_z;//testZ

//Light
#define LineQ 10
byte Light_Pin[LineQ] = {A14, A15, A16, A17, A21, A22, A6, A7, A8, A9};
int LightMax[LineQ] = {0}; //最大值 通常為白值
int LightMin[LineQ] = {9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999}; //最小值 通常為黑值
int LightMid[LineQ] = {512, 503, 404, 504, 526, 423, 466, 460, 601, 599};  //白綠判斷值
int gdb[LineQ] = {0};
int LightPer = 60; //光感百分比
float Ldegree[10] = {270, 234, 198, 162, 126, 90, 54, 18, 342, 306};
float LCos[10], LSin[10];

//IR
byte IR_Pin[10] ={10, 11, 12, 26, 27, 28, 29, 30, 31, 32};
float Pdegree[10] = {85, 36, 0, 324, 288, 252, 216, 180, 144, 95};
float Cos_deg[10], Sin_deg[10];
int IR_offset = 90;

//Motor
byte Motor_D[4][2] = {{14, 2}, {15, 4}, {6, 16}, {8, 17}};
byte PWM[4] = {3, 5, 7, 9};
int maxspeed = 50, degree = 60;

void Error(){
	Serial.print("Error");
	while(1){
		digitalWrite(LED, HIGH);
		delay(100);
		digitalWrite(LED, LOW);
		delay(100);
 }
}
//--------------BNO055---------------//

void displayCalStatus(void)
{
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);

  Serial.print("\t");
  if (!system)Serial.print("! ");
	
  /* Display the individual values */
  Serial.printf("Sys:%d", system);
  Serial.printf(" G:%d", gyro);
  Serial.printf(" A:%d", accel);
  Serial.printf(" M:%d\n", mag);
}

void displaySensorOffsets(const adafruit_bno055_offsets_t &calibData)
{
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

int RawCompass()
{
	sensors_event_t eul;
	bno.getEvent(&eul);
	int Raw = eul.orientation.x;
	return Raw;
}

void BNO_Calibration()
{
	bno.setMode(bno.OPERATION_MODE_CONFIG);
	delay(25);
	bno.setMode(bno.OPERATION_MODE_NDOF);
	delay(20);
  int eeAddress = 0;
  long bnoID;
  bool foundCalib = false;
  sensor_t sensor;
  adafruit_bno055_offsets_t calibrationData;
  EEPROM.get(eeAddress, bnoID);

  bno.getSensor(&sensor);
  if(bnoID != sensor.sensor_id)
  {
    Serial.println("\nNo Calibration Data for this sensor exists in EEPROM");
    delay(500);
  }
  else
  {
    Serial.println("\nFound Calibration for this sensor in EEPROM.");
    eeAddress += sizeof(long);
    EEPROM.get(eeAddress, calibrationData);

    Serial.println("\n\nRestoring Calibration data to the BNO055...");
    bno.setSensorOffsets(calibrationData);

    Serial.println("\n\nCalibration data loaded into BNO055");
    foundCalib = true;
  }
  bno.setExtCrystalUse(true);
  RawCompass();
  if(foundCalib)
  {
		while(!bno.isFullyCalibrated())//當感應器沒有完整校正則持續校正
    {
			RawCompass();
      displayCalStatus();
			delay(100);
    }
  }
	else
  {
    Serial.println("Please Calibrate Sensor: ");
    while(!bno.isFullyCalibrated())//當感應器沒有完整校正則持續校正
    {
      RawCompass();
      displayCalStatus();
			delay(100);
    }
  }
  Serial.println("\nFully calibrated!");
  Serial.println("--------------------------------");
  adafruit_bno055_offsets_t newCalib;
  bno.getSensorOffsets(newCalib);
  displaySensorOffsets(newCalib);

  Serial.println("\n\nStoring calibration data to EEPROM...");

  eeAddress = 0;
  bno.getSensor(&sensor);
  bnoID = sensor.sensor_id;

  EEPROM.put(eeAddress, bnoID);

  eeAddress += sizeof(long);
  EEPROM.put(eeAddress, newCalib);
  Serial.println("Data stored to EEPROM.");
  delay(500);
	bno.setMode(bno.OPERATION_MODE_CONFIG);
	delay(25);
	bno.setMode(_mode);
	delay(20);
}

int CPX(void){
  return (RawCompass() - Forward + 360) % 360;
}

//--------------Motion---------------//

void SetMotor2(byte Port, float Power)//控制單顆馬達(幾號,-100~100)
{
  int16_t sp =  (Power / 100) * 255;
  if(sp > 0){
    digitalWrite(Motor_D[Port - 1][0], HIGH);
    digitalWrite(Motor_D[Port - 1][1], LOW);
  }else if (sp < 0){
    digitalWrite(Motor_D[Port - 1][0], LOW);
    digitalWrite(Motor_D[Port - 1][1], HIGH);
  }else{
    digitalWrite(Motor_D[Port - 1][0], LOW);
    digitalWrite(Motor_D[Port - 1][1], LOW);
  }
  analogWrite(PWM[Port - 1], abs(sp));
}

void MotorStop(int time = 1)//馬達停滯
{
  for (int i = 1; i <= 4;i++)
	{
		SetMotor2(i,0);
	}
  delay(time);
}

void Motion(int px, int py)
{
  int16_t p1, p2, p3, p4, p = 1, ts = 0, com;
  uint8_t range = 120;
  double angle;
  com = (CPX() + Shift + 360) % 360;
	Serial.printf("");
	//com = 0;
  if (com >= (360 - range)) {
    ts = (com - 360);// com - 360 < 0
    p = 1;
  }
  else if (com <= range - 0) {
    ts = (com - 0);// com - 0 > 0
    p = 1;
  }
  else if (com >= 180 && com < (360 - range)) {
    p = 0;
    ts =  (com - 360);// com - 360 < 0
  }
  else if (com < 180 && com > (range - 0)) {
    p = 0;
    ts = (com - 0);// com - 0 > 0
  }
  else{
  }
  //---------------------------------------/
  angle = 3.14 * ((com + 45) % 360) / 180;
  p1 = (int)(-1 * (p * (px * cos(angle) - py * sin(angle))));
  p2 = (int)(p * (py * cos(angle) + px * sin(angle)));
  p3 = p1;
  p4 = p2;
  //---------------------------------------//
  ts = ts * 0.5;
	p1 = p1 + ts;
	p2 = p2 + ts;
	p3 = p3 - ts;
	p4 = p4 - ts;
	SetMotor2(1, p1);
  SetMotor2(2, p2);
  SetMotor2(3, p3);
  SetMotor2(4, p4);
}

void MotorTest()
{
	for(int i = 1;i <= 4; i++)
	{
		unsigned long timer = millis();
		while(millis() - timer < 1000){
			SetMotor2(i, 20);
		}
		MotorStop(0);
	}
	MotorStop(0);
}

//----------------IR-----------------//
int GetIR(bool dis = false, int *t = NULL, bool offset = true)
{
	int port = 0, irwidth[10] = {0}, counter = 0, sum = 0;
	for(int i = 0; i < 5; i++){
		unsigned long int timer = micros();
		do{
			counter = sum = 0;
			for(uint8_t i = 0 ; i < 10 ; i++)
			{
				if(digitalReadFast(IR_Pin[i])==false)
				{
					irwidth[i]++;
				}
				if(irwidth[i] > 0){
					counter ++;
				}
				sum += irwidth[i];
				if(t != NULL) *(t + i) = irwidth[i];
			}
		}while(micros()- timer <= 833);
	}

	float x = 0, y = 0;	
	if(counter == 0){
    port = -1;
  }
	else{
		for(uint8_t i = 0; i < 10; i++){
			x +=  irwidth[i] * Cos_deg[i];
			y +=  irwidth[i] * Sin_deg[i];
		}
		x = x / float(sum);
		y = y / float(sum);
		port = int(atan2(y, x) * 180 / PI + 360)% 360;
	}

	
	if (dis){
		static int bf;
		for (int i = 0;i < 10; i++){
			Serial.printf("%d = %4d", i, irwidth[i]);
			if(irwidth[i] < sum / counter){
				Serial.print(" ** ");
			}
			Serial.println();
		}
		Serial.printf("Maxport = %d , %d , sum = %d, mu = %d \n", port, bf, sum, sum / counter);
		Serial.println("--------------");
		Serial.flush();
		bf = port;
	}
	if(port < 0)
		return -1;
	else{
		if(port < 140 && port > 60 && offset == true)
			return port - (IR_offset - 90);
		else
			return port;
	}
}
//---------------Light---------------//
void Test (bool dis = false)
{
	static bool first = true;
	if(first){
		memset(LightMax, 0, sizeof(LightMax));
		memset(LightMid, 0, sizeof(LightMid));
		memset(LightMin, 9999, sizeof(LightMin));
		first = false;
	}
	int num = LineQ, s[num] = {0};

	for (int i = 0; i < num; i++)
  {
    s[i] = analogRead(Light_Pin[i]);
    LightMax[i] = max(LightMax[i] , s[i]);
		LightMin[i] = min(LightMin[i] , s[i]);
    LightMid[i] = (LightMin[i]*(100-LightPer) + LightMax[i]*LightPer)/100;
    gdb[i]=LightMid[i];
  }

  if(dis)
	{
		Serial.print("     Raw=>");  //原始光感值
		for (int i = 0; i < num; i++){
			Serial.printf("%4d, ", s[i]);
		}
		Serial.print("\nLightMid=>"); //判斷值
		for (int i = 0; i < num; i++){
			Serial.printf("%4d, ", LightMid[i]);
		}
		Serial.print("\nLightMax=>"); //最大值
		for (int i = 0; i < num; i++){
			Serial.printf("%4d, ", LightMax[i]);
		}
		Serial.print("\nLightMin=>"); //最小值
		for (int i = 0; i < num; i++){
			Serial.printf("%4d, ", LightMin[i]);
		}
		Serial.print("\n     gdb=>"); //最小值
		for (int i = 0; i < num; i++){
			Serial.printf("%4d, ", gdb[i]);
		}
		Serial.println("\n-------------------------");
		Serial.flush();
	}
}
//--------------Camera---------------//

//---------------Init----------------//
void Robot_init(void)
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
	digitalWrite(LED, HIGH);
	//------BNO055-------//
  if(!bno.begin(_mode)){
		Error();
  }
	else{
		BNO_Calibration(false);
	}
	
	//-------IR & Light-------//
  for (uint8_t i = 0; i < 10; i++){
		float temp = Pdegree[i] * PI / 180;
    Sin_deg[i] = sin(temp);
    Cos_deg[i] = cos(temp);
		temp = Ldegree[i] * PI / 180;
    LSin[i] = sin(temp);
    LCos[i] = cos(temp);
    pinMode(IR_Pin[i], INPUT);
		pinMode(Light_Pin[i], INPUT);
  }
	//------PWM-------//
  for(int i = 0; i < 4; i++){
    pinMode(PWM[i], OUTPUT);
    pinMode(Motor_D[i][0], OUTPUT);
    pinMode(Motor_D[i][1], OUTPUT);
  }
}
