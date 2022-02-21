
#include <ESP32Servo.h> 

Servo mybrushless;  // create servo object to control a brushless

// Possible PWM GPIO pins on the ESP32: 0(used by on-board button),2,4,5(used by on-board LED),12-19,21-23,25-27,32-33 
int brushlesspin = 19;      // GPIO pin used to connect the brushless control (digital out)
// Possible ADC pins on the ESP32: 0,2,4,12-15,32-39; 34-39 are recommended for analog input
int throttlepin = 34;        // GPIO pin used to connect the potentiometer (analog in)
int ADC_Max = 4096;     // This is the default ADC max value on the ESP32 (12 bit ADC width);
                        // this width can be set (in low-level oode) from 9-12 bits, for a
                        // a range of max values of 512-4096

int idle_value_mapped = 90;   // starting angle of the brushless
int idle_value = 855;          // starting angle of the brushless
int brushless_power_val;    // variable to read the value from the analog pin
float throttlevalue;        // variable to store the value read from the throttle

void setup()
{
    Serial.begin(115200);
	// Allow allocation of all timers
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
  mybrushless.setPeriodHertz(50);// Standard 50hz brushless
  mybrushless.attach(brushlesspin, 500, 2400);   // attaches the brushless on pin 18 to the servo object
                                         // using SG90 brushless min/max of 500us and 2400us
                                         // for MG995 large brushless, use 1000us and 2000us,
                                         // which are the defaults, so this line could be
                                         // "mybrushless.attach(brushlessPin);"
}

void loop() {
  throttlevalue = analogRead(throttlepin);            // read the value of the potentiometer (value between 0 and 1023)
  brushless_power_val = map(throttlevalue, idle_value, ADC_Max, idle_value_mapped, 180);     // scale it to use it with the brushless (value between 0 and 180)
  mybrushless.write(brushless_power_val);  // set the brushless position according to the scaled value
  Serial.println("Potentiometer value: ");
  Serial.println(throttlevalue);   // print the value to the serial monitor
  Serial.println("Mapped value: ");
  Serial.println(brushless_power_val);               
  delay(200);                          // wait for the brushless to get there
}
