#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>

// DEFAULT variables *********************************************************
const char* ssid="Casa";             // yourSSID
const char* password="remioy2006202";     // yourPASSWORD
int LED=2;                              // PINnumber where your LED is
int websockMillis=50;                    // SocketVariables are sent to client every 50 milliseconds
int sliderVal=60;                        // Default value of the slider
// END ***********************************************************************

ESP8266WebServer server(80);
WebSocketsServer webSocket=WebSocketsServer(88);
String webSite,javaScript,JSONtxt;
unsigned long websockCount=0UL,wait000=0UL,wait001=0UL;
int LEDmillis=9*(100-sliderVal)+100;
boolean LEDonoff=true;

void buildWebsite(){
  buildJavascript();
  webSite="<!DOCTYPE HTML><HTML>\n";
  webSite+="<META name='viewport' content='width=device-width, initial-scale=1'>\n";
  webSite+=javaScript;
  webSite+="<BODY>\n";
  webSite+="<BR><B>This is the ESP website ...</B><BR><BR>\n";
  webSite+="Runtime = <A ID='runtime'></A><BR>\n";
  webSite+="websockCount = <A ID='websockCount'></A><BR><BR>\n";
  webSite+="<TABLE BORDER=1 WIDTH=200px BGCOLOR='cornsilk' STYLE='border-collapse:collapse;text-align:center'>\n"; 
  webSite+="<TR><TD>Slidervalue = <A ID='Sliderval'></A><BR>\n";
  webSite+="LEDspeed = <A ID='LEDmillis'></A> ms</TD></TR>\n";
  //********** ONCHANGE works in IE, use ONINPUT in Firefox and Chrome **********
  webSite+="<TR><TD><INPUT ID='slider' TYPE='range' ONCHANGE='Slider()' STYLE='writing-mode:bt-lr;-webkit-appearance:slider-vertical;' orient='vertical'></TD></TR>\n";
  webSite+="<TR><TD><BUTTON ID='button' ONCLICK='button()' STYLE='width:110px;height:40px'></BUTTON></TD></TR>\n";
  webSite+="</TABLE>\n"; 
  webSite+="</BODY>\n";
  webSite+="</HTML>\n";
}

void buildJavascript(){
  javaScript="<SCRIPT>\n";
  javaScript+="InitWebSocket();\n";
  javaScript+="function InitWebSocket(){\n";
  javaScript+="  websock=new WebSocket('ws://'+window.location.hostname+':88/');\n";
  javaScript+="  websock.onmessage=function(evt){\n";
  javaScript+="    JSONobj=JSON.parse(evt.data);\n";
  javaScript+="    document.getElementById('runtime').innerHTML=JSONobj.runtime;\n";
  javaScript+="    document.getElementById('websockCount').innerHTML=JSONobj.websockCount;\n";
  javaScript+="    document.getElementById('slider').value=JSONobj.sliderVal;\n";   
  javaScript+="    document.getElementById('Sliderval').innerHTML=JSONobj.sliderVal;\n"; 
  javaScript+="    document.getElementById('LEDmillis').innerHTML=JSONobj.LEDmillis;\n"; 
  javaScript+="    document.getElementById('button').innerHTML=JSONobj.LEDonoff;\n"; 
  javaScript+="  }\n";
  javaScript+="}\n";

  javaScript+="function button(){\n";
  javaScript+="  btn='LEDonoff=LED = ON';\n";
  javaScript+="  if(document.getElementById('button').innerHTML==='LED = ON')btn='LEDonoff=LED = OFF';\n";
  javaScript+="  websock.send(btn);\n";
  javaScript+="}\n";

  javaScript+="function Slider(){\n";
  javaScript+="  sliderVal=document.getElementById('slider').value;\n";
  javaScript+="  websock.send('sliderVal='+sliderVal);\n";
  javaScript+="}\n";
 
  javaScript+="</SCRIPT>\n";
}

String millis2time(){
  String Time="";
  unsigned long ss;
  byte mm,hh;
  ss=millis()/1000;
  hh=ss/3600;
  mm=(ss-hh*3600)/60;
  ss=(ss-hh*3600)-mm*60;
  if(hh<10)Time+="0";
  Time+=(String)hh+":";
  if(mm<10)Time+="0";
  Time+=(String)mm+":";
  if(ss<10)Time+="0";
  Time+=(String)ss;
  return Time;
}

void handleWebsite(){
  buildWebsite();
  server.send(200,"text/html",webSite);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t wslength){
  String payloadString=(const char *)payload;
  //Serial.println("payload: '"+payloadString+"', channel: "+(String)num);
  if(type==WStype_TEXT){
    byte separator=payloadString.indexOf('=');
    String var=payloadString.substring(0,separator);
    String val=payloadString.substring(separator+1);
    if(var=="LEDonoff"){
      LEDonoff=false;
      if(val=="LED = ON")LEDonoff=true;
      digitalWrite(LED,HIGH);
    }else if(var=="sliderVal"){
      sliderVal=val.toInt();
      LEDmillis=9*(100-sliderVal)+100;
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED,OUTPUT);
  WiFi.begin(ssid,password);
  while(WiFi.status()!=WL_CONNECTED)delay(500);
  WiFi.mode(WIFI_STA);
  Serial.println("\n\nBOOTING ESP8266 ...");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("Station IP address = ");
  Serial.println(WiFi.localIP());
  server.on("/",handleWebsite);
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
  server.handleClient();

  if(millis()>wait000&&LEDonoff==true){
    digitalWrite(LED,!digitalRead(LED));
    wait000=millis()+LEDmillis;
  }

  if(millis()>wait001){
    websockCount++;
    String LEDswitch="LED = OFF";
    if(LEDonoff==true)LEDswitch="LED = ON";
    JSONtxt="{\"runtime\":\""+millis2time()+"\","+              // JSON requires double quotes
             "\"websockCount\":\""+(String)websockCount+"\","+
             "\"sliderVal\":\""+(String)sliderVal+"\","+
             "\"LEDmillis\":\""+(String)LEDmillis+"\","+
             "\"LEDonoff\":\""+LEDswitch+"\"}";
    webSocket.broadcastTXT(JSONtxt);
    wait001=millis()+websockMillis;
  }
}
