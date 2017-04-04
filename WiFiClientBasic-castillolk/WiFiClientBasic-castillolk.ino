#include <ESP8266WiFi.h>

//const char* ssid     = "WiwoNET";
//const char* password = "xxxxxxx";

const char* ssid     = "Casa";
const char* password = "remioy2006202";

//const char* host = "https://pure-caverns-1350.herokuapp.com";
//const char* host = "pure-caverns-1350.herokuapp.com";
const char* host = "castillolk.com.ve";

String line = "";
String tmp = "";
byte OldCounter = -1;
byte NewCounter = -1;

void setup() {
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

int value = 0;

void loop() {
  delay(5000);
  ++value;

  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // We now create a URI for the request
  //String url = "/stan";
  String url = "/WhiteList.txt";

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(90);

  // Read all the lines of the reply from server and print them to Serial
  Serial.println("Respond:");
  while (client.available()) {
    line = client.readStringUntil('\r');
    Serial.print(line);
  }

  tmp = line.substring(3, 5); //Gets oldnumber
  Serial.println("tmp: ");
  Serial.println(tmp);
  OldCounter = tmp.toInt();
  Serial.print("OldCounter: ");
  Serial.println(OldCounter);
  
  tmp = line.substring(6, 8);  // Gets new number
  Serial.println("tmp: ");
  Serial.println(tmp);
  NewCounter = tmp.toInt();
  Serial.println("tmp: ");
  Serial.print("NewCounter: ");
  Serial.println(NewCounter);


  // Works OK
  //  Serial.println("firstPoundSign: ");
  //  int firstPoundSign = line.indexOf(',');
  //  Serial.println(firstPoundSign);
  //
  //  Serial.println("secondPoundSign: ");
  //  int secondPoundSign = line.indexOf(',', firstPoundSign + 1);
  //  Serial.println(secondPoundSign);

  Serial.println();
  Serial.println("closing connection");

}

