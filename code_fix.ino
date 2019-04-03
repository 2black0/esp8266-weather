//library used
#include <ESP8266WiFi.h>
#include <time.h>
#include <SimpleDHT.h>
#include "ThingSpeak.h"
#include "CTBot.h"

//dht configuration
#define DHT_PIN D4
SimpleDHT11 dht11(DHT_PIN);
byte temperature = 0.0;
byte humidity = 0.0;

//rainfall configuration
#define interruptPin D1 
volatile unsigned long contactTime;
const double bucketSize = 1.346;
float dailyRain = 0.0;          
float hourlyRain = 0.0;
float dailyRain_last = 0.0;          
float hourlyRain_last = 0.0;      
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

//telegram configuration
CTBot myBot;
String token = "791642742:AAGsnCIQbPVhWpR2L1kER4wPANhGTq8mUpQ"   ; // REPLACE myToken WITH YOUR TELEGRAM BOT TOKEN
int64_t userid;
byte temp_s =0, hum_s =0;
float hourlyRain_s=0;

//led configuration
uint8_t led = 2;  

//timer intterupt configuration
int x=0, y=0, z=120;

void timer0_ISR (void) { 
  x=x+1;
  timer0_write(ESP.getCycleCount() + 80000000L); // 80MHz == 1sec
  if(x==z){
    y=x;
    x=0;
  }
} 

void setup() {
  Serial.begin(115200);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);
  delay(10);
  
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  } 
  
  configTime(timezone, dst, "pool.ntp.org","time.nist.gov");
  while(!time(nullptr)){
    Serial.print("*");
    delay(1000);
  }
  
  myBot.setTelegramToken(token);
  ThingSpeak.begin(client);
  
  Serial.println(WiFi.localIP());
  Serial.println("Weather Information");
  Serial.println("--------------------");
  for(int i=0;i<10;i++){
    getSensor();
    delay(100);
  }
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);

  noInterrupts(); 
  timer0_isr_init(); 
  timer0_attachInterrupt(timer0_ISR);
  timer0_write(ESP.getCycleCount() + 80000000L); // 80MHz == 1sec 
  interrupts();
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
    Serial.println("--------------------");
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
    hourlyRain_last = hourlyRain;
    hourlyRain = 0;
    hourlyRain_s = 0;
    first_hour = false;
  }

  // check day
  if(timeinfo->tm_hour != 0) first_day = true;
  if(timeinfo->tm_hour == 0 && first_day == true){
    dailyRain = 0;
    dailyRain_last = dailyRain;
    first_day = false;
  }
  
}

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
  Serial.print("Rainfall last Hour = ");
  Serial.print(hourlyRain_last,8);
  Serial.println(" mm");
  Serial.print("Rainfall Yesterday = ");
  Serial.print(dailyRain_last,8);
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

void send_notif(){ 
  if(temperature >= 40 and temperature != temp_s){
    temp_s = temperature;
    String warning = (String)"********* WARNING *********\n";
    warning += (String)"Air Temperature is HIGH !!" + String(temperature) + (String)" C\n";
    myBot.sendMessage(userid, warning);
  }
  if(temperature <= 20 and temperature != temp_s){
    temp_s = temperature;
    String warning = (String)"********* WARNING *********\n";
    warning += (String)"Air Temperature is LOW !!" + String(temperature) + (String)" C\n";
    myBot.sendMessage(userid, warning);
  }
  if(humidity >= 90 and humidity != hum_s){
    hum_s = humidity;
    String warning = (String)"********* WARNING *********\n";
    warning += (String)"Air Humidity is HIGH !!" + String(humidity) + (String)" %\n";
    myBot.sendMessage(userid, warning);
  }
  if(humidity <= 70 and humidity != hum_s){
    hum_s = humidity;
    String warning = (String)"********* WARNING *********\n";
    warning += (String)"Air Humidity is LOW !!" + String(humidity) + (String)" %\n";
    myBot.sendMessage(userid, warning);
  }
  if(hourlyRain >= 30 and hourlyRain != hourlyRain_s){
    hourlyRain_s = hourlyRain;
    String warning = (String)"********* WARNING *********\n";
    warning += (String)"Rainfall is HIGH !!" + String(hourlyRain) + (String)" mm\n";
    myBot.sendMessage(userid, warning);
  }
}

void readtelegram(){
  TBMessage msg; 
  if (myBot.getNewMessage(msg)) {
    if (msg.text.equalsIgnoreCase("/temp")) {
      getSensor();
      String sensor = (String)"Air Temperature = " + String(temperature) + (String)" C\n";
      myBot.sendMessage(msg.sender.id, sensor);
    }
    if (msg.text.equalsIgnoreCase("/hum")) {
      getSensor();
      String sensor = (String)"Air Humidity = " + String(humidity) + (String)" %\n";
      myBot.sendMessage(msg.sender.id, sensor);
    }
    if (msg.text.equalsIgnoreCase("/rain_now")) {
      String sensor = (String)"Total Rainfall on this Day = " + String(dailyRain) + (String)" mm\n";
      myBot.sendMessage(msg.sender.id, sensor);
    }
    if (msg.text.equalsIgnoreCase("/rain_day")) {
      String sensor = (String)"Total Rainfall on yesterday = " + String(dailyRain_last) + (String)" mm\n";
      myBot.sendMessage(msg.sender.id, sensor);
    }
    if (msg.text.equalsIgnoreCase("/rain_hour")) {
      String sensor = (String)"Total Rainfall an hour before = " + String(hourlyRain_last) + (String)" mm\n";
      myBot.sendMessage(msg.sender.id, sensor);
    }
    if (msg.text.equalsIgnoreCase("/all")) {
      getSensor();
      String sensor = (String)"Air Temperature = " + String(temperature) + (String)" C\n";
      sensor += (String)"Air Humidity = " + String(temperature) + (String)" %\n";
      sensor += (String)"Total Rainfall on this Day = " + String(dailyRain) + (String)" mm\n";
      sensor += (String)"Total Rainfall on yesterday = " + String(dailyRain_last) + (String)" mm\n";
      sensor += (String)"Total Rainfall an hour before = " + String(hourlyRain_last) + (String)" mm\n";
      myBot.sendMessage(msg.sender.id, sensor);
    }
    else if ((msg.text.equalsIgnoreCase("/start")) || (msg.text.equalsIgnoreCase("/help"))) {
      String welcome = (String)"Welcome " + msg.sender.username + (String)"\n";
      welcome += (String)"This is a bot who can send weather information\n";
      welcome += (String)"Please command me something : \n";
      welcome += (String)"/temp : read air temperature\n";
      welcome += (String)"/hum : read air humidify\n";
      welcome += (String)"/rain_now : read rainfall total for today\n";
      welcome += (String)"/rain_day : read rainfall on yesterday\n";
      welcome += (String)"/rain_hour : read rainfall an hour before\n";
      welcome += (String)"/all : read all data\n";
      welcome += (String)"If you have some question with this bot don't hesitate to contact developer\n";
      myBot.sendMessage(msg.sender.id, welcome);
      userid = msg.sender.id;
    }
  }
  delay(500);
}

void reconnect_wifi(){
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
}

void loop() {
  reconnect_wifi();
  getSensor();
  checkRain();
  showData();
  readtelegram();
  send_notif();
  /*if(y==z){
    y=0;
    thingspeak_send();
  }*/
}
