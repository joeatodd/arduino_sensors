#define INO_DEBUG

#include <Keypad.h>
/* #include <MySensors.h> */

// 3x4 keypad matrix
const byte rows = 3;
const byte cols = 4;

char keys[rows][cols] = {{'0','1','2','3'},
			 {'4','5','6','7'},
			 {'8','9','#','*'}};

// Had to work out these pins by trial and error...
byte rowPins[rows] = {A0,A1,A4};
byte colPins[cols] = {2,3,5,6};

// corrected:

// #  0
// *  8
// 0  4
// 1  9
// 2  5
// 3  1
// 4  0
// 5  6
// 6  2
// 7  1
// 8  7
// 9  3


// standard keypad model:

// 7 8 9
// 4 5 6
// 1 2 3
// * 0 #

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
