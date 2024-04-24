#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// Replace with your network credentials
const char *ssid = "HUAWEI-mPSv";
const char *password = "8d5Nnhh8";

// Replace with your Google API key
const char *googleApiKey = "your_API_KEY";

// Google API endpoint
const char *googleEndpoint = "/"; // You can modify the endpoint based on your needs

// Google server details
const char *googleHost = "www.google.com";
const int googlePort = 443;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;

  Serial.print("Connecting to ");
  Serial.println(googleHost);

  if (!client.connect(googleHost, googlePort)) {
    Serial.println("Connection failed!");
    return;
  }

  // Send a request to www.google.com
  client.print(String("GET ") + googleEndpoint + " HTTP/1.1\r\n" +
               "Host: " + googleHost + "\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("Request sent");

  // Wait for the response
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }

  // Read and print the response
  while (client.available()) {
    String line = client.readStringUntil('\n');
    Serial.println(line);
  }

  Serial.println("Request complete");

  // Close the connection
  client.stop();
}

void loop() {
  // Nothing to do here
  Serial.println("hassam");
}
