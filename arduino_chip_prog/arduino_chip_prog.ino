#include <SoftwareSerial.h>

SoftwareSerial esp_serial(5,8);///  rx ,tx
String incoming_str = "";
bool flag = true;
const String open_door ="0";
const String check_in ="1";
const String check_out = "2";
String variable_str = "";
String nfc = "1037733846898";
int while_value = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  esp_serial.begin(115200);
  delay(10000);
  Serial.println("Starting");

}

void loop() {
  // put your main code here, to run repeatedly:

  while(while_value<3){
    if(while_value==0){
      Serial.println("data sent");
      variable_str = open_door + "$" +nfc;
      esp_serial.write(variable_str.c_str());
      read_serial();
    }else if(while_value==1){
      Serial.println("data sent");
      variable_str = check_in + "$" + nfc;
      esp_serial.write(variable_str.c_str());
      read_serial();
    }else if(while_value==2){
      Serial.println("data sent");
      variable_str = check_out + "$" + nfc;
      esp_serial.write(variable_str.c_str());
      read_serial();
    }
    while_value++;
  }
  
  
  
}

String read_serial(){
  Serial.println("read_serial_begin");
  while(true){
      if(esp_serial.available()){
      incoming_str = esp_serial.readString();
      Serial.println("Recieved String: "+incoming_str);
      break;
    }
  }
  Serial.println("read_serial_end");
  return incoming_str;
}


