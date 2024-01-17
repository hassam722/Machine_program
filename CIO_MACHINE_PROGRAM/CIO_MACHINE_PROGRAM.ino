#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <Buzzer.h>
#include <SoftwareSerial.h>

String incoming_str = "";
bool flag = true;
const String open_door ="3";
const String check_in ="1";
const String check_out = "2";
const String add_user = "4";

String variable_str = "";
// String nfc = "1037733846898";

String _1="DoesNotExist";
String _2="Successfull";
String _3="AlreadyCheckedIn";
String _4="AlreadyCheckedOut";
String _5="CheckInFirst";


String check_in_str="Check In";
String check_out_str="Check Out";
String _success ="Successfull";

String rec_str ="";
String nfc_string = "";

const int CheckInPin = 2;
int CheckIn_btn_state = 0;
const int CheckOutPin =4;
int CheckOut_btn_state = 0;
const int door_btn_pin = 3;
int door_btn_status = 0;
const int relay_pin = 8;

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above
String authentic_person = "2171672145";

// rfid pins MOSI =>11,MISO=>12,SCK=>13

const int rs = A0, en = A1, d4 = A2, d5 = A3, d6 = A4, d7 = A5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
Buzzer buzzer(6);



//*****************************************************************************************//
void setup() {
  Serial.begin(115200);
  pinMode(CheckInPin,INPUT);
  pinMode(CheckOutPin,INPUT);
  pinMode(door_btn_pin,INPUT); 
  pinMode(relay_pin,OUTPUT);
  SPI.begin();                                                  // Init SPI bus
  mfrc522.PCD_Init();// Init MFRC522 card
  lcd.begin(16, 2);                                              
  lcd.print("Press Button");
}



//*****************************************************************************************//
void loop() {
  CheckIn_btn_state = digitalRead(CheckInPin);
  CheckOut_btn_state = digitalRead(CheckOutPin);
  door_btn_status = digitalRead(door_btn_pin);
  if(CheckIn_btn_state){
    incoming_str = Serial.readString();// TO CLEAR THE BUFFER
    uploadstatus(check_in);
  }else if(CheckOut_btn_state){
    incoming_str = Serial.readString();// TO CLEAR THE BUFFER
    uploadstatus(check_out);
  }else if(door_btn_status){
    incoming_str = Serial.readString();// TO CLEAR THE BUFFER
    open_the_door(open_door);
  }else if(Serial.available()>0){
    incoming_str = Serial.readString();
    String temp =split_string(incoming_str);
    if(temp=="id"){
      add_user_nfc(incoming_str);
    }

  }
}

void add_user_nfc(String user_id){
  int timer = 1;
  while( !read_card()){
    lcd.clear();
    lcd.print("Place your card:");
    lcd.setCursor(0, 1);
    lcd.print(timer);
    timer =timer +1 ;
    if(timer>=5){
      press_btn_string();
      return;
    }
    delay(800);
  }
  
  nfc_string = dump_byte_array(mfrc522.uid.uidByte,mfrc522.uid.size);
  variable_str = give_json_string(add_user,nfc_string,user_id);
  // Serial.println(variable_str.c_str());
  Serial.readString();
  Serial.write(variable_str.c_str());
  press_btn_string();
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


void resetMFRC522() {
  mfrc522.PCD_Reset();  // Reset the MFRC522 module
  mfrc522.PCD_Init();   // Reinitialize the MFRC522 module after reset
}




String dump_byte_array(byte *buffer, byte bufferSize) {
  String return_string="";
  for (byte i = 0; i < bufferSize; i++) {
    return_string += String(buffer[i]);
  }
  return return_string;
}


bool read_card(){
  if (mfrc522.PICC_IsNewCardPresent()) {
    if ( ! mfrc522.PICC_ReadCardSerial()) {
      // when card is not read appropriately
    }else{
      return true;
    }
  }  
  return false;
}



void uploadstatus(String status){
  int timer = 1;
  while( !read_card()){
    lcd.clear();
    lcd.print("Place your card:");
    lcd.setCursor(0, 1);
    lcd.print(timer);
    timer =timer +1 ;
    if(timer>=5){
      press_btn_string();
      return;
    }
    delay(800);
  }
  
  nfc_string = dump_byte_array(mfrc522.uid.uidByte,mfrc522.uid.size);
  variable_str = give_json_string(status,nfc_string);
  // Serial.println(variable_str.c_str());
  Serial.readString();
  Serial.write(variable_str.c_str());
  rec_str = read_serial();
  // split_json_string(rec_str);
  // lcd.print(rec_str.c_str());

  if(rec_str=="2"){
    lcd.clear();
    lcd.print(_2.c_str());
    beep();
  }else if(rec_str=="3"){
    lcd.clear();
    lcd.print(_3.c_str());
    delay(1000);
  }else if(rec_str=="4"){
    lcd.clear();
    lcd.print(_4.c_str());
    delay(1000);
  }else if(rec_str=="5"){
    lcd.clear();
    lcd.print(_5.c_str());
    delay(1000);
  }else if(rec_str=="1"){
    lcd.clear();
    lcd.print(_1.c_str());
    delay(1000);
  }
  press_btn_string();
  mfrc522.PICC_HaltA();
}






void beep(){
    buzzer.begin(0);
    buzzer.sound(NOTE_E4, 500);
    buzzer.end(1000);
}




void beep_and_relay(){
  digitalWrite(relay_pin,HIGH);
  beep();
  digitalWrite(relay_pin,LOW);

}



String split_json_string(String& json_str){
  int index1 = json_str.indexOf(":");
  int index2 = json_str.indexOf("}");
  json_str = json_str.substring(index1+1,index2);
  int index3 = json_str.indexOf("'");
  json_str = json_str.substring(index3+1,json_str.length()-1);
}




String give_json_string(const String& status,String& nfc_num,String& id){
  String temp;
  temp = "{\"0\":\""+status+"\",\""+status+"\":"+nfc_num+ ",\"id\":" + id + "}";
  return temp;  
}

String give_json_string(String& status,String& nfc_num){
  String temp;
  temp = "{\"0\":\""+status+"\",\""+status+"\":"+nfc_num+"}";
  return temp;
}



void press_btn_string(){
  lcd.clear();
  lcd.print("Press button");
}






void open_the_door(String status){
  int timer = 1;
  while(!read_card()){
    lcd.clear();
    lcd.print("Place your card:");
    lcd.setCursor(0, 1);
    lcd.print(timer);
    timer =timer +1 ;
    if(timer>=5){
      press_btn_string();
      return;
    }
    delay(800);
  }
  nfc_string = dump_byte_array(mfrc522.uid.uidByte,mfrc522.uid.size);
  variable_str = give_json_string(status,nfc_string);
  Serial.write(variable_str.c_str());
  rec_str = read_serial();
  // split_json_string(rec_str);
  // lcd.print(rec_str.c_str());
  if(rec_str =="2"){ // when nfc num authentic
    lcd.clear();
    lcd.print(_2.c_str());
    beep_and_relay();
  }else{
    lcd.clear();
    lcd.print(_1.c_str());
    delay(1000);
  }
  press_btn_string();
  mfrc522.PICC_HaltA();
}






String read_serial(){
  lcd.clear();
  lcd.print("Please wait...");
  while(true){
      if(Serial.available()>0){ 
      incoming_str = Serial.readString();
      break;
    }
  }
  lcd.clear();
  return incoming_str;
}




//*****************************************************************************************//
