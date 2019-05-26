/** Sketch for monitoring T/H, motion, door, sound in the garage, from 
 * DhtTemperatureAndHumiditySensor.ino.
 * Uses DHT22, Motion Unit, Door reed switch & sound monitor

 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * REVISION HISTORY
 * Version 1.0: Henrik EKblad
 * Version 1.1 - 2016-07-20: Converted to MySensors v2.0 and added various improvements - Torben Woltjen (mozzbozz)
 * 
 * DESCRIPTION
 * This sketch provides an example of how to implement a humidity/temperature
 * sensor using a DHT11/DHT-22.
 *  
 * For more information, please visit:
 * http://www.mysensors.org/build/humidity
 * 
 */

// Enable debug prints
#define MY_DEBUG

// Enable and select radio type attached 
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69
//#define MY_RS485

// Help others...
#define MY_REPEATER_FEATURE
#define MY_NODE_ID 1

#include <SPI.h>
#include <MySensors.h>  
#include <DHT.h>
#include <Bounce2.h>

// Set this to the pin you connected the DHT's data pin to
#define DHT_DATA_PIN 3

// Set this offset if the sensor has a permanent small offset to the real temperatures
#define SENSOR_TEMP_OFFSET 0

// Sleep time between sensor updates (in milliseconds)
// Must be >1000ms for DHT22 and >2000ms for DHT11
// static const uint64_t UPDATE_INTERVAL = 6000;

// Force sending an update of the temperature after n sensor reads, so a controller showing the
// timestamp of the last update doesn't show something like 3 hours in the unlikely case, that
// the value didn't change since;
// i.e. the sensor would force sending an update every UPDATE_INTERVAL*FORCE_UPDATE_N_READS [ms]
static const uint8_t FORCE_UPDATE_N_READS = 10;

const unsigned long tUpdate=6000; // update interval data
unsigned long t0 = 0;  // The millisecond clock in the main loop.

#define DIGITAL_INPUT_PIR 2   // The digital input you attached your motion sensor.  (Only 2 and 3 g
#define DIGITAL_INPUT_SOUND 5 // Sound sensor digital input
#define DIGITAL_INPUT_DOOR 4 // Door reed switch sensor

#define CHILD_ID_HUM 0
#define CHILD_ID_TEMP 1
#define CHILD_ID_PIR 2
#define CHILD_ID_DOOR 3
#define CHILD_ID_SOUND 4

float lastTemp;
float lastHum;
uint8_t nNoUpdatesTemp;
uint8_t nNoUpdatesHum;
bool metric = true;

bool pir_lastTripped = false;

// Door stuff
Bounce debouncer = Bounce();
int door_oldValue=-1;
//

// Sound stuff
int soundDetectedVal = HIGH; // This is where we record our Sound Measurement
boolean bAlarm = false;
unsigned long lastSoundDetectTime; // Record the time that we measured a sound
int soundAlarmTime = 500; // Number of milli seconds to keep the sound alarm high
//

MyMessage msgHum(CHILD_ID_HUM, V_HUM);
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);
MyMessage msgDoor(CHILD_ID_DOOR,V_TRIPPED);
DHT dht;

// Initialize motion & sound message
MyMessage msgPIR(CHILD_ID_PIR, V_TRIPPED);
MyMessage msgSound(CHILD_ID_SOUND, V_TRIPPED);

void presentation()  
{ 
  // Send the sketch version information to the gateway
  sendSketchInfo("GarageMonitor", "1.2");
  
  // Register all sensors to gw (they will be created as child devices)
  present(CHILD_ID_HUM, S_HUM);
  present(CHILD_ID_TEMP, S_TEMP);
  present(CHILD_ID_PIR, S_MOTION);
  present(CHILD_ID_DOOR, S_DOOR);
  present(CHILD_ID_SOUND, S_DOOR);
  metric = getControllerConfig().isMetric;
}


void setup()
{
  dht.setup(DHT_DATA_PIN); // set data pin of DHT sensor
//  if (UPDATE_INTERVAL <= dht.getMinimumSamplingPeriod()) {
//    Serial.println("Warning: UPDATE_INTERVAL is smaller than supported by the sensor!");
//  }

  // set timer
  t0=millis();

  // Sleep for the time of the minimum sampling period to give the sensor time to power up
  // (otherwise, timeout errors might occure for the first reading)
  sleep(dht.getMinimumSamplingPeriod());

  pinMode(DIGITAL_INPUT_PIR, INPUT);      // sets the motion sensor digital pin as input
  pinMode(DIGITAL_INPUT_DOOR, INPUT);     // sets door sensor pin as input
  digitalWrite(DIGITAL_INPUT_DOOR, HIGH); // pull-up

  pinMode(DIGITAL_INPUT_SOUND, INPUT);     // sets door sensor pin as input

  // After setting up the button, setup debouncer
  debouncer.attach(DIGITAL_INPUT_DOOR);
  debouncer.interval(5);
}


void loop()      
{ 

  //  DOOR
  debouncer.update();
  // Get the update value
  int door_value = debouncer.read();
  if (door_value != door_oldValue) {
     // Send in the new value
     send(msgDoor.set(door_value==HIGH ? 1 : 0));
     door_oldValue = door_value;
  }

  // PIR
  // Read digital motion value
  bool pir_tripped = digitalRead(DIGITAL_INPUT_PIR) == HIGH;
  if(pir_tripped != pir_lastTripped){
    pir_lastTripped = pir_tripped;
    Serial.println(pir_tripped);  
    send(msgPIR.set(pir_tripped?"1":"0"));  // Send tripped value to gw
  }

  // Sound detected?
  soundDetectedVal = digitalRead(DIGITAL_INPUT_SOUND) ; // read the sound alarm time
  if (soundDetectedVal == 1) // If we hear a sound
  {
    /* Serial.println("loud"); */
    lastSoundDetectTime = millis(); // record the time of the sound alarm
    // The following is so you don't scroll on the output screen
    if (!bAlarm){
      Serial.println("LOUD");
      send(msgSound.set(1));  // Send tripped value to gw
      //gw.send(msg.set(OPEN));
      bAlarm = true;
    }
  }
  else
  {
    /* Serial.println("quiet"); */
    if( (millis()-lastSoundDetectTime) > soundAlarmTime  &&  bAlarm){
      Serial.println("quiet");
      send(msgSound.set(0));
      bAlarm = false;
    }
  }

  // DHT - temp hum
  if((millis()-t0) > tUpdate){
    t0=millis();

    // Force reading sensor, so it works also after sleep()
    dht.readSensor(true);
    // Get temperature from DHT library
    float temperature = dht.getTemperature();
    Serial.println(temperature);
    if (isnan(temperature)) {
      Serial.println("Failed reading temperature from DHT!");
    } else if (temperature != lastTemp || nNoUpdatesTemp == FORCE_UPDATE_N_READS) {
      // Only send temperature if it changed since the last measurement or if we didn't send an update for n times
      lastTemp = temperature;
      if (!metric) {
	temperature = dht.toFahrenheit(temperature);
      }
      // Reset no updates counter
      nNoUpdatesTemp = 0;
      temperature += SENSOR_TEMP_OFFSET;
      send(msgTemp.set(temperature, 1));

      #ifdef MY_DEBUG
      Serial.print("T: ");
      Serial.println(temperature);
      #endif
    } else {
      // Increase no update counter if the temperature stayed the same
      nNoUpdatesTemp++;
    }

    // Get humidity from DHT library
    float humidity = dht.getHumidity();
    if (isnan(humidity)) {
      Serial.println("Failed reading humidity from DHT");
    } else if (humidity != lastHum || nNoUpdatesHum == FORCE_UPDATE_N_READS) {
      // Only send humidity if it changed since the last measurement or if we didn't send an update for n times
      lastHum = humidity;
      // Reset no updates counter
      nNoUpdatesHum = 0;
      send(msgHum.set(humidity, 1));

      #ifdef MY_DEBUG
      Serial.print("H: ");
      Serial.println(humidity);
      #endif
    } else {
      // Increase no update counter if the humidity stayed the same
      nNoUpdatesHum++;
    }

  }

  // Sleep for a while to save energy
  //  sleep(UPDATE_INTERVAL);
  // sleep(digitalPinToInterrupt(DIGITAL_INPUT_PIR), CHANGE, UPDATE_INTERVAL);
}
