// "0" means app connected .
// "1,SSID,PASSWORD"  CHANGE WIFI CONFIGURATION
// "2,PASSWORD"       CHANGE PASSWORD
// "3,user_id"                ADD USER


#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <WebSocketsServer.h>
#include <EEPROM.h>
WebSocketsClient webSocket;
bool client_connx = false;

// String SSID ="HUAWEI-mPSv";
// String PASS ="8d5Nnhh8";

String SSID ="";
String PASS ="";
String HOTSPOT_PASS = "";
String IPADDRESS = "";
String PORT = "";
String incoming_str = "";

int ssid_store = 32;// maximum character
int pass_store = 64;// maximum character
int hotspot_pass_store = 51;//maximum character
int ipaddrss_store = 16;//maximum character
int port_store = 5;//maximum character


// below int variable used to identify the read and write data.
int _ssid =1;
int _pass =2;
int _hotspot_pass =3;
int _ipaddress =4;
int _port =5;

int read = 1;
int write = 0;
String wifi_config = "1";
String change_pass = "2";
String add_user = "3";
String app_connect = "0";
String port_ip = "4";


void erase_data(int start,int end){
  for (int i = start; i < end; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}

String read_write_data(int _which_data,int _signal,String _data_to_write=""){
  int start_point,end_point;

  if(_which_data==_ssid){
  
    start_point =0;
    end_point = ssid_store;
  
  }else if(_which_data==_pass){
  
    start_point = ssid_store;
    end_point = ssid_store + pass_store;
  
  }else if(_which_data==_hotspot_pass){
  
    start_point = ssid_store + pass_store;
    end_point = ssid_store + pass_store + hotspot_pass_store;
  
  }else if(_which_data==_ipaddress){
    // Serial.println("ipaddress");
    start_point = ssid_store + pass_store + hotspot_pass_store;
    end_point = ssid_store + pass_store + hotspot_pass_store+ipaddrss_store;  
  
  }else if(_which_data==_port){
  
    start_point = ssid_store + pass_store + hotspot_pass_store+ipaddrss_store;
    end_point = ssid_store + pass_store + hotspot_pass_store+ipaddrss_store + port_store;
  
  }
  String temp_string ="";
  if(_signal==read){
    // Serial.println("readcondition");
    for(int i =start_point;i<end_point;i++){
      if(char(0)==char(EEPROM.read(i))){
        break;
      }
      temp_string += char(EEPROM.read(i));
      // Serial.println(temp_string);
    }
  }else if(_signal==write){
    if(_data_to_write.length()>0){
      erase_data(start_point,end_point);
      for(int i =0;i<_data_to_write.length();i++){
        // Serial.print(i);
        // Serial.println(_data_to_write[i]);
        EEPROM.write(i+start_point,_data_to_write[i]);
      }
      EEPROM.commit();
    }
  }
  return temp_string;
  
}




// void read_write_ipaddress(int _signal,String _ipaddress=""){
//   int start_point = ssid_store + pass_store + hotspot_pass_store;
//   int end_point = ssid_store + pass_store + hotspot_pass_store+ipaddrss_store;
//   if(_signal==read){
//     for(int i =start_point;i<end_point;i++){
//       if(char(0)==char(EEPROM.read(i))){
//         break;
//       }
//       IPADDRESS += char(EEPROM.read(i));
//       // Serial.println(SSID);
//     }
//   }else if(_signal==write){
//     if(_ipaddress.length()>0){
//       erase_data(start_point,endpoint);
//       for(int i =0;i<_ipaddress.length();i++){
//         // Serial.print(i);
//         // Serial.println(_ssid[i]);
//         EEPROM.write(i+start_point,_ipaddress[i]);
//       }
//       EEPROM.commit();
//     }
//   }

// }

// void read_write_ssid(int _signal,String _ssid=""){
//   if(_signal==read){
//     for(int i =0;i<ssid_store;i++){
//       if(char(0)==char(EEPROM.read(i))){
//         break;
//       }
//       SSID += char(EEPROM.read(i));
//       // Serial.println(SSID);
//     }
//   }else if(_signal==write){
//     if(_ssid.length()>0){
//       erase_data(0,ssid_store);
//       for(int i =0;i<_ssid.length();i++){
//         // Serial.print(i);
//         // Serial.println(_ssid[i]);
//         EEPROM.write(i,_ssid[i]);
//       }
//       EEPROM.commit();
//     }
//   }
// }

// void read_write_hotspot_pass(int _signal,String _pass=""){
//   if(_signal==read){
//     for(int i =ssid_store+pass_store;i<ssid_store+pass_store+hotspot_pass_store;i++){
//       if(char(0)==char(EEPROM.read(i))){
//         break;
//       }
//       HOTSPOT_PASS += char(EEPROM.read(i));
//       // Serial.println(HOTSPOT_PASS);
//     }
//   }else if(_signal==write){
//     if(_pass.length()>0){
//       erase_data(ssid_store+pass_store,ssid_store+pass_store+hotspot_pass_store);
//       for(int i=0;i<_pass.length();i++){
//         // Serial.print(i);
//         // Serial.println(_pass[i]);
//         EEPROM.write(ssid_store+pass_store+i,_pass[i]);
//       }
//       EEPROM.commit();
//     }
//   }
// }

// void read_write_pass(int _signal,String _pass=""){
//   if(_signal==read){
//     for(int i =ssid_store;i<ssid_store+pass_store;i++){
//       if(char(0)==char(EEPROM.read(i))){
//         break;
//       }
//       PASS += char(EEPROM.read(i));
//       // Serial.println(PASS);
//     }
//   }else if(_signal==write){
//     if(_pass.length()>0){
//       erase_data(ssid_store,ssid_store+pass_store);
//       for(int i=0;i<_pass.length();i++){
//         // Serial.print(i);
//         // Serial.println(_pass[i]);
//         EEPROM.write(ssid_store+i,_pass[i]);
//       }
//       EEPROM.commit();
//     }
//   }
// }

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


WebSocketsServer webSocket_server(81);
void webSocketServerEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket_server.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
      }
      break;
    case WStype_TEXT:
      // Send the received text back to the client
      String payloadString = String((char*)payload);
      String to_check = split_string(payloadString);
      if(to_check ==wifi_config){
        SSID = split_string(payloadString);
        PASS = split_string(payloadString);
        read_write_data(_ssid,write,SSID);
        read_write_data(_pass,write,PASS);
        ESP.reset();
      }else if(to_check == change_pass){
        //string not less than 9 or greater than 50;
        HOTSPOT_PASS = split_string(payloadString);
        // read_write_hotspot_pass(write,HOTSPOT_PASS);
        read_write_data(_hotspot_pass,write,HOTSPOT_PASS);
        ESP.reset();
      }else if(to_check == add_user){
        String temp =  "id,"+payloadString;
        Serial.write(temp.c_str());
      }else if(app_connect == to_check){
        String temp = scan_network_give_string();
        webSocket_server.sendTXT(num, temp.c_str(), temp.length());
      }else if(to_check == port_ip){
        split_ip_and_port(payloadString);
        read_write_data(_ipaddress,write,IPADDRESS);
        read_write_data(_port,write,PORT);
        ESP.reset();
        
      }
      break;
  }
}

void split_ip_and_port(String& payload){
  int colon_ind = payload.indexOf(":");
  if(colon_ind != -1){
    IPADDRESS = payload.substring(0,colon_ind);
    PORT = payload.substring(colon_ind+1);
  }
}



void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
	switch(type) {
		case WStype_DISCONNECTED:
			client_connx= false;
			break;
		case WStype_CONNECTED:
      client_connx = true;
			break;
		case WStype_TEXT:
      Serial.readString();
      // Serial.write("{:hassam}");
			Serial.write(payload,length);
			break;
		case WStype_BIN:
			break;
		case WStype_ERROR:			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
	}

}

String scan_network_give_string(){
  int n = WiFi.scanNetworks();
  
  String temp;
  if(n==0){
    temp="null";
  }else if(n > 0) {
    for(int i=0;i<n ;i++){
      if(WiFi.SSID(i)==WiFi.SSID()){
        continue;
      }
      temp += WiFi.SSID(i);
      if(i==n-1){break;}
      temp +=",";
    }
    temp += ":";
    temp+=WiFi.SSID();
  }
  return temp;
}


void setup() {
	// USE_SERIAL.begin(921600);
  EEPROM.begin(512);
  Serial.begin(115200);
  // read_write_hotspot_pass(read);
  // read_write_ssid(read);
  // read_write_pass(read);
	IPADDRESS = read_write_data(_ipaddress,read);
  PORT =  read_write_data(_port,read);
  WiFi.begin(read_write_data(_ssid,read), read_write_data(_pass,read));
  WiFi.scanComplete();
  WiFi.softAP("itx.solution.",read_write_data(_hotspot_pass,read));
  webSocket_server.begin();
  webSocket_server.onEvent(webSocketServerEvent);
	//WiFi.disconnect();
	while(WiFi.status() != WL_CONNECTED) {
    // Serial.print(".");
    webSocket_server.loop();
		delay(100);
	}
  
	// server address, port and URL
	webSocket.begin(IPADDRESS,PORT.toInt(), "machine-side");
	// event handler
	webSocket.onEvent(webSocketEvent);
	// try ever 5000 again if connection has failed
	webSocket.setReconnectInterval(5000);
  // Serial.print("WiFi. IP address: ");
  // Serial.println(WiFi.localIP());
  // Serial.print("Soft AP IP address: ");
  // Serial.println(WiFi.softAPIP());
}

void loop() {
	webSocket.loop();
  webSocket_server.loop();
  if(Serial.available()>0){
    incoming_str = Serial.readString();
    if(client_connx){
      webSocket.sendTXT(incoming_str.c_str());
    }
  }
  delay(100);
}
