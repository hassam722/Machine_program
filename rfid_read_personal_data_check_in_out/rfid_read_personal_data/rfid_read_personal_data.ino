

#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <Buzzer.h>
#include <SoftwareSerial.h>

SoftwareSerial esp_serial(5,7);///  rx ,tx
String incoming_str = "";
bool flag = true;
const String open_door ="0";
const String check_in ="1";
const String check_out = "2";

// flag_dict = {
//     "Something went wrong":"3",
//     "Already Checked In":"4",
//     "Please Checked In first":"5",
//     "Already Checked Out":"6",
//     "Successfully Checked In":"7",
//     "Successfully Checked Out": "8",
//     "Anonymous":"9",
//     "Door open":"10"
// }

const String _3 = "Offline"; // -----> 3 
// const String _4 = "Already \n Checked In";
// const String _5 = "Please \n Checked In first";
// const String _6 = "Already \n Checked Out";
// const String _7 = "Successfully \n Checked In";
// const String _8 = "Successfully \n Checked Out";
const String _9 = "Anonymous";
const String _10 = "Door Open";


String variable_str = "";
// String nfc = "1037733846898";

String check_in_str="Check In";
String check_out_str="Check Out";

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
    open_the_door();
  }  
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






void uploadstatus(String status){
  int timer = 1;
  while(timer<=3){
    lcd.clear();
    lcd.print("Place your card:");
    lcd.setCursor(0, 1);
    lcd.print(timer);
    timer ++ ;
    delay(800);
  }
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    press_btn_string();
    resetMFRC522();
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    press_btn_string();
    resetMFRC522();
    return;
  }
  nfc_string = dump_byte_array(mfrc522.uid.uidByte,mfrc522.uid.size);
  lcd.clear();
  variable_str = status + "$" +nfc_string;
  Serial.write(variable_str.c_str());
  rec_str = read_serial();
  if (rec_str =="3"){
    lcd.print(_3);
  }else if (rec_str==" 4"){
    lcd.print("Already");
    lcd.setCursor(0, 1);
    lcd.print("Checked In");
  }else if (rec_str==" 5"){
    lcd.print("Already");
    lcd.setCursor(0, 1);
    lcd.print("Checked In First");
  }else if (rec_str==" 6"){
    lcd.print("Already");
    lcd.setCursor(0, 1);
    lcd.print("Checked Out");
  }else if (rec_str==" 7"){ // when successfully check in
    lcd.print("Successfully");
    lcd.setCursor(0, 1);
    lcd.print("Checked In");
    buzzer.begin(0);
    buzzer.sound(NOTE_E4, 500);
    buzzer.end(1000);
  }else if (rec_str==" 8"){ // when successfully check out
    lcd.print("Successfully");
    lcd.setCursor(0, 1);
    lcd.print("Checked Out");
    buzzer.begin(0);
    buzzer.sound(NOTE_E4, 500);
    buzzer.end(1000);
  }else if (rec_str==" 9"){
    lcd.print(_9);
  }
  delay(1000);
  press_btn_string();
  mfrc522.PICC_HaltA();
}




void press_btn_string(){
  lcd.clear();
  lcd.print("Press button");
}




void open_the_door(){
  int timer = 1;
  while(timer<=3){
    lcd.clear();
    lcd.print("Place your card:");
    lcd.setCursor(0, 1);
    lcd.print(timer);
    timer ++ ;
    delay(800);// 1000 increase the timer after 1 second,
  }
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    press_btn_string();
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    press_btn_string();
    mfrc522.PICC_HaltA();
    return;
  }
  nfc_string = dump_byte_array(mfrc522.uid.uidByte,mfrc522.uid.size);
  lcd.clear();
  variable_str = open_door + "$" +nfc_string;
  Serial.write(variable_str.c_str());
  rec_str = read_serial();
  if(rec_str ==" 10"){ // when nfc num authentic
    lcd.print(_10);
    digitalWrite(relay_pin,HIGH);
    delay(100);
    digitalWrite(relay_pin,LOW);
    buzzer.begin(0);
    buzzer.sound(NOTE_E4, 500);
    buzzer.end(1000);
  }else if(rec_str==" 9"){ // when anonyomus person comes
    lcd.print(_9);
  }else if(rec_str =="3"){
    lcd.print(_3);
  }
  delay(1000);
  press_btn_string();
  mfrc522.PICC_HaltA();
}






String read_serial(){
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
