![Machine Orchesta](http://bitdeph.com/wp-content/uploads/2017/06/The_Pantheon_Shields_24.jpg)
# Homados

16 Channel Solenoid Driver

Please note that for each board you much change the ARDUINO_ID to a unique number

If you want to turn off LED feedback change LED_FEEDBACK to 0

If you don't want the startup sequence change BOOT_TEST to 0

If the LED is not yellow during the BOOT_TEST change your led pins until it is.

-------------------

Homados is the larger of the two actuator shields in the Pantheon system and is 
fashioned for use with the Arduino Mega microcontroller. The board supports 
sixteen channels of actuator control, three I2C jacks, and a RGB status LED. The 
first fifteen channels of the Homados driver utilizes PWM while the sixteenth channel 
uses a standard digital pin. The Homados is intended to function as a larger version of 
the Brigid shield and is to be used when six channels of control are inadequate. The 
Homados shield is functionally comparable to the Brigid in all ways.