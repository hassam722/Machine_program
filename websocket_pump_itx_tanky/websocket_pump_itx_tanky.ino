
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <Hash.h>

int OffCondition = 100;
int OnCondition = 75;
String default_no = "-1";
int def_web_no = -1;
String incoming_str;
const int disconnect_count_value =75000;
const int send_data_after = 35000;
int data_flag = send_data_after;

IPAddress local_IP(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);

struct tank{
  String valve;
  String level;
  String name;
  int disconnect_count;
  int websock_num = def_web_no; //def_web_no defines that tank is not connected
};

struct app {
  int websock_num= def_web_no; //def_web_no defines that app is not connected
};

struct Pump_status{
  String OnValve = default_no;// default_no defines that pump is currently off
};

int line_water = 0; // water status in line

Pump_status PumpStatus;

tank tankArray[4];
app APP;



WebSocketsServer webSocket(81);
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      if(APP.websock_num==num){
        APP.websock_num=def_web_no;
      }
      break;

    case WStype_CONNECTED:
      break;

    case WStype_TEXT:
      
      String payload_str = String((char*)payload);
      String check_str = split_string(payload_str);
      // tank sends payload = valve_no,tank_name,level
      if(check_str.toInt()>0 and check_str.toInt() < 5){
        update_tank_data(check_str,payload_str,num);
      }else if(check_str.toInt()== 6){ // 6 indicates app message "6,app"
        if(payload_str=="app"){
          if(APP.websock_num!=def_web_no){
            webSocket.disconnect(num);
          }else{
            APP.websock_num = num;
            // String temp = "tank1,45,1$tank2,55,2$tank3,53,3$tank4,53,4$"+String(line_water)+"#"+PumpStatus.OnValve;
            // webSocket.sendTXT(APP.websock_num,temp.c_str(),temp.length());
          }
          
        }else{ // "6,on_off_cmd,valve_no" i.e. "6,0,1"
          on_of_from_app(payload_str);
        }

      }
      break;
  }
}

void send_data_to_app(){
  if(APP.websock_num!= def_web_no){
    if(data_flag <=0){
      String temp_str = create_string();
    webSocket.sendTXT(APP.websock_num,temp_str.c_str(),temp_str.length());
    data_flag =send_data_after;
    }else{
      data_flag--;
    }
    
  }
}

String create_string(){
  String main_str = "";

  for(int i=0;i<4;i++){
    if(tankArray[i].websock_num!=def_web_no){
      main_str += tank_data_in_string(i) ;
    }
  }
  main_str += String(line_water)+"#"+PumpStatus.OnValve;
  
  return main_str;

}

String tank_data_in_string(int index){
  String temp;
  temp = tankArray[index].name+","+tankArray[index].level+","+tankArray[index].valve+"$";
  return temp;
}



void show_tanks(){
  for(int i=0;i<4;i++){
    Serial.print(tankArray[i].valve);
    Serial.print(tankArray[i].name);
    Serial.print(tankArray[i].level);
    Serial.println();
    
  }
}

void update_tank_data(String valve,String payload_str,int web_no){
  tankArray[valve.toInt()-1].valve=valve;
  tankArray[valve.toInt()-1].name = split_string(payload_str);
  tankArray[valve.toInt()-1].level = payload_str;
  tankArray[valve.toInt()-1].websock_num = web_no;
  tankArray[valve.toInt()-1].disconnect_count = disconnect_count_value;
  // automate_pump(valve);
}

void automate_pump(String valve_no){
  if(tankArray[valve_no.toInt()-1].level.toInt()<=75){
    if(PumpStatus.OnValve==default_no or PumpStatus.OnValve==valve_no){
      PumpStatus.OnValve = valve_no;
      String  temp =valve_no+",1"; 
      Serial.write(temp.c_str());// on  command to pump
    }
  }else if(tankArray[valve_no.toInt()-1].level.toInt()>=100){
    String  temp = valve_no+",0";
    if(valve_no==PumpStatus.OnValve){ // no other tank can off the valve of another tank
      Serial.write(temp.c_str());// off command to pump
      PumpStatus.OnValve = default_no;
    } 
  }
}

String split_string(String &payload){
  //  e.g "tank,1"
  int comma_index = payload.indexOf(",");
  if(comma_index != -1){
    String temp = payload.substring(0,comma_index);
    payload =payload.substring(comma_index+1);
    return temp;
  }
  return payload;
}

void setup() {
	Serial.begin(115200);
  WiFi.softAPConfig(local_IP, gateway, subnet);
	WiFi.softAP("ITX-Pump","987654321");
		
	// server address, port and URL
	webSocket.begin();
	// event handler
	webSocket.onEvent(webSocketEvent);
  Serial.write("0,0");//exception case: to stop the pump after reseting of wifimodule
  // to clear the buffer
  // incoming_str = Serial.readString();
}

void loop() {
	webSocket.loop();
  is_tanks_connected();
  pump_operator();
  send_data_to_app();
  // delay(1000);
}

void is_tanks_connected(){
  for(int i=0;i<4;i++){
    if(tankArray[i].websock_num != def_web_no){
      if(tankArray[i].disconnect_count<=0) {
        webSocket.disconnect(tankArray[i].websock_num);
        tankArray[i].websock_num=def_web_no;
      }else{
        tankArray[i].disconnect_count--;
      }
    }
  }
}

void pump_operator(){
  if(PumpStatus.OnValve==default_no){
    for(int i= 0;i<4;i++){
      if(tankArray[i].level.toInt()<=OnCondition and tankArray[i].websock_num != def_web_no){
        please_on_the_pump(tankArray[i].valve);
        PumpStatus.OnValve = tankArray[i].valve;
        return;
      }
    }

  }else{
    
    if(tankArray[PumpStatus.OnValve.toInt()-1].level.toInt()>=OffCondition){
      please_off_the_pump();
      PumpStatus.OnValve = default_no;
      return;
    }
  
    if(tankArray[PumpStatus.OnValve.toInt()-1].websock_num==def_web_no){ // if pump is on than it checks, is tank connected for which pump is "ON"
       // if pump is 'on" on the same valve 
        please_off_the_pump();
        PumpStatus.OnValve = default_no;
        return;
      
    }
  }
}

void on_of_from_app(String text){
  String check = split_string(text);
  if(check=="1"){
    if(PumpStatus.OnValve==default_no){
      please_on_the_pump(text);
      PumpStatus.OnValve = text;
    }
  }else if(check=="0"){
    if(PumpStatus.OnValve!=default_no){
      please_off_the_pump();
      PumpStatus.OnValve=default_no;
    }
  }
}

void please_off_the_pump(){
  String  temp = "0,0";
  Serial.write(temp.c_str());
  while(true){
    for(int i = 0;i<4;i++){
      delay(1000);
      if(Serial.available()>0){
      String temp_var = Serial.readString();
        if(temp_var=="0"){
          return;
        }
      }
    }
    Serial.readString();
    Serial.write(temp.c_str());
  }
}

void please_on_the_pump(String valve){
  String  temp = valve+",1";
  Serial.write(temp.c_str());
  while(true){
    for(int i = 0;i<4;i++){
      delay(1000);
      if(Serial.available()>0){
      String temp_var = Serial.readString();
        if(temp_var=="1"){
          return;
        }
      }
    }
    Serial.readString();
    Serial.write(temp.c_str());
  }
}