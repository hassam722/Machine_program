
void setup() {
  Serial.begin(115200); // Set the baud rate
  delay(5000);
  
}

void loop() {
  Serial.print("0$123456789");
  delay(20000);
}

