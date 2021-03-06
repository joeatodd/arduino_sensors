
#define RADIO
//#define INO_DEBUG

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

String status;

#ifdef RADIO
MyMessage msgKey(CHILD_ID_KEYS, V_TEXT);
#endif

char keys[rows][cols] = {{'*','7','4','1'},  // 7
			 {'0','8','5','2'},  // A1
			 {'#','9','6','3'},  // A4
			 {'R','R','R','R'}}; // A5 // this whole row is just the reset button
                        // 2   3   5   6

/* Input row 0 (7) is low for every column if any of the keys are pressed.

   It's nothing to do with sleep - confirmed by disabling the sleepy
   However, when I don't even enter the goToSleep() function, the problem does get
   better, though it still *sometimes* occurs on pins 7 & *...

   Dodgy wiring?

   Yes, hardware problem, switched pin A0 to 7
*/

// Had to work out these pins by trial and error...
byte rowPins[rows] = {7,A1,A4,A5};
byte colPins[cols] = {2,3,5,6};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );

/* keypad.setHoldTime(); */
/* keypad.setDebounceTime(); */

ISR(PCINT1_vect){
  PCICR = 0;
}
ISR(PCINT2_vect){
  PCICR = 0;
}


void colPinsHigh(){
  // Set col pins to input high as per Keypad::scanKeys
  for (byte i=0; i < cols; i++){
    pinMode(colPins[i], INPUT);
    digitalWrite(colPins[i], HIGH);
  }
}

void colPinsLow(){
  // Set colpins output low
  for (byte i = 0; i < cols; i++){
    pinMode(colPins[i], OUTPUT);
    digitalWrite(colPins[i], LOW);
  }
}

void rowPinsHigh(){
  // Set row pins input pullup
  // This is also what KeyPad expects
  for (byte i = 0; i < rows; i++){
    pinMode(rowPins[i], INPUT_PULLUP);
  }
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

  colPinsLow();
  rowPinsHigh();

  // Check any buttons currently pressed
  // Note this shouldn't be necessary any more as we waitForRelease()
  for (byte i = 0; i < rows; i++){
    if (digitalRead(rowPins[i]) == LOW){
      Serial.println("Button still pressed");

      colPinsHigh();
      return;
    }
  }

  Serial.println("Yaaawn");
  Serial.flush();

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  byte ADCSRA_prev = ADCSRA;
  ADCSRA = 0;
  power_all_disable();

  PCIFR |= bit (PCIF1) | bit (PCIF2);	// Clear interrupt flag
  PCICR |= bit (PCIE1) | bit (PCIE2);  // Enable interrupt on relevant pins

  sleep_cpu();

  // zzzz...

  sleep_disable();

  power_all_enable();
  ADCSRA = ADCSRA_prev;
  Serial.println("Big stretch!");

  colPinsHigh();
  rowPinsHigh();
}

void setup(){

#ifdef INO_DEBUG
  Serial.begin(9600);
  Serial.println("**** Booting ****");
#endif

  // Setup the button for the receiver cradle (child lock)
  pinMode(BUTTON_PIN, INPUT);
  digitalWrite(BUTTON_PIN, HIGH);

  // Enable interrupt only on pins 7,A1,A4,A5
  PCMSK1 = bit(PCINT9) | bit(PCINT12) | bit(PCINT13);
  PCMSK2 = bit(PCINT23);
}

char getKeyPress(){
  // Use keypad.getKeys() and return first newly 'PRESSED' key
  // The logic of keypad.getKey() isn't great for sleeping, I don't think
  if (keypad.getKeys()){
    for (int i=0; i < LIST_MAX; i++){
      if((keypad.key[i].stateChanged) && (keypad.key[i].kstate == PRESSED)){
	return keypad.key[i].kchar;
      }
    }
    return NO_KEY;

  }else{
    return NO_KEY;
  }
}

void waitForRelease(){
  // Update the keypad every 100ms until all released (then we can sleep)
  bool anyPressed;
  do
    {
      anyPressed = false;

      keypad.getKeys();
      for (int i=0; i < LIST_MAX; i++){
	if((keypad.key[i].kstate == PRESSED) || (keypad.key[i].kstate == HOLD)){
	  anyPressed = true;
	  break;
	}
      }
      delay(100);
    }while(anyPressed);
}

void padStatus(){
  for (byte i = 0; i < LIST_MAX; i++){
    if (keypad.key[i].kchar == '\0'){
      continue;
    }
    if(keypad.key[i].kstate == IDLE){continue;}

    switch (keypad.key[i].kstate){
    case PRESSED:
      status = "PRESSED";
      break;
    case HOLD:
      status = "HOLD";
      break;
    case RELEASED:
      status = "RELEASED";
      break;
    }

    Serial.print("Key: ");
    Serial.print(keypad.key[i].kchar);
    Serial.print(" ");
    Serial.print(status);
    Serial.print(" changed: ");
    Serial.println(keypad.key[i].stateChanged);
  }
}

void loop(){

  // Get the pressed key
  /* char key = keypad.getKey(); */
  char key = getKeyPress();
  // Check if the receiver is in the cradle
  bool receiver = (digitalRead(BUTTON_PIN) == HIGH);

  if (key != NO_KEY && receiver){


#ifdef INO_DEBUG
    Serial.print("**** Keypad registered: ");
    Serial.print(key);
    Serial.println(" ****");

    padStatus();
#endif

#ifdef RADIO
    // Tell homeassistant which key was pressed
    send(msgKey.set(key));
#endif

    waitForRelease();

  }else{ // NO_KEY


#ifdef INO_DEBUG
    Serial.println("No button");
    padStatus();
#endif

    waitForRelease();
    goToSleep();
  }
}

