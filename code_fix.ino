//library used
#include <ESP8266WiFi.h>
#include <time.h>
#include <SimpleDHT.h>
#include "ThingSpeak.h"

//dht configuration
#define DHT_PIN D4
SimpleDHT11 dht11(DHT_PIN);
byte temperature = 0;
byte humidity = 0;

//rainfall configuration
#define interruptPin D1 
volatile unsigned long contactTime;
const double bucketSize = 1.346;
float dailyRain = 0.0;          
float hourlyRain = 0.0;        
float dailyRain_till_LastHour = 0.0;
bool first_hour, first_day;  

//wifi configuration
const char* ssid = "WIFI_GRATIS";
const char* pass = "arDY1234*$";
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

//time configuration
int timezone = 7 * 3600;
int dst = 0;

//thingspeak configuration
unsigned long myChannelNumber = 746681;
const char *myWriteAPIKey = "TLO5WA41KTENXZ1Y";

void setup() {
  Serial.begin(115200);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);
  delay(10);
  
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  } 
  
  configTime(timezone, dst, "pool.ntp.org","time.nist.gov");
  while(!time(nullptr)){
     Serial.print("*");
     delay(1000);
  }

  ThingSpeak.begin(client);  // Initialize ThingSpeak
  
  Serial.println(WiFi.localIP());
  Serial.println("Weather Information");
  Serial.println("--------------------");
  for(int i=0;i<10;i++){
    getSensor();
    delay(100);
  }
}

void thingspeak_send(){
    // set the fields with the values
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(3, hourlyRain);
  ThingSpeak.setField(4, dailyRain);
  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
}
 
void handleInterrupt() {
  if((millis() - contactTime) > 15){
    hourlyRain+=bucketSize;
    dailyRain+=bucketSize;
    contactTime = millis();
  }
}

void checkRain(){
  time_t now = time(nullptr);
  struct tm * timeinfo;
  timeinfo = gmtime(&now);

  // check hour
  if(timeinfo->tm_min != 0) first_hour = true;
  if(timeinfo->tm_min == 0 && first_hour == true){
    hourlyRain=0;
    first_hour = false;
  }

  // check day
  if(timeinfo->tm_hour != 0) first_day = true;
  if(timeinfo->tm_hour == 0 && first_day == true){
    dailyRain=0;
    first_day = false;
  }
  
}

/*void counterRain(){
  time_t now = time(nullptr);
  struct tm * timeinfo;
  timeinfo = gmtime(&now);
  if(timeinfo->tm_min != 0) first = true;
  if(timeinfo->tm_min == 0 && first == true){
    hourlyRain = dailyRain - dailyRain_till_LastHour;
    dailyRain_till_LastHour = hourlyRain;
    first = false;
  }
  if(timeinfo->tm_hour == 0){
    dailyRain = 0.0;
    dailyRain_till_LastHour = 0.0;
  }
}*/

void showData(){
  time_t now = time(nullptr);
  Serial.print("Date and Time Now = ");
  Serial.println(ctime(&now));
  Serial.print("Air Temperature = ");
  Serial.print(temperature);
  Serial.println(" *C");
  Serial.print("Air Humidity = ");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.print("Rainfall Hourly = ");
  Serial.print(hourlyRain,8);
  Serial.println(" mm");
  Serial.print("Rainfall Daily = ");
  Serial.print(dailyRain,8);
  Serial.println(" mm");
  Serial.println("--------------------");
}

void getSensor(){
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    //Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
    return;
  }
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
  
  getSensor();
  checkRain();
  showData();
  thingspeak_send();
  delay(20000);

}
