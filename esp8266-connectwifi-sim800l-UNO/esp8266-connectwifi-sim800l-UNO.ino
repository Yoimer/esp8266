/*
  WiFiEsp example: WebClientRepeating

  This sketch connects to a web server and makes an HTTP request
  using an Arduino ESP8266 module.
  It repeats the HTTP call each 10 seconds.

  For more details see: http://yaab-arduino.blogspot.com/p/wifiesp.html
*/
#include "WiFiEsp.h"

// Emulate ESP8266 on pins 2/3 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"


//The connection in ESP8266 should be like these:
//
//ESP8266_TX->RX(D3)
//ESP8266_RX->TX(D2)
//ESP8266_CH_PD->3.3V
//ESP8266_VCC->3.3V
//ESP8266_GND->GND

//ESP8266_TX is connected to Arduino UNO as RX in D3
#define ESP8266_RX_PIN 3

//ESP8266_RX is connected to Arduino UNO as TX in D2
#define ESP8266_TX_PIN 2

//Create software serial object to communicate with ESP8266
SoftwareSerial ESP8266(ESP8266_RX_PIN, ESP8266_TX_PIN);

//SIM800 TX is connected to Arduino D8
#define SIM800_TX_PIN 8

//SIM800 RX is connected to Arduino D7
#define SIM800_RX_PIN 7

//Create software serial object to communicate with SIM800
SoftwareSerial serialSIM800(SIM800_TX_PIN, SIM800_RX_PIN);

#define TIMEOUT 20000

#define onModulePin 13

////SoftwareSerial Serial1(6, 7); // RX, TX
#endif

int8_t answer;                          //where sendATcommand response is saved
char *ssid                              = "Casa";// your network SSID (name)
char *pass                              = "remioy2006202";// your network password
int status                              = WL_IDLE_STATUS;// the Wifi radio's status
unsigned long lastConnectionTime        = 0;// last time you connected to the server, in milliseconds
const unsigned long postingInterval     = 10000L;// delay between updates, in milliseconds
int sensorValue                         = 0;//read the input on analog pin 0:
float voltage                           = 0;// convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
String voltageAsString                  = "";// value read from readLDR()
char *server                            = "estredoyaqueclub.com.ve";//server to pusblish data
char *url                               = "/arduinoenviacorreo.php?telefono=";//path in server
String Password                         = "";// where password will be saved
String lastLine                         = "";// where last line of SMS or call will be saved
char currentLine[500]                   = "";//variable to hold last line of serial output from SIM800
bool nextValidLineIsCall                = false;//boolean to be set to true when a valid call
String phonenum                         = "";//where incoming number will be saved, might be SMS or call
String PhoneCalling                     = "";//variable to manipulate call number
String PhoneCallingIndex                = "";//variable to manipulate call number index
String DateandHour                      = "";//variable to save date and hour when receiving a SMS
bool nextLineIsMessage                  = false;//boolean to be set to true when a SMS
int firstComma                          = -1;//initialize first comma index
int secondComma                         = -1;//initialize second comma index
int thirdComma                          = -1;//initialize third comma index
int forthComma                          = -1;//initialize forth comma index
int fifthComma                          = -1;//initialize fifth comma index
int firstQuote                          = -1;//initialize first quote index
int secondQuote                         = -1;//initialize second comma index
int firstColon                          = -1;//initialize first colon index
int secondColon                         = -1;//initialize second colon index
int thirdColon                          = -1;//initialize third colon index
int swveces                             =  0;//initialize swveces index
int len                                 = -1;//initialize len index
int j                                   = -1;//initialize j index
int i                                   = -1;//initialize i index
int f                                   = -1;//initialize f index
int r                                   =  0;//initialize r index
bool isIncontact                        = false;//boolean to be set to true when a number is on phonebook
bool isAuthorized                       = false;//boolean to be set to true when a number is authorized
int currentLineIndex                    = 0;//initialize index when reading buffer
int x;
String OldPhoneCalling                  = "";//where previous phone will be saved
unsigned long xprevious;//previous measured value


// Initialize the Ethernet client object
WiFiEspClient client;

void setup()
{
  // initialize serial for debugging
  Serial.begin(115200);
  //initialize serial for SIM800L module
  serialSIM800.begin(9600);
  // initialize serial for ESP8266 module
  ESP8266.begin(9600);

  //initialize ESP module
  WiFi.init(&ESP8266);

  //check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) 
     {
		Serial.println("WiFi shield not present");
		// don't continue
		while (true);
	 }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) 
		{
			Serial.print("Attempting to connect to WPA SSID: ");
			Serial.println(ssid);
			// Connect to WPA/WPA2 network
			status = WiFi.begin(ssid, pass);
		}

  Serial.println("You're connected to the network");

  printWifiStatus();
}

void loop()
{

  Sim800Module();
  delay(5000);
  ESP8266Module();

}

//////////////////////////////////////////////////////////////////////////////////////////////////////

// this method makes a HTTP connection to the server
void httpRequest()
{
  Serial.println();

  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection
  if (client.connect(server, 80)) 
	 {
		Serial.println("Connecting...");

		// cleans voltageAsString
		voltageAsString = "";
		// converts float voltage to String
		voltageAsString = String(readLDR());

		// send the HTTP PUT request
		client.print(String("GET ") + url + voltageAsString + " HTTP/1.1\r\n" +
					 "Host: " + server + "\r\n" +
					 "Connection: close" + "\r\n");

		//    client.println(F("GET /WhiteList.txt HTTP/1.1"));
		//    client.println(F("Host: castillolk.com.ve"));
		//    client.println("Connection: close");
		client.println();

		// note the time that the connection was made
		lastConnectionTime = millis();
     }
	 else 
	 {
		// if you couldn't make a connection
		Serial.println("Connection failed");
     }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
float readLDR()
{
  //read the input on analog pin 0:
  sensorValue = analogRead(A0);
  //Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  voltage = sensorValue * (5.0 / 1023.0);
  // print out the value you read:
  Serial.println(voltage);
  return voltage;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void power_on() 
{

  uint8_t answer = 0;

  Serial.println("On Power_on...");

  // checks if the module is started
  answer = sendATcommand("AT\r\n", "OK\r\n", TIMEOUT, 0);
  if (answer == 0)
	 {
		// power on pulse
		digitalWrite(onModulePin, HIGH);
		delay(3000);
		digitalWrite(onModulePin, LOW);

    // waits for an answer from the module
		while (answer == 0) 
			  {
				  // Send AT every two seconds and wait for the answer
				  answer = sendATcommand("AT\r\n", "OK\r\n", TIMEOUT, 0);
				  Serial.println("Trying connection with module...");
			  }
     }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
int8_t sendATcommand(const char* ATcommand, const char* expected_answer1,unsigned int timeout, int xpassword) 
{

  uint8_t x = 0,  answer = 0;
  char response[100];
  unsigned long previous;

  memset(response, '\0', 100);    // Initialize the string

  delay(100);

  //while ( Serial.available() > 0) Serial.read();   // Clean the input buffer

  //  while(Serial.available()) { //Cleans the input buffer
  //        Serial.read();
  //    }

  while (serialSIM800.available())
		{ 	//Cleans the input buffer
			serialSIM800.read();
		}

  Serial.println(ATcommand);    // Prints the AT command
  serialSIM800.write(ATcommand); // Sends the AT command
  x = 0;
  previous = millis();

  // this loop waits for the answer
  do 
  {
    ////if (Serial.available() != 0) {
    if (serialSIM800.available() != 0)
	   {
		  ////response[x] = Serial.read();
		  response[x] = serialSIM800.read();
		  x++;
		  // check if the desired answer is in the response of the module
		  if (strstr(response, expected_answer1) != NULL)
			 {
				answer = 1;
				String numbFromSim = String(response);
				numbFromSim = numbFromSim.substring(numbFromSim.indexOf(":"),
                                            numbFromSim.indexOf(",129,"));
				numbFromSim = numbFromSim.substring((numbFromSim.indexOf(34) + 1),
                                            numbFromSim.indexOf(34, numbFromSim.indexOf(34) + 1));
				if ( xpassword == 1)
				   {
						numbFromSim = numbFromSim.substring( 0, 4);
						Password = numbFromSim ;
						return 0;
				   }
				else
				   {
						numbFromSim = numbFromSim.substring( 0, 11 );
				   }
             }
        }

  }
        
  // Waits for the asnwer with time out
  while ((answer == 0) && ((millis() - previous) < timeout));

  return answer;
}
///////////////////////////////////////////////////////////////////////////////
void Sim800Module()
{
  serialSIM800.listen();
  power_on();
  Serial.println("Connecting to the network...");
  while ( (sendATcommand("AT+CREG?\r\n", "+CREG: 0,1\r\n", TIMEOUT, 0) ||
           sendATcommand("AT+CREG?\r\n", "+CREG: 0,5\r\n", TIMEOUT, 0)) == 0 );
  sendATcommand("AT+CMGF=1\r\n", "OK\r\n", TIMEOUT, 0);
  sendATcommand("AT+CNMI=1,2,0,0,0\r\n", "OK\r\n", TIMEOUT, 0);
  sendATcommand("AT+CPBR=1,1\r\n", "OK\r\n", TIMEOUT, 1);
  Serial.println("Password:");
  Serial.println(Password);
  sendSMS("04168262667", "System is online");
  
  if (serialSIM800.available() > 0)
     {
		char lastCharRead = Serial.read();
		if (lastCharRead == '\r' || lastCharRead == '\n')
		   {
				endOfLineReached();
		   }
		else
		   {
				currentLine[currentLineIndex++] = lastCharRead;
		   }
     }
  serialSIM800.end();
}
/////////////////////////////////////////////////////////////////////////////////
void ESP8266Module()
{
  ESP8266.listen();

  // Try twice HTTP Request
  
  /*The while loop might be also controlled by time, like
  if 10 seconds have passed since your last connection,
  then connect again and send data
  if (millis() - lastConnectionTime > postingInterval)
  {
    httpRequest();
  }*/

  int counter = 0;
  while (counter < 2) 
		{
			httpRequest();

			// if there's incoming data from the net connection send it out the serial port
			// this is for debugging purposes only
			while (client.available())
				  {
					char c = client.read();
					Serial.write(c);
				  }

			counter = counter + 1;

        }

  ESP8266.end();
}
///////////////////////////////////////////////////////
int sendSMS(char *phone_number, char *sms_text)
{
  char aux_string[30];
  uint8_t answer = 0;
  Serial.print("Setting SMS mode...");
  sendATcommand("AT+CMGF=1\r\n", "OK\r\n", TIMEOUT, 0);   // sets the SMS mode to text
  Serial.println("Sending SMS");
  sprintf(aux_string, "AT+CMGS=\"%s\"\r\n", phone_number);
  answer = sendATcommand(aux_string, ">", TIMEOUT, 0);   // send the SMS number
  if (answer == 1)
     {
		Serial.println(sms_text);
		serialSIM800.write(sms_text);
		delay(1000);
		serialSIM800.write(0x1A);
		delay(500);
		answer = sendATcommand("", "OK\r\n", TIMEOUT, 0);
     if (answer == 1)
        {
			Serial.println("Sent ");
        }
     else
	    {
			Serial.println("error ");
        }
     }
  else
     {
		Serial.println("error ");
		Serial.println(answer, DEC);
     }
  return answer;
}
///////////////////////////////////////////////////////
void endOfLineReached()
{
	lastLine = String(currentLine);

  //-----------------------Call---------------------------//
  // If lastLine reads RING, New Call Indication was received.
  // Hence, the THIRD LINE is the caller information.

  // The whole string will be something like this if caller IS registered on SIM Card:
  //RING (First Line)
  //     (Second Line is empty)
  //+CLIP: "04168262667",129,"",0,"Yoimer",0 (Third Line)

  // The whole string will be something like this if caller is NOT registered on SIM Card
  //RING (First Line)
  //     (Second Line is empty)
  //+CLIP: "04168262667",129,"",0,"",0 (Third Line)

  //----------------------SMS-------------------------------------//
  //If lastLine reads +CMT, New SMS Message Indications was received.
  //Hence, next line is the message content.

  if (lastLine.startsWith("RING"))//New incoming call
     {
		Serial.println(lastLine);
		nextValidLineIsCall = true;
     }
  else
     {
		if ((lastLine.length() > 0) && (nextValidLineIsCall))//Rejects any empty line
		   {
				LastLineIsCLIP();
		   }
		   else if (lastLine.startsWith("+CMT:"))//New incoming SMS
		           {
						Serial.println(lastLine);
						phonenum = lastLine.substring((lastLine.indexOf(34) + 1),lastLine.indexOf(34, lastLine.indexOf(34) + 1));
						nextLineIsMessage = true;
						firstComma        = lastLine.indexOf(',');
						secondComma       = lastLine.indexOf(',', firstComma  + 1);
						thirdComma        = lastLine.indexOf(',', secondComma + 1);
						forthComma        = lastLine.indexOf(',', thirdComma  + 1);
						fifthComma        = lastLine.indexOf(',', forthComma  + 1);
						PhoneCalling      = lastLine.substring((firstComma - 12), (firstComma - 1));
						PhoneCallingIndex = lastLine.substring((firstComma + 2), (secondComma - 1));
						firstColon        = lastLine.indexOf(':');
						secondColon       = lastLine.indexOf(':', firstColon + 1);
						thirdColon        = lastLine.indexOf(':', secondColon + 1);
						DateandHour       = lastLine.substring((secondComma + 2), (thirdColon));
						Serial.println(DateandHour);
						Serial.println(PhoneCalling);        ////////////////////////////////////////////
						Serial.println(PhoneCallingIndex);
						j                 = PhoneCallingIndex.toInt();
						isIncontact       = false;
						isAuthorized      = false;
						if (j > 0)
					       {
								isIncontact = true;
								Serial.println("en phonebook"); //////////////////////////////////////////////
								if (j <= 5 )
								   {
										Serial.println("autorizada"); //////////////////////////////////////////////
										isAuthorized = true;
								   }
					       }
						else
					       {
					       }
		            }
		else if ((lastLine.length() > 0) && (nextLineIsMessage))
		        {
					LastLineIsCMT();
		        }
  }
	CleanCurrentLine();
}
/////////////////////////////////////////////////////////////////////
void CleanCurrentLine()
{
  for ( int i = 0; i < sizeof(currentLine);  ++i )
	  {
		currentLine[i] = (char)0;
      }
  currentLineIndex = 0;
}
/////////////////////////////////////////////////////////////////////
void LastLineIsCMT()
{
  Serial.println(lastLine);
  //clearBuffer();
  if (isIncontact)
	 {
		if (lastLine.indexOf("LED ON") >= 0)
           {
				prendeapaga(0);
           }
		else if (lastLine.indexOf("LED OFF") >= 0)
                {
					prendeapaga(1);
                }
        else if (lastLine.indexOf("ADD") >= 0)
		        {
					DelAdd(1);
		        }
        else if (lastLine.indexOf("DEL") >= 0)
                {
					DelAdd(2);
                }
	    else if (lastLine.indexOf("GETA0") >= 0)
			    {
					//GetValueofAnalog0();
			    }
        else
           {
					//clearBuffer();
           }
	 }
  CleanCurrentLine();
  nextLineIsMessage = false;

}
////////////////////////////////////////////////////////
void LastLineIsCLIP()

{
  firstComma         = lastLine.indexOf(',');
  secondComma        = lastLine.indexOf(',', firstComma + 1);
  thirdComma         = lastLine.indexOf(',', secondComma + 1);
  forthComma         = lastLine.indexOf(',', thirdComma + 1);
  fifthComma         = lastLine.indexOf(',', forthComma + 1);
  PhoneCalling       = lastLine.substring((firstComma - 12), (firstComma - 1));
  PhoneCallingIndex  = lastLine.substring((forthComma + 2), (fifthComma - 1));
  j                  = PhoneCallingIndex.toInt();
  if (PhoneCalling == OldPhoneCalling)
     {
		 swveces = 1;
		if ((millis() - xprevious ) > 9000)
		   {
				swveces   = 0;
				xprevious = millis();
		   };
     }
  else
     {
		xprevious       = millis();
		OldPhoneCalling = PhoneCalling;
		swveces         = 0;
     }  
  if (j > 0 & swveces == 0)
     {
		digitalWrite(LED_BUILTIN, HIGH);
     }
  // if ((WiFiMulti.run() == WL_CONNECTED) & swveces == 0)
  // {
    // HTTPClient http;
    // String xp = "http://estredoyaqueclub.com.ve/arduinoenviacorreo.php?telefono=" + PhoneCalling + "-" + PhoneCallingIndex;
    // http.begin(xp);
    // int httpCode = http.GET();
    // if (httpCode > 0)
    // {
      // if (httpCode == HTTP_CODE_OK)
      // {
        // String BuildStringx = http.getString();
        // Serial.println("[+++++++++++++++++++");
        // Serial.println(BuildStringx);
        // Serial.println("[+++++++++++++++++++");
      // }
    // }
    // else
    // {
      // Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    // }
    // http.end();
  // }
  // clearBuffer();
  nextValidLineIsCall = false;
}
///////////////////////////////////////////////////////////////////////////////////
int  prendeapaga (int siono)
{
  Serial.println("KKKKKKKKKKKKKKKKKKKKKKKKKKK");
  Serial.println(lastLine);
  firstComma    = lastLine.indexOf(',');
  secondComma   = lastLine.indexOf(',', firstComma  + 1);
  String InPassword = lastLine.substring((firstComma + 1), (secondComma));
  Serial.println(InPassword);
  //PasswordOk        =  false ;
  if (InPassword == Password)
     {
		//PasswordOk = true ;
		digitalWrite(LED_BUILTIN, siono);
     }
}
int  DelAdd(int DelOrAdd)
/////////////////////////////////////////////////////////////////////////////////////////////
{
  char aux_string[100];
  firstComma          = lastLine.indexOf(',');
  secondComma         = lastLine.indexOf(',', firstComma  + 1);
  thirdComma          = lastLine.indexOf(',', secondComma + 1);
  String indexAndName = lastLine.substring((firstComma + 1), (secondComma));
  String newContact   = lastLine.substring((secondComma + 1), thirdComma);
  if (!isAuthorized)
     {
		Serial.println(j); //////////////////////////////////////////////////////////////
		Serial.println("Not authorized to Delete/Add"); /////////////////////////////////
		return 0;
     }
  String tmpx;
  tmpx = "AT+CPBW=" + indexAndName + "\r\n\"";
  if ( DelOrAdd == 1 )
     {
		tmpx = "AT+CPBW=" + indexAndName + ",\"" + newContact + "\"" + ",129," + "\"" + indexAndName + "\"" + "\r\n\"";
     }
  tmpx.toCharArray( aux_string, 100 );
  Serial.println(aux_string);
  answer = sendATcommand(aux_string, "OK\r\n", 20000, 0);   // send the SMS number
  if (answer == 1)
     {
		Serial.println("Sent ");
     }
  else
     {
		Serial.println("error ");
     }
}