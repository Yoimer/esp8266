#include <ESP8266WiFi.h>

//const char* ssid     = "WiwoNET";
//const char* password = "xxxxxxx";

const char* ssid     = "Casa";
const char* password = "remioy2006202";

//const char* host = "https://pure-caverns-1350.herokuapp.com";
//const char* host = "pure-caverns-1350.herokuapp.com";
////const char* host="dev.teubi.co"; Salvadorian fellas
const char* host = "castillolk.com.ve";

String line = "";
String tmp = "";
String jj = "";

byte OldCounter = -1;
byte NewCounter = -1;
byte j = -1;
byte f = -1;
byte r = -1;

void setup() {
  Serial.begin(9600);
  delay(10);

  // We start by connecting to a WiFi network

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
//    Serial.print(".");
  }
}

void loop() {
  delay(5000);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
//    Serial.println("connection failed");
    return;
  }

  // We now create a URI for the request
  //String url = "/stan";
  //String url = "/hola.php";  Salvadorian fellas
  String url = "/WhiteList.txt";

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(90);
  
  while (client.available()) {
    line = client.readStringUntil('\r');
    Serial.print(line);
  }
}

