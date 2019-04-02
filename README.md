# Weather Station ESP8266
## Peripheral :
```
> NodeMCU V3
> DHT11
> Rainfall Sensor
```
## Feature :
```
> Give information : Air Temperature, Humidity and Rainfall for hourly and daily into website (graph via thingspeak) and telegram bot
> give notification on Telegram if high railfall, low/high temperature or low/high humidity
```
## Note for file
### esp8266_thingspeak.ino
```
> Multiple data (4 field) send to thingspeak
```
### read_sensor.ino
```
> Read Sensor DHT11 : Air Temp & Hum
> Read rainfall sensor for hourly and daily
> get time sync with server
```
### code_fix.ino
```
> Combine between file esp8266_thingspeak and read_sensor
```
