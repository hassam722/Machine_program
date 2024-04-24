
//  communication structure "(1,1)" => (valve,pump_status) => e.g (1,1)

// valve pin set up
int valve1= 9;
int valve2 = 10;
int valve3 = 13;
int valve4 = 12;
int pump_pin = 11 ;
int connx_led_pin = 5; 


String income_str ;

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  pinMode(valve1,OUTPUT);
  pinMode(valve2,OUTPUT);
  pinMode(valve3,OUTPUT);
  pinMode(valve4,OUTPUT);
  pinMode(pump_pin, OUTPUT);// pump pin
  pinMode(connx_led_pin,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  // for(int i=1;i<5;i++){
  //   please_on_the_valve(i);
  //   please_operate_the_pump(1);
  //   delay(1000);
  //   please_operate_the_pump(0);
  //   please_off_the_valve();
  // }

  if(Serial.available()>0){
    income_str = Serial.readString();
    // Serial.print(income_str.c_str());
    String valve = split_string(income_str);
    String status = split_string(income_str);
    if(status =="1"){
      if(valve=="1"){
        please_on_the_valve(1);
        please_operate_the_pump(1);
      }else if(valve =="2"){
        please_on_the_valve(2);
        please_operate_the_pump(1);
      }else if(valve =="3"){
        please_on_the_valve(3);
        please_operate_the_pump(1);
      }else if(valve =="4"){
        please_on_the_valve(4);
        please_operate_the_pump(1);
      }
    }else if(status =="0"){
      please_operate_the_pump(0);
      please_off_the_valve();
    }
  }
  delay(1000);
  
}

void please_on_the_valve(int valve_no){
  if(valve_no==1){
    digitalWrite(valve1, HIGH);
  }else if(valve_no==2){
    digitalWrite(valve2, HIGH);
  }else if(valve_no==3){
    digitalWrite(valve3, HIGH);
  }else if(valve_no==4){
    digitalWrite(valve4, HIGH);
  }

}

void please_operate_the_pump(int req){
  if(req==1){
    digitalWrite(pump_pin, HIGH);
    // Serial.readString();
    Serial.write("1");
  }else if(req==0){
    digitalWrite(pump_pin, LOW);
    // Serial.readString();
    Serial.write("0");
  }
}

void please_off_the_valve(){
    digitalWrite(valve1, LOW);
    digitalWrite(valve2, LOW);
    digitalWrite(valve3, LOW);
    digitalWrite(valve4, LOW);

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

