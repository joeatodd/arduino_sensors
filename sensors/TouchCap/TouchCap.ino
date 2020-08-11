#include <CapacitiveSensor.h>
CapacitiveSensor Sensor = CapacitiveSensor(4, 2);
long val;
int pos;
#define led 13

void setup()
{
Serial.begin(9600);
pinMode(led, OUTPUT);
}

void loop()
{

val = Sensor.capacitiveSensor(30);
Serial.println(val);
if (val >= 1000 && pos == 0)
{
digitalWrite(led, HIGH);
pos = 1;
delay(500);
}

else if (val >= 1000 && pos == 1)
{
digitalWrite(led, LOW);
pos = 0;
delay(500);
}

delay(10);
}
