#ifndef RCJ_SD_H
#define RCJ_SD_H

#include <Arduino.h>
#include <SD.h>

class RCJ_SD{
public:
	void init();
	bool SDwrite(char const *fn, String data);
	bool SDread(char const *fn, String &data);
private:
	File myFile;
	Sd2Card card;
	SdVolume volume;
	SdFile root;
	const int chipSelect = BUILTIN_SDCARD;
};

void RCJ_SD::init(){
  if(!SD.begin(chipSelect)){
    Serial.print("SD Fail!!");
  }
}

bool RCJ_SD::SDwrite(char const *fn, String data){
	if(SD.exists(fn)) Serial.println(String("Remove:") + SD.remove(fn));
  myFile = SD.open(fn, FILE_WRITE);
  myFile.print(data);
  delay(5);
  myFile.close();
	return true;
}

bool RCJ_SD::SDread(char const *fn, String &data){
	if(SD.exists(fn)) myFile = SD.open(fn, FILE_READ);
	else return false;
	while(myFile.position() < myFile.size()){
    static String sbuf = "";
    if(myFile.available()){
      char cbuf = (char)myFile.read();
			data += cbuf;
    }
  }
  myFile.close();
	return true;
}

#endif