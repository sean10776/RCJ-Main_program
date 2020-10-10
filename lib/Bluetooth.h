#ifdef BLUETOOTH_H
#define BLUETOOTH_H

#define BT Serial1

class Bluetooth{
public:
	void init(){BT.begin(Serial_rate);}
	void Read_data();
	void Send_data();
private:
	void split_data(String &, int &);
	const uint32_t Serial_rate = 115200;
};

void Bluetooth :: Read_data(){
	
}

void Bluetooth :: Send_data(){
	
}

bool Bluetooth :: split_data(String &title, int &data){
  static String sbuf = "";
  if(BT.available()){
    char cbuf = (char)BT.read();
    if(cbuf != ',' && cbuf != '\n')sbuf += cbuf;
    else{
      title = sbuf.substring(0,sbuf.indexOf('='));
      data = sbuf.substring(sbuf.indexOf('=') + 1).toInt();
      sbuf = "";
      return true;
    }
  }
  return false;
}
#endif