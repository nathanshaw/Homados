/*
  Standard firmware for the Homados 16 channel solinoid driver
 The first 9 channels are PWM driven while the remaining 7 channels are digital
 
 When loading the firmware onto an Arduino Mega take note of
 the following steps:
 
 1. if you want the bot to silently boot (not trigger its solinoids) 
 set SILENT_BOOT to 1
 2. set LED_FEEDBACK to 1 if you want the LED's to provide runtime feedback
 3. Assign a new ARDUINO_ID: the Homados boards are 11 - 20
 
 */
// on H1 10, 12, 14, 16 are broken
# define NUM_SOLENOIDS 16
// botnum/board type/board num
#define BOT_ID 3
#define BOT_TYPE 2
#define ARDUINO_ID 1

#include <avr/interrupt.h>
#include <avr/io.h>

#define LED_POWER 11
#define LED_STATUS 12

#define LED_FEEDBACK 1
#define BOOT_TEST 1

char bytes[2];
short notes[NUM_SOLENOIDS];

int handshake = 0;
int statustimer = 0;

// actuator pins

int actuators[] = {
  10, 9, 8, 7, 6,
  5, 4, 3, 2, 22, 24, 26, 28, 30, 32, 34
};


void setup() {
  Serial.begin(57600);

  // interrupt timer parameters, these need to change
  TCCR2A = 1;
  TCCR2B = 3;
  TIMSK2 = 1;

  pinMode(LED_POWER, OUTPUT);
  pinMode(LED_STATUS, OUTPUT);
  digitalWrite(LED_STATUS, HIGH);
  digitalWrite(LED_POWER, HIGH);

  bootTest();
  
}

ISR(TIMER2_OVF_vect) {
  // solenoid control
  for (int i = 0; i < NUM_SOLENOIDS; i++) {
    if (notes[i] > 0) {
      digitalWrite(actuators[i], HIGH);
      notes[i]--;
    }
    else {
      digitalWrite(actuators[i], LOW);
    }
  }
  if (statustimer > 0 && LED_FEEDBACK) {
    digitalWrite(LED_STATUS, HIGH);
    statustimer--;
  }
  else {
    digitalWrite(LED_STATUS, LOW);
  }
}

void bootTest() {
  for (int i = 0; i < NUM_SOLENOIDS; i++) {
    pinMode(actuators[i], OUTPUT);
    if (BOOT_TEST == 1){
      notes[i] = 200;
      delay(350);  
    }
  }
  delay(2500); 
}

void loop() {
  bootTest();
  if (Serial.available()) {
    if (Serial.read() == 0xff) {
      // reads in a two index array from ChucK
      Serial.readBytes(bytes, 2);

      // bit wise operations
      // ~~~~~~~~~~~~~~~~~~~
      // reads the first six bits for the note number
      // then reads the last ten bits for the note velocity
      int pitch = byte(bytes[0]) >> 2;
      int velocity = (byte(bytes[0]) << 8 | byte(bytes[1])) & 1023;

      // message required for "handshake" to occur
      // happens once per Arduino at the start of the ChucK serial code
      if (pitch == 63 && velocity == 1023 && handshake == 0) {
        Serial.write(BOT_ID);
        Serial.write(BOT_TYPE);
        Serial.write(ARDUINO_ID);
        handshake = 1;
        digitalWrite(LED_POWER, LOW);
      }   
      if (pitch >= 0 && pitch <= NUM_SOLENOIDS) {
        statustimer = 120;
        notes[pitch] = (velocity * 0.5);
      }
    }
  }
}

