#include "IR.h"

void IR::init(){
	for(int i = 0; i < 10;i++){
		pinMode(IR_Pin[i], INPUT);
		Cos[i] = cos(Deg[i] * PI / 180);
		Sin[i] = sin(Deg[i] * PI / 180);
	}
	_offset = 90;
}

int IR::GetVal(bool offset, bool dis){
	return GetVal(NULL, offset, dis);
}

int IR::GetVal(int *port, bool offset, bool dis){
	int width[10] = {0}, counter = 0;
	for(uint8_t i = 0; i < 5;i++){
		unsigned long timer = micros();
		do{
			for(uint8_t i = 0; i < 10;i++){
				width[i] += !digitalReadFast(IR_Pin[i]);
				counter += width[i];
				if(port) *(port + i) = width[i];
			}
		}while(micros() - timer <= 833);
	}
	
	int deg = -1;
	if(counter){
		float x = 0, y = 0;
		for(uint8_t i = 0; i < 10;i++){
			x += width[i] * Cos[i];
			y += width[i] * Sin[i];
			deg = int(atan2(y, x) * 180 / PI + 360) % 360;
		}
		if(60 < deg && deg < 140 && offset) deg = deg - (_offset - 90);
	}
	
	if(dis){
		uint8_t sum = 0;
		for (int i = 0;i < 10; i++){
			sum += width[i];
			Serial.printf("%d = %4d", i, width[i]);
			Serial.println();
		}
		Serial.printf("Maxport = %d , sum = %d, mu = %d \n", deg, sum, sum / counter);
		Serial.println("--------------");
		Serial.flush();
	}
	return deg;
}