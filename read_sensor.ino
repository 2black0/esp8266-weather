//library used
#include <ESP8266WiFi.h>
#include <time.h>
#include <SimpleDHT.h>

//dht configuration
#define DHT_PIN D4
SimpleDHT11 dht11(DHT_PIN);
byte temperature = 0;
byte humidity = 0;

//rainfall configuration
#define interruptPin D1 
volatile unsigned long contactTime;
const double bucketSize = 1.346;
double dailyRain = 0.0;          
double hourlyRain = 0.0;        
double dailyRain_till_LastHour = 0.0;
bool first;  

//wifi configuration
const char* ssid = "WIFI_GRATIS";
const char* password = "arDY1234*$";

//time configuration
int timezone = 7 * 3600;
int dst = 0;

void setup() {
  Serial.begin(115200);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);
  delay(10);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  } 
  
  configTime(timezone, dst, "pool.ntp.org","time.nist.gov");
  while(!time(nullptr)){
     Serial.print("*");
     delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println("Weather Information");
  Serial.println("--------------------");
}
 
void handleInterrupt() {
  if((millis() - contactTime) > 15){
    dailyRain+=bucketSize;
    contactTime = millis();
  }
}

void rain_daily(){
  time_t now = time(nullptr);
  struct tm * timeinfo;
  timeinfo = gmtime(&now);
  
  //Serial.println("--------------");
  //Serial.print("Hour = ");Serial.println(timeinfo->tm_hour);
  //Serial.print("Min = ");Serial.println(timeinfo->tm_min); 
  //Serial.print("Sec = ");Serial.println(timeinfo->tm_sec); 
  //Serial.println("--------------");
}

void counterRain(){
  time_t now = time(nullptr);
  struct tm * timeinfo;
  timeinfo = gmtime(&now);
  //Serial.print(timeinfo->tm_hour);
  //Serial.print(timeinfo->tm_min); 
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
  Serial.print("Rainfall in last Hour = ");
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
  getSensor();
  counterRain();
  //rain_daily();
  showData();
  delay(2000);

}
