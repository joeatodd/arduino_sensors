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
#include <avr/power.h>

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

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );

/* keypad.setHoldTime(); */
/* keypad.setDebounceTime(); */

ISR(PCINT1_vect){
  /* sleep_disable(); */
}

void presentation(){
  /* Send the sketch version information to the gateway */

#ifdef RADIO
  sendSketchInfo("RotaryPhone", "1.0");
  /* Register all sensors to gw (they will be created as child devices) */
  present(CHILD_ID_KEYS, S_INFO);
#endif
}

// Trying out basic sleep functionality
void goToSleep(){

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  PCIFR |= bit (PCIF1);
  PCICR |= bit (PCIE1);

  byte ADCSRA_prev = ADCSRA;
  ADCSRA = 0;
  power_all_disable();

  byte i;
  // Set colpins output low
  for (i = 0; i < cols; i++){
    pinMode(colPins[i], OUTPUT);
    digitalWrite(colPins[i], LOW);
  }

  // Set row pins input pullup
  for (i = 0; i < rows; i++){
    pinMode(rowPins[i], INPUT_PULLUP);
  }

  Serial.println("Yaaawn");
  Serial.flush();
  delay(50);
  sleep_cpu();

  sleep_disable();

  power_all_enable();
  ADCSRA = ADCSRA_prev;
  Serial.println("Big stretch!");
  Serial.flush();
}

void setup(){

#ifdef INO_DEBUG
  Serial.begin(9600);
#endif

  // Setup the button for the receiver cradle (child lock)
  pinMode(BUTTON_PIN, INPUT);
  digitalWrite(BUTTON_PIN, HIGH);

  /* PCMSK1 |= bit(PCINT10); */
  // A0,A1,A4,A5
  PCMSK1 = 0;
  PCMSK1 |= bit(PCINT8) | bit(PCINT9) | bit(PCINT12) | bit(PCINT13);
}

void loop(){

  Serial.println("Big stretch loop!");

  // Get the pressed key
  char key = keypad.getKey();
  // Check if the receiver is in the cradle
  bool receiver = (digitalRead(BUTTON_PIN) == HIGH);

  if (key != NO_KEY && receiver){

    delay(200);

#ifdef INO_DEBUG
    Serial.println(key);
    Serial.flush();
#endif

#ifdef RADIO
    // Tell homeassistant which key was pressed
    send(msgKey.set(key));
#endif

  }else{

    goToSleep();
  }
}
