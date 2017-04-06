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

void loop() {
  delay(5000);


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
  //String url = "/hola.php";  Salvadorian fellas
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
  LoadWhiteList();

  Serial.println();
  Serial.println("closing connection");

}


//////////////////////////////////////
void ClearWhiteList()
{
  Serial.println("On ClearWhiteList");
  String jj = "";
  j = 1 ;         // lleva la cuenta de los nros a borrar
  while (j <= OldCounter)
  {
    jj = j;
    tmp = "AT+CPBW=" + jj + "\r\n";
    Serial.println(tmp);       // comando AT a ejecutar ??
    j = j + 1;
  }
}
/////////////////////////////////////////////////////
void LoadWhiteList()
{
  Serial.println("On LoadWhiteList");
  jj = "";
  tmp = line.substring(3, 5); //Gets oldnumber
  OldCounter = tmp.toInt();
  Serial.print("OldCounter: ");
  Serial.println(OldCounter);
  
  tmp = line.substring(6, 8);  // Gets new number
  NewCounter = tmp.toInt();
  Serial.print("NewCounter: ");
  Serial.println(NewCounter);
  
  ClearWhiteList();
  
  f = 9;         // aqui comienzan los nros de telefono
  j = 1;         // lleva la cuenta de los nros a cargar
  while (j <= NewCounter)
  {
    r  = f + 11;             //  nros son de largo 11 ejm 04265860622
    ////tmp  = BuildString.substring( f , r );
    tmp  = line.substring(f, r);
    jj   = j;
    tmp  = "AT+CPBW=" + jj + ",\"" + tmp + "\",129,\"" + jj + "\"\r\n";
    //  if(0 != gprs.sendCmdAndWaitForResp(tmp.c_str(), "OK", TIMEOUT))
    //    {
    //    ERROR("ERROR:CMGF");
    //    return;
    //   }
    Serial.println(tmp);       // comando AT a ejecutar
    f = f + 12;            //  12 para saltar la coma
    j = j + 1;
  }
}
