# GarageBak (basically a copy of Shed Monitor, then relegated to backup)

Currently a temp/humidity and PIR motion detector for the shed.


## Hardware

Operating on 8 MHz Arduino Pro Mini board, with DHT22 Temp/Hum sensor and HC-SR501 PIR motion sensor.

DHT on pin 5 & PIR on pin 2

Powered by 2xAA

## Script

Combines the default 'sleep and occasionally wake up to read temp/hum and transmit if changed' with 'interrupt (wake) on motion trigger'.


