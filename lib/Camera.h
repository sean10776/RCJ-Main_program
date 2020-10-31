#ifndef CAMERA_H
#define CAMERA_H

#include <Arduino.h>
/*-----------------------------------------------------*/
class Camera{

public:
	void init();
	void init(int rate);
	int GetVal(float ratio, bool dis = false); 
private:
	bool Uart_read(String &title, int &val);
};

/*-----------------------------------------------------*/
#define Cam Serial6

void Camera::init(int rate){
	Cam.begin(rate);
}

void Camera::init(){
	Cam.begin(115200);
}

bool Camera::Uart_read(String &title, int &val){
	static String sbuf = "";
  if(Cam.available()){
    char cbuf = (char)Cam.read();
    if(cbuf != ',' && cbuf != '\n')sbuf += cbuf;
    else{
      title = sbuf.substring(0,sbuf.indexOf('='));
      val = sbuf.substring(sbuf.indexOf('=') + 1).toInt();
      sbuf = "";
      return true;
    }
  }
  return false;
}

int Camera::GetVal(float ratio, bool dis){
	static int data[8] = {0}, Shift = 100;
	String title = ""; int val = 0; bool change = false;
	change = Uart_read(title, val);
	if(change){
		if(title[0] == 'f') data[title[1] - '0'] = val;
		if(title[0] == 'b') data[title[1] - '0' + 3] = val;
	}
	if(data[2] == 0 && change){//No_Front = False
		int deg = (int(atan2(-1 * (data[1] - 120),data[0] - 160) / PI * 180) + 360) % 360;
		if(dis)Serial.printf("C_deg : %d, delta: %d, Shift : %d\n",deg, 90 - deg, Shift);
		if(abs(90 - deg) > (60 / ratio)){
			if(90 - deg < 0)Shift = -60;
			else Shift = 60;
		}
		else if(90 - deg != 0)Shift = int(90 - deg) * ratio;
		else{
			Shift = 0;
		}
	}else if(data[2] == 1){
		if(dis)Serial.println("No Goal");
		Shift = 100;
	}
	return Shift;
}
#endif