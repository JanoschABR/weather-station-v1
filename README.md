# Weather Station v1
A simple weather station based on an ESP8266.

![Overview](https://github.com/JanoschABR/weather-station-v1/blob/d3cf0b37891ef15b32f3fb7d15556ca328d79d1d/assets/overview.jpg)

## Todo
 - [ ] - [05/01/2022] - Reconnect on WiFi failure
 - [X] - [05/01/2022] - Automatic restart to ensure internal timer doesn't drift  
<sup>&nbsp;&nbsp;&nbsp;This is true for Arduinos (after ~50 days), not so sure about ESPs. Probably best to add it, just to be sure.</sup>

## Oops~!
As of right now the DHT21 is connected to the same pin as the BUILTIN_LED. I noticed this after I assembled the circuit and didn't want to change it again. This is not really an issue, since it doesn't seem to make the sensor output wrong data. This does however make the builtin LED unuseable; The LED will simply light up whenever data is read from the sensor.

## Building your own weather station
If you want to build your own you can have a look at [the Wiki](https://github.com/JanoschABR/weather-station-v1/wiki/Components-&-Wiring).  
If you do end up using my code or instructions I would love to hear about it!
