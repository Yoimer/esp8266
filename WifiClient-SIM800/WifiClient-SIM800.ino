/*
  SoftwareSerialParser
  (based on SoftwareSerialExample.ino)

  Description: Software to parse serial data from ESP8266 and
  sent it to SIM800L-EVB

*/
#include <SoftwareSerial.h>

SoftwareSerial mySerial(0, 1); // RX, TX

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

//Indexes
byte j = -1;

void setup() {


  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("SoftwareSerialParser Started!");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  delay(1000);

}

void loop() { // run over and over

  while (mySerial.available()) {
    getSerialString();
  }
}
//////////////////////////////////////////////////////////////////////////////
boolean getSerialString()
{
  static byte dataBufferIndex = 0;
  while (mySerial.available())
  {
    char incomingbyte = mySerial.read();
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
        //delay(10000);
        if (mySerial.available()) {
          mySerial.end();
        }
        parseSerialString();
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

