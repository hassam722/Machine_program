
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <WebSocketsServer.h>
#include <EEPROM.h>

const int name_len = 10;
const int height_len = 50;
const float On_level = 75;
const float Off_level = 100;

bool connx_status = false;
int provide_connx_status_count=2000;
String ssid = "ITX-Pump";
String password = "987654321";

IPAddress local_IP(192,168,3,22);
IPAddress gateway(192,168,3,9);
IPAddress subnet(255,255,255,0);

String incoming_str;
String valve_no= "1";
String tank_name = "tank";
float tank_height = 180;// height in mm
float percent;
int reset_flag =50000;


// ESP8266WiFi WiFi;
WebSocketsClient webSocket;
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length){
	switch(type) {
		case WStype_DISCONNECTED:
			connx_status = false;
      break;
		case WStype_CONNECTED: {
      connx_status = true;
		}
			break;
		case WStype_TEXT:
			break;
		case WStype_BIN:
			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
        case WStype_PING:
            // pong will be send automatically
            break;
        case WStype_PONG:
            // answer to a ping we send
            break;
    }

}

WebSocketsServer webSocketServer(81);
void webSocketServerEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      break;

    case WStype_CONNECTED:{
      }
      break;

    case WStype_TEXT:
      // Serial.printf("[%u] Received text: %s\n", num, payload);
      String payload_str = String((char*)payload);
      String check_str = split_string(payload_str);
      // tank sends payload = valve_no,tank_name,level
      if(payload_str=="app"){
        String temp_str = tank_name +","+ String((int)tank_height) +","+valve_no;
        webSocketServer.sendTXT(num,temp_str);
      }else{
        tank_name = split_string(payload_str);
        tank_height = payload_str.toFloat();
        // adding name and height in memory
        write_data(0,name_len,tank_name);
        write_data(name_len,height_len,payload_str);
        
        //...
        // giving reset command
        ESP.reset();
        
        //...
      }
      break;
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

String read_data(int start_point,int end_point){
  String temp_string;
    for(int i =start_point;i<end_point;i++){
      if(char(0)==char(EEPROM.read(i))){
        break;
      }
      temp_string += char(EEPROM.read(i));
      // Serial.println(temp_string);
    }
    return temp_string;
}

void write_data(int start_point,int end_point,String data_to_write){
  erase_data(start_point,end_point);
  for(int i =0;i<data_to_write.length();i++){
    EEPROM.write(i+start_point,data_to_write[i]);
  }
  EEPROM.commit();
  
}

void erase_data(int start,int end){
  for (int i = start; i < end; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}

void setup() {
	Serial.begin(115200);
  EEPROM.begin(512);
  WiFi.begin(ssid.c_str(),password.c_str());// connects with ITX-Pump network
  String _name = read_data(0,name_len);
  String _height = read_data(name_len,height_len);
  if(_name!=""){
    tank_name = _name;
  }
  if(_height!=""){
    tank_height = _height.toFloat();
  }
	
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(tank_name,password);// provides our network
  webSocketServer.begin();
	// event handler
	webSocketServer.onEvent(webSocketServerEvent);  
  
  // when wifi is not connected
  while(WiFi.status() != WL_CONNECTED) {
		delay(100);
	}
  // server address, port and URL
	webSocket.begin("192.168.4.22", 81);
	// event handler
	webSocket.onEvent(webSocketEvent);
  	// try ever 5000 again if connection has failed
	webSocket.setReconnectInterval(5000);
  webSocket.enableHeartbeat(15000, 3000, 2);

  // to clear the buffer
  incoming_str = Serial.readString();
}

void loop() {

  // erase_data(0,name_len);
  // erase_data(name_len,height_len);
  // delay(800);
	webSocket.loop();
  webSocketServer.loop();
  if(Serial.available()>0){ 
    incoming_str = Serial.readString();
    float water_height = tank_height - incoming_str.toFloat();  
    percent = ( water_height / tank_height) * 100;// it gives the distance between water and senser in percent
    // percent = 100 - percent;
    incoming_str = valve_no+","+tank_name+","+String(percent);
    if(connx_status){
      webSocket.sendTXT(incoming_str);
    }
    provide_connx_status();
    
  }
  check_reset_requirement();  
  
}

void check_reset_requirement(){
  if(connx_status==false and percent <= On_level){
    if(reset_flag<=0){
      ESP.reset();
    }else{
      reset_flag--;
      // delay(800);
    }
  }
  
}

void provide_connx_status(){
  if(connx_status){
    Serial.write("5");// '5' open the led no 5 which means connected
  }else{
    Serial.write("-5");// '-5' close the led no 5 which means disconnected
  }
  // if(provide_connx_status_count<=0){ 
  // }else{
  //   provide_connx_status_count--;
  // }
  
}
















// String update_string(String w_dist){
//   // Serial.write(w_dist.c_str());
//   String temp_string = "{\"Id\" :"+String(container_id)+",\"Ind_Id\" :"+tank_id+",\"Update\" : true,\"W_dist\" :"+w_dist+"}";
//   return temp_string;
// }

