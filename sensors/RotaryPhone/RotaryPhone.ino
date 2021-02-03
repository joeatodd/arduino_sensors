
//#define RADIO
#define INO_DEBUG

#ifdef RADIO
#define MY_DEBUG
#define MY_RADIO_RF24

#define MY_NODE_ID 5

#define CHILD_ID_KEYS 0

#include <SPI.h>
#include <MySensors.h>
#endif

#include <Keypad.h>

#define BUTTON_PIN A2

// 4x4 keypad matrix (one whole row is just the central button)
const byte rows = 4;
const byte cols = 4;

char test_msg[3] = "01";


#ifdef RADIO
MyMessage msgButt(CHILD_ID_KEYS, V_TEXT);
#endif

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

void presentation(){
  /* Send the sketch version information to the gateway */

  #ifdef RADIO
  sendSketchInfo("RotaryPhone", "1.0");
  /* Register all sensors to gw (they will be created as child devices) */
  present(CHILD_ID_KEYS, S_INFO);
  #endif
}

void setup(){

#ifdef INO_DEBUG
  Serial.begin(9600);
#endif

// Setup the button
pinMode(BUTTON_PIN, INPUT);
// Activate internal pull-up
digitalWrite(BUTTON_PIN, HIGH);

}

void loop(){
  /* Serial.println("HELLO"); */
  char key = keypad.getKey();
  bool receiver = (digitalRead(BUTTON_PIN) == HIGH);
  Serial.println(receiver);

  if (key != NO_KEY){// && digitalRead(BUTTON_PIN) == HIGH){
    Serial.println(key);
  }

}
