/**
 * @example ConnectWiFi.ino
 * @brief The ConnectWiFi demo of library WeeESP8266. 
 * @author Wu Pengfei<pengfei.wu@itead.cc> 
 * @date 2015.03
 * 
 * @par Copyright:
 * Copyright (c) 2015 ITEAD Intelligent Systems Co., Ltd. \n\n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version. \n\n
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <SoftwareSerial.h>

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
SoftwareSerial mySerial(ESP8266_RX_PIN, ESP8266_TX_PIN);
 
////SoftwareSerial mySerial(3, 2); /* RX:D3, TX:D2 */

#include "ESP8266.h"
#define SSID        "Casa"
#define PASSWORD    "remioy2006202"

//Create ESP8266 wifi object on mySerial to be able to use all ITEAD libraries
ESP8266 wifi(mySerial);


void setup(void)
{
    Serial.begin(9600);
    Serial.print("setup begin\r\n");

    Serial.print("FW Version: ");
    Serial.println(wifi.getVersion().c_str());
    
    
    if (wifi.setOprToStation()) {
        Serial.print("to station ok\r\n");
    } else {
        Serial.print("to station err\r\n");
    }

    if (wifi.joinAP(SSID, PASSWORD)) {
        Serial.print("Join AP success\r\n");
        Serial.print("IP: ");       
        Serial.println(wifi.getLocalIP().c_str());
    } else {
        Serial.print("Join AP failure\r\n");
    }
    
    Serial.print("setup end\r\n");
}

void loop(void)
{
}

