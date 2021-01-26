#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <Wire.h>

#define anologPin     A0
const char* ssid = "SSID";
const char* password = "password";

String logString;
String logEnding;

int ledPin = 2;
int lowestGas = 190;
unsigned long myTime;

AsyncWebServer server(80);

void loginit(){
  logString = "<html><title>Logger</title><body><table><tr><th>Type</th><th>Value</th></tr>";
  logEnding = "</table></body></html>";
}

void logprintln(String logtype, String input){
  if (logString.length() > 2000 ) {
    loginit();
  }
  logString = logString + "<tr><td>"+logtype+"</td><td>"+input+"</td></tr>";
}

void logprintln(String logtype, int input){

    if (logString.length() > 2000 ) {
    loginit();
  }
  logString = logString + "<tr><td>"+logtype+"</td><td>"+String(input)+"</td></tr>";
}

String getLog(){
  digitalWrite(ledPin, LOW);
  delay(500);
  digitalWrite(ledPin, HIGH);
  return logString + logEnding;
  }

int findLow(int input) {
  int returnvalue;
  int gas = analogRead(anologPin);
  logprintln("GAS",gas);
  if (gas <= input) {
    returnvalue = gas;
  } else {
    returnvalue = input;
  }
  logprintln("LOW",returnvalue);
  return returnvalue;
}

String getOK() {
  return "OK";
}


String readCO2Value() {

  // first hour try to find lowes Gas level
  myTime = millis();
  if (myTime < 3600000) {
    logprintln("Stamp",myTime);
    lowestGas = findLow(lowestGas);
  }

  int co2last[10], co2lvl, co2tssn = 0, co2ppm = 0, co2avg = 0;

  for (int x = 0;x<10;x++)  //samplpe co2 10x over 2 seconds
  {
    co2last[x]=analogRead(anologPin);
    delay(200);
  }

  for (int x = 0;x<10;x++)  //add samples together
  {
    co2avg=co2avg + co2last[x];
  }
  co2tssn = co2avg/10;

  co2lvl = co2tssn - lowestGas;
  co2lvl = map(co2lvl,0,1024,400,5000);
  logprintln("PPM",co2lvl);

  digitalWrite(ledPin, LOW);
  delay(500);
  digitalWrite(ledPin, HIGH);
  return String(co2lvl);
}

void setup()
{
  pinMode(anologPin, INPUT);
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  loginit();

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  delay(5000);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    logprintln("WiFi", "Connecting...");
  }

  logprintln("WiFi", "Connected!");

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });

  server.on("/AirQuality", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readCO2Value().c_str());
  });

  server.on("/log", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", getLog().c_str());
  });

   server.on("/set", HTTP_GET, [](AsyncWebServerRequest *request){
   String urlValue = request->url();
   logprintln("URL", urlValue );
   String passedValue = urlValue.substring(5,urlValue.length());
   logprintln("Passed Value", passedValue );
   lowestGas = passedValue.toInt();
   request->send_P(200, "text/html", getOK().c_str());
  });

  // Start server
  server.begin();
}

void loop(){
}
