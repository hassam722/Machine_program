#include <DistanceSensor.h>

// Define pins
const int echoPin = 3;
const int trigPin = 2;

// Start the sensor
DistanceSensor sensor(trigPin, echoPin);

void setup () {
  // Start serial port
  Serial.begin(115200);
}

void loop () {
  // Get distance in cm
  int distance = sensor.getCM();

  // Write values to serial port
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println("cm");

  // Wait a bit
  delay(1000);
}
