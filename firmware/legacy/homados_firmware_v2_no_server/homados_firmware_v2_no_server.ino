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

#include <avr/interrupt.h>
#include <avr/io.h>

#define LED_RED 41
#define LED_GREEN 40
#define LED_BLUE 42

#define LED_FEEDBACK 1
#define BOOT_TEST 1

char bytes[2];
short notes[NUM_SOLENOIDS];

int handshake = 0;
int statustimer = 0;
//actuator pins v2
int actuators[] = {
  11, 12, 13, 10, 9, 8, 7, 6, 5, 4, 3, 2,
  44, 45, 46, 47
};

void setup() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_RED, HIGH);
  for (int i = 0; i < NUM_SOLENOIDS; i++) {
    pinMode(actuators[i], OUTPUT);
    if (BOOT_TEST == 1){
      notes[i] = (100);
      delay(100);  
    }
  }
  delay(1000);
  digitalWrite(LED_RED, LOW);
}

//1, 2 acting funky
void loop() {
  for (int i = 0; i < NUM_SOLENOIDS; i++){
    digitalWrite(actuators[i], HIGH);
    delay(10);
    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(actuators[i], LOW);
    digitalWrite(LED_BLUE, LOW);
    delay(200); 
  }
}



