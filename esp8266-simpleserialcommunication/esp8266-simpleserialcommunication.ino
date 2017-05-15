/* Piece of code for testing AT commands on ESP8266 from Arduino UNO  */

#include <SoftwareSerial.h>

//ESP8266_TX is connected to Arduino UNO as RX in D3
#define ESP8266_RX_PIN 3

//ESP8266_RX is connected to Arduino UNO as TX in D2
#define ESP8266_TX_PIN 2

//Create software serial object to communicate with ESP8266
SoftwareSerial ESP8266(ESP8266_RX_PIN, ESP8266_TX_PIN);




 
void setup() {
  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  while(!Serial);
   
  //Being serial communication with Arduino and ESP8266
  ESP8266.begin(9600);
  delay(1000);
   
  Serial.println("Setup Complete!");
}
 
void loop() {
  //Read ESP8266 output (if available) and print it in Arduino IDE Serial Monitor
  if(ESP8266.available()){
    Serial.write(ESP8266.read());
  }
  //Read Arduino IDE Serial Monitor inputs (if available) and send them to ESP8266
  if(Serial.available()){
    ESP8266.write(Serial.read());
  }
}

