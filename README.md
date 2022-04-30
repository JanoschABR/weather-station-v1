# Weather Station v1
A simple weather station based on an ESP8266.

![Overview](https://github.com/JanoschABR/weather-station-v1/blob/d3cf0b37891ef15b32f3fb7d15556ca328d79d1d/assets/overview.jpg)

## Things for the future
As of right now the DHT21 is connected to the same pin as the BUILTIN_LED. I only noticed this after I already assembled the circuit. This is not really an issue, since it doesn't seem to make the sensor output wrong data. However it does make it impossible to use the builtin LED for anything. The LED will simply light up whenever data is read from the sensor.

***
### For more information, have a look at the Wiki.
