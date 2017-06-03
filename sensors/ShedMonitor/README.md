# Shed Monitor

Currently a temp/humidity and PIR motion detector for the shed.


## Hardware

Operating on 16 MHz Arduino Nano board, with DHT22 Temp/Hum sensor and HC-SR501 PIR motion sensor.

Powered by USB

## Script

Combines the default 'sleep and occasionally wake up to read temp/hum and transmit if changed' with 'interrupt (wake) on motion trigger'.


