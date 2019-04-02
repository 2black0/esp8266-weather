#include "ThingSpeak.h"
#include <ESP8266WiFi.h>

String ssid = "WIFI_GRATIS";   // your network SSID (name) 
String pass = "arDY1234*$";   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = 746681;
const char *myWriteAPIKey = "TLO5WA41KTENXZ1Y";

// Initialize our values
int number1 = random(0,100);
int number2 = random(0,100);
int number3 = random(0,100);
int number4 = random(0,300);

void setup() {
  Serial.begin(115200);  // Initialize serial

  WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  // set the fields with the values
  ThingSpeak.setField(1, number1);
  ThingSpeak.setField(2, number2);
  ThingSpeak.setField(3, number3);
  ThingSpeak.setField(4, number4);
  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
  number1 = random(0,100);
  number2 = random(0,100);
  number3 = random(0,100);
  number4 = random(0,100);

  delay(20000); // Wait 20 seconds to update the channel again
}
