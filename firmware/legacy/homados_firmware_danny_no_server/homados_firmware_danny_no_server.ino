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

// min amount of ms between actuator triggers
int cooldown = 500;
//actuator pins v2
int actuators[] = {
  11, 12, 13, 10, 9, 8, 7, 6, 5, 4, 3, 2,
  44, 45, 46, 47
};

int last_triggered[NUM_SOLENOIDS];

void setup() {
  // interrupt timer parameters, these need to change
  TCCR2A = 1;
  TCCR2B = 3;
  TIMSK2 = 1;

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_RED, HIGH);
  for (int i = 0; i < NUM_SOLENOIDS; i++) {
    pinMode(actuators[i], OUTPUT);
    if (BOOT_TEST == 1){
      notes[i] = 100;
      delay(100);  
    }
  }
  delay(1000);
  digitalWrite(LED_RED, LOW);
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
    digitalWrite(LED_BLUE, HIGH);
    statustimer--;
  }
  else {
    digitalWrite(LED_BLUE, LOW);
  }
}


void loop() {
  statustimer = 120;
  for (int i = 0; i < NUM_SOLENOIDS; i++){
    // this will activate the actuator 5% of the time
    // but allows for multiple ones to be activated at once
    if (random(0, 100) < 5){
      // what you make notes[i] equal to
      // corresponds to velocity, or time on 
      // where i is the number of actuator you 
      // are addressing

      // if an amount of ms greater than cooldown has passed
      // from the last time the actuator was triggered...
      // activate it again
      if (last_triggered[i] + cooldown < millis()){
        notes[i] = 100;
        // this allows you to keep track of the last time 
        // the actuator was triggered
        last_triggered[i] = millis();
      }
    }
  }
}




