//#define RADIO
#define INO_DEBUG

#ifdef RADIO
#define MY_DEBUG
#define MY_RADIO_NRF24

#define MY_NODE_ID 5

#define CHILD_ID_KEYS 0

#include <SPI.h>
#include <MySensors.h>
#endif

#include <Keypad.h>
#include <avr/sleep.h>

#define BUTTON_PIN A2

// 4x4 keypad matrix (one whole row is just the central button)
const byte rows = 4;
const byte cols = 4;

#ifdef RADIO
MyMessage msgKey(CHILD_ID_KEYS, V_TEXT);
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

void wakeUp(){
  sleep_disable();
  detachInterrupt(0);
}

// Trying out basic sleep functionality
void goToSleep(){

  sleep_enable();
  attachInterrupt(0, wakeUp, LOW);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  Serial.println("Yaaawn");
  delay(200);
  sleep_cpu();
  Serial.println("Big stretch!");
}

void setup(){

#ifdef INO_DEBUG
  Serial.begin(9600);
#endif

// Setup the button for the receiver cradle (child lock)
pinMode(BUTTON_PIN, INPUT);
digitalWrite(BUTTON_PIN, HIGH);

}

void loop(){

  goToSleep();
  // Get the pressed key
  char key = keypad.getKey();
  // Check if the receiver is in the cradle
  bool receiver = (digitalRead(BUTTON_PIN) == HIGH);

  if (key != NO_KEY && receiver){

#ifdef INO_DEBUG
    Serial.println(key);
#endif

#ifdef RADIO
    // Tell homeassistant which key was pressed
    send(msgKey.set(key));
#endif
  }

}
