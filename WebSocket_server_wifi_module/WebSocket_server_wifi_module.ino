#include <ESP8266WiFi.h>
// #include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
// below are the strings that are used to distinguish between each other
// app =0 , tank1 = 1 ,tank2 = 2 , tank3 = 3 , tank4 = 4 , pump = 5

int safety_height = 15;// 15 cm far from sensor
int default_websock =10;// "10" is default that shows websock is not set
String default_id = "9";// "9" is default that shows object is not set

struct tank {
  String Id_name_valve=default_id;
  String level;
  int websock_num =default_websock;
};

struct app {
  String Id_name=default_id;
  int websock_num=default_websock;
};

struct pump{
  String Id_name=default_id;
  String water_status ="0";
  int websock_num=default_websock;
};

tank tank_arry[4];
pump pump_array[1];
app app_array[1];


const char *ssid = "Hassam";
const char *password = "987654321";

IPAddress local_IP(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);


WebSocketsServer webSocket(81);
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      if(app_array[0].websock_num==num){
        app_array[0].websock_num=default_websock;
        app_array[0].Id_name=default_id;
      }else if(pump_array[0].websock_num==num){
        pump_array[0].websock_num=default_websock;
        pump_array[0].water_status="0";
        pump_array[0].Id_name=default_id;
      }else{
        for(int i=0;i<4;i++){
          if(tank_arry[i].websock_num==num){
            tank_arry[i].Id_name_valve=default_id;
            tank_arry[i].websock_num=default_websock;
            break; 
          }
        }
      }
      break;

    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
      }
      break;

    case WStype_TEXT:
      Serial.printf("[%u] Received text: %s\n", num, payload);
      String payloadString = String((char*)payload);
      String to_check = split_string(payloadString);
      if(to_check=="tank"){
        tank_arry[payloadString.toInt() - 1].Id_name_valve =payloadString;
        tank_arry[payloadString.toInt() - 1].websock_num = num;
      }else if(to_check=="pump"){
        // Serial.println(split_string(payloadString));
        pump_array[0].Id_name=payloadString;
        pump_array[0].websock_num=num;
      }else if(to_check=="app"){
        // Serial.println(split_string(payloadString));
        app_array[0].Id_name=payloadString;
        app_array[0].websock_num = num;
      }else if(to_check.toInt()>0 and to_check.toInt() < 5){ // for tank ,updateing water distance
        if(to_check =="1"){
          Serial.println(payloadString);
          tank_arry[to_check.toInt()-1].level=payloadString;
          if(to_check.toInt()<=safety_height){
            please_operate_the_pump("(0,1)");//"(x,y)" x is either 0 or 1 and y is valve no;  
          }
          send_data_to_app();
        }else if(to_check =="2"){
          Serial.println(payloadString);
          tank_arry[to_check.toInt()-1].level=payloadString;
          if(to_check.toInt()<=safety_height){
            please_operate_the_pump("(0,2)");
          }
          send_data_to_app();
        }else if(to_check =="3"){
          Serial.println(payloadString);
          tank_arry[to_check.toInt()-1].level=payloadString;
          if(to_check.toInt()<=safety_height){
            please_operate_the_pump("(0,3)");
          }
          send_data_to_app();
        }else if(to_check =="4"){
          Serial.println(payloadString);
          tank_arry[to_check.toInt()-1].level=payloadString;
          if(to_check.toInt()<=safety_height){
            please_operate_the_pump("(0,4)");
          }
          send_data_to_app();
        }
      }else if(to_check =="0"){// for app, receiving pump command
      // supposing app sends "(x,y)" x is either 0 or 1 and y is valve no; 
        Serial.println(payloadString);
        please_operate_the_pump(payloadString);
      }else if(to_check =="5"){// for pump, receiving water status
        Serial.println(payloadString);
        pump_array[0].water_status= payloadString;
        send_data_to_app();
      }


      for(int i=0;i<4;i++){
        Serial.println(tank_arry[i].Id_name_valve);
      }
      Serial.println(app_array[0].Id_name);
      Serial.println(pump_array[0].Id_name);
      // Send the received text back to the client
      webSocket.sendTXT(num, payload, length);
      break;
  }
}
void send_data_to_app(){
  String temp ="{";
  if(app_array[0].Id_name!=default_id){
    for(int i=0;i<4;i++){
      if(tank_arry[i].Id_name_valve!=default_id){
      temp += (i+1)+":"+tank_arry[0].level;
      }  
    }
    if(pump_array[0].Id_name!=default_id){
      temp += "5:"+pump_array[0].water_status;
    }

    temp += "}";
    webSocket.sendTXT(app_array[0].websock_num,temp);
  }
}

void please_operate_the_pump(String _bit){
  Serial.println("Safety_call");
  if(pump_array[0].Id_name!=default_id){
    webSocket.sendTXT(pump_array[0].websock_num,_bit);
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


void setup()
{
 	Serial.begin(115200);
 	Serial.println();

 	Serial.print("Setting soft-AP configuration ... ");
 	Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

 	Serial.print("Setting soft-AP ... ");
 	Serial.println(WiFi.softAP(ssid,password) ? "Ready" : "Failed!");
 	//WiFi.softAP(ssid);
 	//WiFi.softAP(ssid, password, channel, hidden, max_connection)
 	
 	Serial.print("Soft-AP IP address = ");
 	Serial.println(WiFi.softAPIP());

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
  // webSocket.s
 	delay(500);
}

