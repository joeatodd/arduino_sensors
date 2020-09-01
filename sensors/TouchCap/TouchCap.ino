// ==== Radio Stuff ====
// Enable debug prints
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69
//#define MY_RS485

#define MY_NODE_ID 4

#define CHILD_ID_BUTT 0

#include <SPI.h>
#include <MySensors.h>

MyMessage msgButt(CHILD_ID_BUTT, V_STATUS);

// ==== Cap Stuff ====

#include <CapacitiveSensor.h>
CapacitiveSensor Sensor = CapacitiveSensor(4, 2);
long val;
int pos;


// Radio stuff
void presentation()
{
  // Send the sketch version information to the gateway
  sendSketchInfo("TouchCap", "1.0");

  // Register all sensors to gw (they will be created as child devices)
  present(CHILD_ID_BUTT, S_BINARY);
}


void setup()
{
Serial.begin(9600);
pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{

val = Sensor.capacitiveSensor(30);
Serial.println(val);
if (val >= 1000 && pos == 0)
{
send(msgButt.set(1));
digitalWrite(LED_BUILTIN, HIGH);
pos = 1;
delay(500);
}

else if (val >= 1000 && pos == 1)
{
send(msgButt.set(0));
digitalWrite(LED_BUILTIN, LOW);
pos = 0;
delay(500);
}

delay(10);
}
