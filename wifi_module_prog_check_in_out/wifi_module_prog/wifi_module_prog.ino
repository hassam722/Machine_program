#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

HTTPClient http;    
WiFiClient wifiClient;

const String open_door ="0";
const String check_in ="1";
const String check_out = "2";

String incoming_string  = "";
String delimiter= "$";
String command_string = "";
int del_index;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10);

  // Connect to Wi-Fi
  WiFi.begin("HUAWEI-mPSv", "8d5Nnhh8");

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
}

void loop() {
  if(Serial.available()>0){
    incoming_string =Serial.readString();
    extract_command(incoming_string,delimiter,0);
    if(command_string==open_door){
      extract_command(incoming_string,delimiter,0);
      connect_with_server(command_string,open_door);
    }else if(command_string==check_in){
      extract_command(incoming_string,delimiter,0);
      connect_with_server(command_string,check_in);
    }else if(command_string==check_out){
      extract_command(incoming_string,delimiter,0);
      connect_with_server(command_string,check_out);
    }
  }
}


// this function is used to extract the command from serail communication.
String extract_command(String &incoming_str,String &limiter,int index){
  del_index = incoming_str.indexOf(limiter);
  if(del_index != -1){
    command_string = incoming_str.substring(index,del_index);
    incoming_str = incoming_str.substring(del_index + 1);
    return command_string;
  }
  else{
    command_string = incoming_str;
    return incoming_str;
  }
}




// this function create connection with server, and sends request.
void connect_with_server(String nfc_num,String check_str) {
  if (WiFi.status() == WL_CONNECTED) {
    http.begin(wifiClient,"http://192.168.100.23:8000/rfid-auth");  // this endpoint is of local django server
    http.addHeader("auth-app","itx_solution");
    http.addHeader("nfcnum",nfc_num);
    if(check_str ==open_door){
      http.addHeader("flag",open_door);
    }else if(check_str==check_in){
      http.addHeader("flag",check_in);
    }else if(check_str==check_out){
      http.addHeader("flag",check_out);
    }
    int httpCode = http.GET();
    if (httpCode > 0) {
      String payload = http.getString();
      extract_flag(payload);// this func extract the flag value from payload and assign to payload again
      Serial.flush();
      Serial.write(payload.c_str());
    }else{
      Serial.write("3");//"something went wrong"
    }
    http.end();
  }
}




void extract_flag(String& str){
  int ind = str.indexOf(":");
  str = str.substring(ind+1);
  int ind1 =str.indexOf("}");
  str = str.substring(0,ind1);
}





