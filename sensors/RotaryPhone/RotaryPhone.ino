#define INO_DEBUG

#include <Keypad.h>
/* #include <MySensors.h> */

// 4x4 keypad matrix (one whole row is just the central button)
const byte rows = 4;
const byte cols = 4;


char keys[rows][cols] = {{'*','7','4','1'},
			 {'0','8','5','2'},
			 {'#','9','6','3'},
			 {'R','R','R','R'}}; // this whole row is just the reset button


// Had to work out these pins by trial and error...
byte rowPins[rows] = {A0,A1,A4,A5};
byte colPins[cols] = {2,3,5,6};

// Conclusion - something wrong with A4, A5? They may be different by default
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
