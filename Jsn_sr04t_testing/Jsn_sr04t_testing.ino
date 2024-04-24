// #include <DistanceSensor.h>
#include <HC_SR04.h>

// Define pins
const int echoPin = 12;
const int trigPin = 13;
HC_SR04<echoPin> sensor(trigPin); 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  sensor.begin();
}

void loop() {
  sensor.startMeasure();
  long distance = sensor.getDist_mm();
  Serial.println(distance/10);
  delay(1000);

  // put your main code here, to run repeatedly:
}
