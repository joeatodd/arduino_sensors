#define INO_DEBUG

#include <Keypad.h>
/* #include <MySensors.h> */

// 4x4 keypad matrix
const byte rows = 4;
const byte cols = 4;

char keys[rows][cols] = {{'0','1','2','3'},{'4','5','6','7'},{'8','9','10','11'},{'12','13','14','15'}}; // TODO map these

/* byte rowPins[rows] = {2,3,6,5}; */
/* byte colPins[cols] = {A4,A5,A1,A0}; // <- TODO sort a0 etc */

/* byte rowPins[rows] = {2,3,6,5}; */
/* byte colPins[cols] = {A4,A5,A1,A0}; // <- TODO sort a0 etc */

byte rowPins[rows] = {2,A4,3,A5};
byte colPins[cols] = {6,A1,5,A0};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );

/* keypad.setHoldTime(); */
/* keypad.setDebounceTime(); */

/* void presentation(){ */

/* } */

void setup(){

#ifdef INO_DEBUG
  Serial.begin(9600);
#endif


}

void loop(){
  /* Serial.println("HELLO"); */
  char key = keypad.getKey();
  if (key != NO_KEY){
    Serial.println(key);
  }

}
