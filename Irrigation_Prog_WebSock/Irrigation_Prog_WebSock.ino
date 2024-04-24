/*
 * WebSocketClient.ino
 *
 *  Created on: 24.05.2015
 *
 */



#include <ESP8266WiFi.h>

#include <WebSocketsClient.h>

WebSocketsClient webSocket;




void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

	switch(type) {
		case WStype_DISCONNECTED:
			break;
		case WStype_CONNECTED:
			break;
		case WStype_TEXT:
      Serial.write(String(*payload).c_str());
      // Serial.println();
			break;
		case WStype_BIN:

			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
		case WStype_ERROR:			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
	}

}

void setup() {
	// USE_SERIAL.begin(921600);
	Serial.begin(115200);
  WiFi.begin("Itx-solutions", "987654321");


	//WiFi.disconnect();
	while(WiFi.status() != WL_CONNECTED) {
		delay(100);
	}

	// server address, port and URL
	webSocket.begin("192.168.0.103", 9500, "/wifimod");

	// event handler
	webSocket.onEvent(webSocketEvent);


	// try ever 5000 again if connection has failed
	webSocket.setReconnectInterval(5000);
  Serial.readString();

}

void loop() {
	webSocket.loop();
  if(Serial.available()>0){
    String  temp_str = Serial.readString();
    webSocket.sendTXT(temp_str);
  }
}
