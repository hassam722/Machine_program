

// #include <DistanceSensor.h>
#include <HC_SR04.h>

// Define pins
const int echoPin = 12;
const int trigPin = 13;
const int connx_pin = 5; // setup pending
const long safe_height = 50;// height in mm
long curr_value;
bool flag = true;
String incoming_str;
const int count_value = 2;
int count = count_value;

HC_SR04<echoPin> sensor(trigPin); 

void setup () {
    // Start serial port
    Serial.begin(115200);
    pinMode(connx_pin, OUTPUT); 
    sensor.begin();
}

void loop () {

    sensor.startMeasure();
    long distance = sensor.getDist_mm();
    distance = distance - safe_height;
    // Write values to serial port


    if(distance != curr_value){
      curr_value = distance;
      // Serial.write(String(curr_value).c_str());
      Serial.println(curr_value);
      count = count_value;
    }
    check_connx_status();
    write_data_at_specific_time_period(distance);
    // Wait a bit
    delay(1000);
}

void check_connx_status(){
  if(Serial.available()>0){
    incoming_str = Serial.readString();
    if(incoming_str=="5"){
      digitalWrite(connx_pin, HIGH);
    }else if(incoming_str=="-5"){
      digitalWrite(connx_pin, LOW);
    }
  }
  // Serial.readString();
}

void write_data_at_specific_time_period(long dist){
  if(count <= 0){
    curr_value = dist;
    Serial.write(String(curr_value).c_str());
    count = count_value;
  }else{
    count--;
  }
  
}

// String get_tuple(int num){
//   return String(tankid)+","+String(num);
// }
