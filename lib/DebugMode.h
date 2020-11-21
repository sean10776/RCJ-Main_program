#ifndef DEBUGMODE_H
#define DEBUGMODE_H

#define ESP Serial1
#include "Value.h"
#include "RCJ_SD.h"
#include "Camera.h"
#include "Light.h"
#include "IR.h"

class DebugMode{
/*****************************
void init(bool 決定是否連上ESP, IR 讀取ir物件, Light 讀取Light物件, Camera 讀取camera物件)：初始化
bool Debug()：調整參數+讀取SD資料
void SendData()：傳資訊給ESP
*****************************/
public:
	void init(bool debug, IR *ir, Light *li, Camera *cam);
	bool Debug();
	void SendData();
private:
	IR *_ir;
	Light *_li;
	Camera *_cam;
	bool _debug = false;
	RCJ_SD sd;
	bool Command(String &title, int &value);
	void datatrans(String data, int *value);
};

void DebugMode::init(bool debug, IR *ir, Light *li, Camera *cam){
	if(debug)ESP.begin(115200);
	Serial.begin(115200);
	sd.init();
	_ir = ir;
	_li = li;
	_cam = cam;
}

bool DebugMode::Command(String &title, int &value){
  static String sbuf = "";
  if(ESP.available()){
    char cbuf = (char)ESP.read();
    if(cbuf != ',' && cbuf != '\n')sbuf += cbuf;
    else{
      title = sbuf.substring(0,sbuf.indexOf('='));
      value = sbuf.substring(sbuf.indexOf('=') + 1).toInt();
      sbuf = "";
      return true;
    }
  }
  return false;
}

void DebugMode::datatrans(String data, int *value){
	String sbuf = "";int count = 0;
	for(uint16_t i = 0; i < data.length();i++){
		char cbuf = data[i];
		if(cbuf != ',' && cbuf != '\n')sbuf += cbuf;
    else{
      *(value + count) = sbuf.substring(sbuf.indexOf('=') + 1).toInt();
      sbuf = "";
			count++;
    }
	}
}

void DebugMode::SendData(){
	String data = "";
	if(_ir)data += (String("IR=") + _ir->GetVal(false, false) + ',');
	if(_cam)data += (String("Cam=") + _cam->GetVal(0.5) + ',');
	if(_li){
		int val[10] = {0};
		_li->SetVal(val);
		for(int i = 0; i < 10; i++){
			data += (String("L") + i + "=" + val[i] + ',');  
		}
	}
  ESP.print(data);
  ESP.flush();
}

bool DebugMode::Debug(){
	String irdata = "", lidata = "", camdata = "", valuedata = "";
	if(_debug){
		while(true){
			SendData();
			String title = "";int val = 0;
			if(Command(title, val)){
				Serial.print(String(title) + " : " + val);
				if(title == "IR"){
					irdata = String(title) + "=" + val + "\n";
					_ir->Set_offset(val);
				}
				if(title == "Camera")camdata = String(title) + "=" + val + "\n";
				if(title == "Pwr"){
					valuedata = String(title) + "=" + val + "\n";
				}
				if(title == "LightPer"){
					valuedata += String(title) + "=" + val + "\n";
				}
			}
			if(title == "Done	"){
				Serial.println("****Saving****");
				int lival[10] = {0};
				_li->SetVal(lival);
				for(int i = 0; i < 10; i++)
					lidata += String("L") + i + "=" + lival[i]+"\n";
				sd.SDwrite("IR.txt", irdata);
				sd.SDwrite("Light.txt",lidata);
				sd.SDwrite("Camera.txt",camdata);
				sd.SDwrite("Value.txt",valuedata);
				delay(5);
				return true;
			}
		}
	}
	else{
		bool ir_offset 		= sd.SDread("IR.txt", irdata);
		bool li_offset 		= sd.SDread("Light.txt",lidata);
		bool cam_offset 	= sd.SDread("Camera.txt",camdata);
		bool value_offset = sd.SDread("Value.txt",valuedata);
		int data[10] = {0};
		if(ir_offset){
			datatrans(irdata, data);
			_ir->Set_offset(data[0]);
		}
		if(li_offset){
			datatrans(lidata, data);
			_li->SetVal(data, true);
		}
		if(cam_offset);
		if(value_offset){
			datatrans(valuedata, data);
			maxspeed = data[0];
			Light_Per = data[1];
		}
	}
	return true;
}
#endif