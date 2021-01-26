# Carbon Dioxide Parts Per Million Meter
## CO2PPM
```
Air Quality Sensor based on:
ESP-8266
MQ-135
```
| Situation | PPM |
| ----------- | ----------- |
| Atmospheric CO2 Level | 400ppm |
| Average indoor co2 | 350-450ppm |
| Maxiumum acceptable co2| 1000ppm|
| Dangerous co2 levels | >2000ppm |

## End Points

| What | URL |
| ----------- | ----------- |
| Grafh at root | http://192.168.1.100/ |
| View log at | http://192.168.1.100/log |
| To set a new low value | http://192.168.1.100/set/190 |
| Read PPM | http://192.168.1.100/AirQuality |

![Chart Example](grafh.png)
