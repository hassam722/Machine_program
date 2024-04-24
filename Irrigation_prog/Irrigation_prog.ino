// # include <DHT11.h>

// # define DhtPin A0
// int temp;
// DHT11 Dht11(DhtPin);



// void setup() {
//   // put your setup code here, to run once:

//   Serial.begin(115200);
//   // Dht11.setDelay();
// }

// void loop() {
//   // put your main code here, to run repeatedly:

//   temp = Dht11.readTemperature();

//   if (temp != DHT11::ERROR_CHECKSUM && temp != DHT11::ERROR_TIMEOUT) {
//         Serial.print("Temperature: ");
//         Serial.print(temp);
//         Serial.println(" °C");
//     } else {
//         // Print error message based on the error code.
//         Serial.println(DHT11::getErrorString(temp));
//     }

// }






// #include <DFRobot_DHT11.h>
// DFRobot_DHT11 DHT;
// #define DHT11_PIN A0

// void setup(){
//   Serial.begin(115200);
// }

// void loop(){
//   DHT.read(DHT11_PIN);
//   Serial.print("temp:");
//   Serial.print(DHT.temperature);
//   Serial.print("  humi:");
//   Serial.println(DHT.humidity);
//   delay(1000);
// }



// # define SoilSensorPin A0

// // int MoistureValue;



// void setup(){
//   Serial.begin(115200);
// }

// void loop(){
//   Serial.println(GetSoilLevel());
//   delay(1000);
// }






//  main program

#include <DHT11.h>
#include <LiquidCrystal.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

# define FanPin 10
# define LightPin 11
# define PumpPin 12
# define SoilSensorPin A0
# define TempSensorPin 5

JsonDocument doc,recv_doc,Ack_doc;




int SoilLevel=0, Temp=0, LightIntensity=0, Humidity=0, SoilTemp=0, WaterLevel=0;
double WaterPh=0.0;
int MaxTemp=34,MinTemp=30; // temperature in degree celcius
int MaxSoilMoisture=40, MinSoilMoisture=30;
int MaxSoilTemp = 10, MinSoilTemp = 10;
int MaxWaterPh = 10, MinWaterPh = 10;
int MaxLightIntensity = 10, MinLightIntensity = 10;
int MaxHumidity = 10, MinHumidity = 10;
String LightStartTime ="";
String LightStopTime = "";
int PumpStatus = 0, LightStatus = 0, FanStatus =0;

// sequence of storing min and max variables in memory.
// MaxTemp,MinTemp,MaxSoilTemp,MinSoilTemp,MaxSoilMoist,MinSoilMoist,MaxPh,MinPh,
//MaxHumidiy,MinHumidity,MaxLightIntensity,MinLightIntensity,LightStartTime,LightStopTime



int MaxTempLen = 5,MinTempLen = 5,MaxSoilTempLen=5,MinSoilTempLen = 5,MaxSoilMoistLen = 5,MinSoilMoistLen=5,MaxPhLen = 3,MinPhLen =3;
int MaxHumidityLen = 5,MinHumidityLen =5,MaxLightIntensityLen = 10,MinLightIntensityLen = 10,LightScheduleLen = 15;
int LightStartTimeLen = 7 , LightStopTimeLen = 7;


const int rs = 13, en = A1, d4 = A2, d5 = A3, d6 = A4, d7 = A5;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
DHT11 Dht11(TempSensorPin);

void(* resetFunc) (void) = 0;  // declare reset fuction at address 0


void setup(){
  Serial.begin(115200);
  lcd.begin(16, 2);
  erase_data(0, MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen+MaxHumidityLen+MinHumidityLen+MaxLightIntensityLen+MinLightIntensityLen+LightStartTimeLen+LightStopTimeLen+50);
  InitiateLimits();
  pinMode(FanPin,OUTPUT);
  pinMode(LightPin,OUTPUT);
  pinMode(PumpPin,OUTPUT);

}

void loop(){
  // lcd.clear();
  // Temp = Dht11.readTemperature();
  // if (Temp != DHT11::ERROR_CHECKSUM && Temp != DHT11::ERROR_TIMEOUT) {
  // //   TempController(Temp);
  //   ShowTemp(Temp);
  // }

  // SoilLevel = GetSoilLevel();
  // // SoilController(SoilLevel); // making decision on soil moisture level
  // ShowSoilLevel(SoilLevel);
  // ShowPumpStatus();
  // ShowLightStatus();
  // ShowFanStatus();
  // CreateJsonAndSend();
  // CheckPlantUpdate();
  delay(5000);
}

void InitiateLimits(){
  MaxTemp = ReadData(0,MaxTempLen).toInt();
  MinTemp = ReadData(MaxTempLen,MaxTempLen+MinTempLen).toInt();
  MaxSoilTemp = ReadData(MaxTempLen+MinTempLen,MaxTempLen+MinTempLen+MaxSoilTempLen).toInt();
  MinSoilTemp = ReadData(MaxTempLen+MinTempLen+MaxSoilTempLen,MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen).toInt();
  MaxSoilMoisture = ReadData(MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen , MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen).toInt();
  MinSoilMoisture = ReadData(MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen , MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen).toInt();
  MaxWaterPh = ReadData(MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen , MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen).toFloat();
  MinWaterPh = ReadData(MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen , MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen).toFloat();
  MaxHumidity = ReadData(MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen , MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen+MaxHumidityLen).toInt();
  MinHumidity = ReadData(MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen+MaxHumidityLen , MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen+MaxHumidityLen+MinHumidityLen).toInt();
  MaxLightIntensity = ReadData(MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen+MaxHumidityLen+MinHumidityLen , MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen+MaxHumidityLen+MinHumidityLen+MaxLightIntensityLen).toInt(); 
  MinLightIntensity = ReadData(MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen+MaxHumidityLen+MinHumidityLen+MaxLightIntensityLen , MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen+MaxHumidityLen+MinHumidityLen+MaxLightIntensityLen+MinLightIntensityLen ).toInt();
  LightStartTime = ReadData(MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen+MaxHumidityLen+MinHumidityLen+MaxLightIntensityLen+MinLightIntensityLen , MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen+MaxHumidityLen+MinHumidityLen+MaxLightIntensityLen+MinLightIntensityLen+LightStartTimeLen);
  LightStopTime = ReadData(MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen+MaxHumidityLen+MinHumidityLen+MaxLightIntensityLen+MinLightIntensityLen+LightStartTimeLen , MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen+MaxHumidityLen+MinHumidityLen+MaxLightIntensityLen+MinLightIntensityLen+LightStartTimeLen+LightStopTimeLen ); 
}

void erase_data(int start,int end){
  for (int i = start; i < end; i++) {
    EEPROM.write(i, 0);
  }
  // EEPROM.commit();
}

String ReadData(int startpoint,int endpoint){
  String temp_string ="";
  for(int i =startpoint;i<endpoint;i++){
    if(char(0)==char(EEPROM.read(i))){
      break;
    }
    temp_string += char(EEPROM.read(i));
  }
  return temp_string;
}

void WriteData(int startpoint,int endpoint, String datatowrite){
  if(datatowrite.length()>0 and datatowrite.length()<(startpoint-endpoint)){
      erase_data(startpoint,endpoint);
      for(int i =0;i<datatowrite.length();i++){
        // Serial.print(i);
        // Serial.println(_data_to_write[i]);
        EEPROM.write(i+startpoint,datatowrite[i]);
      }
      // EEPROM.commit();
  }
}

void CheckPlantUpdate(){
  
  if(Serial.available()>0){

    String str = Serial.readString();
    deserializeJson(recv_doc, str.c_str());

    WriteData(0,MaxTempLen,recv_doc["max_temp"]);
    WriteData(MaxTempLen,MaxTempLen+MinTempLen,recv_doc["min_temp"]);
    WriteData(MaxTempLen+MinTempLen,MaxTempLen+MinTempLen+MaxSoilTempLen,recv_doc["max_soil_temp"]);
    WriteData(MaxTempLen+MinTempLen+MaxSoilTempLen,MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen,recv_doc["min_soil_temp"]);
    WriteData(MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen , MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen,recv_doc["max_moisture"]);
    WriteData(MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen , MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen,recv_doc["min_moisture"]);
    WriteData(MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen , MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen,recv_doc["max_water_ph"]);
    WriteData(MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen , MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen,recv_doc["min_water_ph"]);
    WriteData(MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen , MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen+MaxHumidityLen,recv_doc["max_humidity"]);
    WriteData(MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen+MaxHumidityLen , MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen+MaxHumidityLen+MinHumidityLen,recv_doc["min_humidity"]);
    WriteData(MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen+MaxHumidityLen+MinHumidityLen , MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen+MaxHumidityLen+MinHumidityLen+MaxLightIntensityLen,recv_doc["max_light_intensity"]); 
    WriteData(MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen+MaxHumidityLen+MinHumidityLen+MaxLightIntensityLen , MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen+MaxHumidityLen+MinHumidityLen+MaxLightIntensityLen+MinLightIntensityLen,recv_doc["min_light_intensity"] );
    WriteData(MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen+MaxHumidityLen+MinHumidityLen+MaxLightIntensityLen+MinLightIntensityLen , MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen+MaxHumidityLen+MinHumidityLen+MaxLightIntensityLen+MinLightIntensityLen+LightStartTimeLen,recv_doc["light_start_time"]);
    WriteData(MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen+MaxHumidityLen+MinHumidityLen+MaxLightIntensityLen+MinLightIntensityLen+LightStartTimeLen , MaxTempLen+MinTempLen+MaxSoilTempLen+MinSoilTempLen+MaxSoilMoistLen+MinSoilMoistLen+MaxPhLen+MinPhLen+MaxHumidityLen+MinHumidityLen+MaxLightIntensityLen+MinLightIntensityLen+LightStartTimeLen+LightStopTimeLen , recv_doc["light_stop_time"]);
    SendAcknowledgement();
    delay(1000);
    resetFunc();

  }

}

void SendAcknowledgement(){
  
  Ack_doc["Action"]=2;
  serializeJson(Ack_doc,Serial);
  // Serial.println();
}


void CreateJsonAndSend(){
  doc["Action"]=1;
  doc["water_ph"]=String(WaterPh);
  doc["light_intensity"]=LightIntensity;
  doc["humidity"]=Humidity; 
  doc["soil_temp"]=SoilTemp;
  doc["moisture"]=SoilLevel; 
  doc["temperature"]=Temp;
  doc["water_level"]=WaterLevel;
  serializeJson(doc,Serial);
  // Serial.println();
}

void IncreamentData(){
  WaterPh+=1.0;
  LightIntensity +=1;
  Humidity +=1;
  SoilTemp +=1;
  SoilLevel +=1;
  Temp +=1;
  WaterLevel +=1;
}


void ShowFanStatus(){
  lcd.setCursor(12,1);
  if(FanStatus){
    String str = "F:On";
    lcd.print(str);
  }else{
    String str = "F:Of";
    lcd.print(str);
  }
}

void ShowLightStatus(){
  lcd.setCursor(6,1);
  if(LightStatus){
    String str = "Lt:On";
    lcd.print(str);
  }else{
    String str = "Lt:Of";
    lcd.print(str);
  }
}

void ShowPumpStatus(){
  lcd.setCursor(0,1);
  if(PumpStatus){
    String str = "Pp:On";
    lcd.print(str);
  }else{
    String str = "Pp:Of";
    lcd.print(str);
  }
}


void ShowTemp(int temp){
  lcd.setCursor(0, 0);
  String str = "T "+ String(temp) +"C";
  lcd.print("      ");
  lcd.setCursor(0, 0);
  lcd.print(str);  
}


void ShowSoilLevel(int soil_level){
  lcd.setCursor(8, 0);
  String str = "ML "+ String(soil_level) +"%";
  lcd.print("       ");
  lcd.setCursor(8, 0);
  lcd.print(str);
}



void TempController(int temp){
  if(temp > MaxTemp){
    FanOn();
  }else if(temp <= (MaxTemp + MinTemp)/2){
    FanOff();
  }
  if(temp < MinTemp){
    LightOn();
  }else if(temp >= (MaxTemp + MinTemp)/2){
    LightOff();
  }
}

void SoilController(int soil_level){
  if(soil_level <= MinSoilMoisture){
    PumpOn();
  }else if(soil_level >= MaxSoilMoisture){
    PumpOff();
  }
}


void FanOn(){
  FanStatus =1;
  digitalWrite(FanPin,HIGH);
}

void FanOff(){
  FanStatus = 0;
  digitalWrite(FanPin,LOW);
}

void LightOn(){
  LightStatus = 1;
  digitalWrite(LightPin,HIGH);
}

void LightOff(){
  LightStatus = 0;
  digitalWrite(LightPin,LOW);
}

void PumpOn(){
  PumpStatus = 1;
  digitalWrite(PumpPin,HIGH);
}

void PumpOff(){
  PumpStatus = 0;
  digitalWrite(PumpPin,LOW);
}


int GetSoilLevel(){
  float Resist = analogRead(SoilSensorPin);
  Resist = (Resist/1023)*100;
  int Moist = 100 - Resist;
  return Moist;
}


























