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
#define I2C_S
#include <Wire.h>
#ifdef I2C_S
#define I2C_ID 0x90
#endif

// #define USB_SERIAL
// on H1 10, 12, 14, 16 are broken
#define NUM_SOLENOIDS 16

// botnum/board type/board num
#define PANTHEON
#ifdef PANTHEON
#define BOT_ID 3
#define BOT_TYPE 2
#define ARDUINO_ID 3
#endif

#include <avr/interrupt.h>
#include <avr/io.h>

#define LED_BLUE 42
#define LED_GREEN 40
#define LED_RED 41

#define LED_FEEDBACK 1
#define BOOT_TEST 0

// Function declarations
///////////////////////////////////////
void bootTest();
void pollserial();
void i2cRequest();
void i2cReceive(unsigned int count);

// Global Variables
////////////////////////////////////////
char bytes[2];
short notes[NUM_SOLENOIDS];

int handshake = 0;
int statustimer = 0;

// actuator pins for rev 2
int actuators[] = {
  11, 12, 13, 10,
  9, 44, 7, 6,
  5, 4, 3, 2,
  45, 46, 47, 8
};

///////////// LEDs ///////////////////////////////
char led_pins[] = {LED_RED, LED_GREEN, LED_BLUE};
unsigned long led_last_on[] = {0, 0, 0};
bool led_state[] = {0, 0, 0};

//////////// I2C /////////////////////////////////
#define MEM_LEN 256
char databuf[MEM_LEN];
volatile uint8_t received;

void setup() {
  Serial.begin(57600);

  // interrupt timer parameters, these need to change
  TCCR2A = 1;
  TCCR2B = 3;
  TIMSK2 = 1;

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  // While booting the LED is Yellow
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, LOW);
  Serial.println("Running boot tests");
  bootTest();

  // When boot is over the board will give you a green LED if Mode is Serial
#ifdef USB_SERIAL
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_RED, LOW);
  Serial.println("USB_SERIAL mode is active, the board will expect commands via usb");
#endif

#ifdef I2C_S
  // Wire.begin(I2C_SLAVE, I2C_ID, I2C_PINS_18_19, I2C_PULLUP_EXT, 400000);
  Wire.begin(I2C_ID); // joing i2c bus with address of I2C_ID
  Wire.onReceive(i2cReceive);
  // Wire.onRequest(i2cRequest);
  // data init
  // received = 0;
  // memset(databuf, 0, sizeof(databuf));

  digitalWrite(LED_BLUE, HIGH);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);
  Serial.print("I2C mode is active, the board will act as a slave with address");
  Serial.println(I2C_ID);
#endif
  Serial.println("setup loop complete");
  Serial.println("- - - - - - - - - - - -");
}

void loop() {
#ifdef USB_SERIAL
  pollSerial();
#endif
}

void i2cReceive(unsigned int count) {
  // should be the note followed by a velocity
  while (1 < Wire.available()) {
    int n = Wire.read();
    int v = Wire.read();
    statustimer = 60;
    notes[n] = v;
  }
  // Wire.read(databuf, count);
  // received = count;
  // digitalWrite(LED_RED, HIGH);
  // digitalWrite(LED_GREEN, HIGH);
  // digitalWrite(LED_BLUE, HIGH);
  Serial.println("Received I2C Message");
}

void pollSerial() {
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
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GREEN, LOW);
      }
      if (pitch >= 0 && pitch <= NUM_SOLENOIDS) {
        statustimer = 120;
        notes[pitch] = (velocity * 0.5);
      }
    }
  }
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

void bootTest() {
  for (int i = 0; i < 12; i++) {
    pinMode(actuators[i], OUTPUT);
    if (BOOT_TEST == 1) {
      notes[i] = 70;
      delay(350);
    }
  }
  delay(1500);
}
