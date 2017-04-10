/*
  SoftwareSerialParser
  (based on SoftwareSerialExample.ino)

  Description: Software to parse serial data from ESP8266 and
  sent it to SIM800L-EVB

*/
#include <SoftwareSerial.h>

//ESP8266TX is connected to Arduino RX D0
#define ESP8266TX 0

//ESP8266RX is connected to Arduino TX D1
#define ESP8266RX 1

//Create software serial object to communicate with ESP8266
SoftwareSerial ESP8266(ESP8266TX, ESP8266RX);

////SoftwareSerial ESP8266(0, 1); // RX, TX

//SIM800 TX is connected to Arduino D8
#define SIM800_TX_PIN 8

//SIM800 RX is connected to Arduino D7
#define SIM800_RX_PIN 7

//Create software serial object to communicate with SIM800
SoftwareSerial serialSIM800(SIM800_TX_PIN, SIM800_RX_PIN);

#define TIMEOUT 5000

#define onModulePin 13

char startChar = '#'; // or '!', or whatever your start character is
char endChar = '#';
boolean storeString = false; //This will be our flag to put the data in our buffer

int const DATABUFFERSIZE = 512;
char dataBuffer[DATABUFFERSIZE + 1]; //Add 1 for NULL terminator

char* wordlist; // Parse Processor

// Actual and New Contacts
byte oldNumber = -1;
byte newNumber = -1;

//DATABUFFERPHONENUMBER = 30 contacts
char* phoneNumber[31]; //Add 1 for NULL terminator

// Array to be used for converting a string to integer on ClearWhiteList()
char jj[DATABUFFERSIZE + 1];

//Indexes
int j = -1;

// Temporal variable
String tmp = "";


int8_t answer;


void setup() {


  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("SoftwareSerialParser Started!");

  // set the data rate for the SoftwareSerial port
  ESP8266.begin(9600);
  delay(1000);

}

void loop() { // run over and over

  while (ESP8266.available()) {
    getSerialString();
  }
}
//////////////////////////////////////////////////////////////////////////////
boolean getSerialString()
{
  static byte dataBufferIndex = 0;
  while (ESP8266.available())
  {
    char incomingbyte = ESP8266.read();
    // if(incomingbyte == startChar)
    // {
    //     dataBufferIndex = 0;  //Initialize our dataBufferIndex variable
    //     storeString = true;
    // }
    if (storeString)
    {
      //Let's check our index here, and abort if we're outside our buffer size
      //We use our define here so our buffer size can be easily modified
      if (dataBufferIndex == DATABUFFERSIZE)
      {
        //Oops, our index is pointing to an array element outside our buffer.
        dataBufferIndex = 0;
        break;
      }
      if (incomingbyte == endChar)
      {
        dataBuffer[dataBufferIndex] = 0; //null terminate the C string
        //Our data string is complete.  return true
        Serial.println(dataBuffer);
        
        // Closes communication with ESP8266
        if (ESP8266.available()) {
          ESP8266.end();
        }
        
        parseSerialString();
        
        // Opens communication with SIM800
        serialSIM800.begin(9600);
        delay(1000);
        
        LoadWhiteList();
        
        return true;
      }
      else
      {
        dataBuffer[dataBufferIndex++] = incomingbyte;
        dataBuffer[dataBufferIndex] = 0; //null terminate the C string   //<Hello, 10.5, 21>
      }
    }
    else
    {

    }
    if (incomingbyte == startChar)
    {
      dataBufferIndex = 0;  //Initialize our dataBufferIndex variable
      storeString = true;
    }
  }

  //We've read in all the available Serial data, and don't have a valid string yet, so return false
  return false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void parseSerialString()
{
  Serial.println("On parseSerialString...");
  wordlist = strtok(dataBuffer, ",");  //Extracts first comma
  Serial.print("Printing wordlist:");
  Serial.println(wordlist);
  oldNumber = atoi(wordlist);         //Converts first number into an integer
  Serial.print("Printing oldNumber :");
  Serial.println(oldNumber);
  wordlist = strtok(NULL, ",");      //Extracts second comma
  newNumber = atoi(wordlist);       //Converts second number into an integer
  Serial.print("Printing newNumber :");
  Serial.println(newNumber);
  static byte phoneNumberIndex = 0;
  while ((wordlist = strtok(NULL, ",")) != NULL)
  {
    phoneNumber[phoneNumberIndex++] = wordlist;
    phoneNumber[phoneNumberIndex] = 0; //null terminate the C string
  }

  j = 0;
  for ( j = 0; j < newNumber; ++j)
  {
    Serial.println(phoneNumber[j]);
  }
}
//////////////////////////////////////////////////////////////////////////////////
void ClearWhiteList()
{
  j = 1;         // lleva la cuenta de los nros a borrar
  while (j <= oldNumber)
  {
    String jj = String(j); // Converts int to String
    tmp = "AT+CPBW=" + jj + "\r\n";
    Serial.println(tmp);       // comando AT a ejecutar (LIBELIUM o SALVADOREÑOS)
    
    sendATcommand(tmp.c_str(), "OK\r\n", TIMEOUT);

    //if (0 != gprs.sendCmdAndWaitForResp(tmp, "OK", TIMEOUT))
    j = j + 1;
  }
}
////////////////////////////////////////////////////////////////////////////////////////
void LoadWhiteList()
{
  
  power_on();
  ClearWhiteList();
  j = 0;
  while (j < newNumber)
  {
    String jj = String(j + 1); // Converts int to String
    tmp = "";
    tmp = "AT+CPBW=" + jj + ",\"" + phoneNumber[j] + "\",129,\"" + jj + "\"\r\n";
    Serial.println(tmp);  // comando AT a ejecutar (LIBELIUM o SALVADOREÑOS)
    sendATcommand(tmp.c_str(), "OK\r\n", TIMEOUT);
    j = j + 1;
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////
int8_t sendATcommand(const char* ATcommand, const char* expected_answer1, unsigned int timeout) {

  uint8_t x = 0,  answer = 0;
  char response[100];
  unsigned long previous;

  memset(response, '\0', 100);    // Initialize the string

  delay(100);

  //while ( Serial.available() > 0) Serial.read();   // Clean the input buffer

  //  while(Serial.available()) { //Cleans the input buffer
  //        Serial.read();
  //    }

  while (serialSIM800.available()) { //Cleans the input buffer
    serialSIM800.read();
  }
  
  Serial.println(ATcommand);    // Prints the AT command
  serialSIM800.write(ATcommand); // Sends the AT command
  x = 0;
  previous = millis();

  // this loop waits for the answer
  do {
    ////if (Serial.available() != 0) {
    if (serialSIM800.available() != 0) {
      ////response[x] = Serial.read();
      response[x] = serialSIM800.read();
      x++;
      // check if the desired answer is in the response of the module
      if (strstr(response, expected_answer1) != NULL)
      {
        answer = 1;
      }
    }
    // Waits for the asnwer with time out
  }
  while ((answer == 0) && ((millis() - previous) < timeout));

  return answer;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
void power_on() {

  uint8_t answer = 0;

  Serial.println("On Power_on...");

  // checks if the module is started
  answer = sendATcommand("AT\r\n", "OK\r\n", TIMEOUT);
  if (answer == 0)
  {
    // power on pulse
    digitalWrite(onModulePin, HIGH);
    delay(3000);
    digitalWrite(onModulePin, LOW);

    // waits for an answer from the module
    while (answer == 0) {
      // Send AT every two seconds and wait for the answer
      answer = sendATcommand("AT\r\n", "OK\r\n", TIMEOUT);
      Serial.println("Trying connection with module...");
    }
  }
}

