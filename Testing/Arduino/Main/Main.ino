/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MPU6050_light.h>  //Include library for MPU communication
#include <ESP32Servo.h> 


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int trigPin = 5;
const int echoPin = 18;

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;

float accx;
float accy;
float accz;

float gyrox;
float gyroy;
float gyroz;

float gravvecttiltx;
float gravvecttilty;

float mputemp;

// throttle is connected to GPIO 34 (Analog ADC1_CH6) 
const int throttlepin = 34;

// throttle is connected to GPIO 35 (Analog ADC1_CH7) 
const int joystick_xpin = 35;

// Possible PWM GPIO pins on the ESP32: 0(used by on-board button),2,4,5(used by on-board LED),12-19,21-23,25-27,32-33 
const int brushlesspin = 19;  

// variable for storing the throttle value
float throttlevalue = 0;
float joystick_xvalue = 0;
float throttle_delta = 0;

int ADC_Max = 4096; // Maximum value returned by ADC

int idle_value_mapped = 90;   // starting angle of the brushless
int idle_value = 855;          // starting angle of the brushless
int brushless_power_val;    // variable to read the value from the analog pin
 

int menu_selection = 0;
int total_menu_items = 3;
float menu_timer = 0;

MPU6050 mpu(Wire);   //Create object mpu

Servo mybrushless;  // create servo object to control a brushless

unsigned long timer = 0;    


void setup() {
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

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  Wire.begin();     
  mpu.begin();     
  Serial.print(F("MPU6050 status: "));
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  
  //calculate MPU6050 offsets
  display.println("Calculating offsets, do not move MPU6050");
  display.display(); 
  Serial.println(F("Calculating offsets, do not move MPU6050"));   
  delay(1000);
  mpu.calcGyroOffsets();     //Calibrate gyroscope
  Serial.println("Done!\n");
  display.println("Done!");
  display.println("");
  display.println("Starting in a sec...");
  display.display(); 
  delay(1000);
 
}

void calibrate_sensors()  {
//ask user to leave throttle at idle and moniter the throttle min and max values in 5 seconds and print out a delta 
//calculate the delta and use that as the offset for the throttle
Serial.println("Calibrating throttle...");
Serial.println("Please leave throttle at idle for 5 seconds");
Serial.println("");
display.clearDisplay();
display.println("Calibrating throttle...");
display.println("Please leave throttle at idle for 5 seconds");
display.println("");
display.display();
delay(1000);
display.clearDisplay();
float throttlemax = analogRead(throttlepin);
float throttlemin = analogRead(throttlepin);
float timer = millis();
  
  while (millis() - timer < 5000) {
    display.clearDisplay();
    display.println(5000 - (millis() - timer));
    throttlevalue = analogRead(throttlepin);
    if (throttlevalue > throttlemax) {
      throttlemax = throttlevalue;
    }
    if (throttlevalue < throttlemin) {
      throttlemin = throttlevalue;
    }
  }

  throttle_delta = throttlemax - throttlemin;
  Serial.print("Throttle max: ");
  Serial.println(throttlemax);
  Serial.print("Throttle min: ");
  Serial.println(throttlemin);
  Serial.print("Throttle delta: ");
  Serial.println(throttle_delta);
  Serial.println("");
  Serial.println("Calibration complete");
  Serial.println("");
  display.clearDisplay();
  display.println("Calibration complete");
  display.println("");
  display.display();
  delay(1000);
  display.clearDisplay();
  display.println("throttle max: ");
  display.println(throttlemax);
  display.println("throttle min: ");
  display.println(throttlemin);
  display.println("throttle delta: ");
  display.println(throttle_delta);
  display.display();
  delay(2000);
  display.clearDisplay();
 
}


void loop() {
  
  if ((millis() - timer) > 10) { // print data every 100ms
    mpu.update();
    timer = millis();
    
    /////////gyro acc temp
    
    gyrox = mpu.getAngleX();
    gyroy = mpu.getAngleY();
    gyroz = mpu.getAngleZ();

    accx = mpu.getAccX();
    accy = mpu.getAccX();
    accz = mpu.getAccX();

    gravvecttiltx = mpu.getAccAngleX();
    gravvecttilty = mpu.getAccAngleY();
    
    mputemp = mpu.getTemp();

    
    //////////ultra sound dist
    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);   
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
   
    distanceCm = duration * SOUND_SPEED/2;
    distanceInch = distanceCm * CM_TO_INCH;

    /////joystick
    
    throttlevalue = analogRead(throttlepin);
    
    joystick_xvalue = analogRead(joystick_xpin);
    
    
    //////serial print
    

    Serial.print("GX: ");
    Serial.print(gyrox);
    Serial.print(" ");//Print Z angle value on LCD 
    Serial.print("GY: ");
    Serial.print(gyroy);
    Serial.print(" ");//Print Z angle value on LCD 
    Serial.print("GZ: ");
    Serial.print(gyroz);
    Serial.print(" ");
    Serial.print("AX: ");
    Serial.print(accx);
    Serial.print(" ");//Print Z angle value on LCD 
    Serial.print("AY: ");
    Serial.print(accy);
    Serial.print(" ");//Print Z angle value on LCD 
    Serial.print("AZ: ");
    Serial.print(accz);
    Serial.print(" ");
    Serial.print("GvectX: ");
    Serial.print(gravvecttiltx);
    Serial.print(" ");//Print Z angle value on LCD 
    Serial.print("GvectY: ");
    Serial.print(gravvecttilty);
    Serial.print(" ");
    Serial.print("temp: ");
    Serial.print(mputemp); 
    Serial.print(" ");  
    Serial.print("JoyX: ");
    Serial.print(throttlevalue);
    Serial.print(" ");
    Serial.print("Distance(cm): ");
    Serial.print(distanceCm);
    Serial.print(" ");
    Serial.print("Distance(inch): ");
    Serial.print(distanceInch);
    Serial.println(" ");//Print Z angle value on LCD 
    
    //if joystick x value is greater that 3600 for 500ms
    
    if (joystick_xvalue < 3400) {
      menu_timer = millis();
    }

    if (joystick_xvalue >= 3400) {
      if (millis() - menu_timer > 500) {
        menu_timer = millis();
        menu_selection++;
        if (menu_selection > total_menu_items) {
          menu_selection = 0;
        }
      }
    }
    
    
    


    //if(menu_selection > total_menu_items) {
      //menu_selection = 0;
    //}
    
    if(menu_selection == 0) {
    //arm motors
    brushless_power_val = map(throttlevalue, idle_value, ADC_Max, idle_value_mapped, 180);     // scale it to use it with the brushless (value between 0 and 180)
    mybrushless.write(brushless_power_val);  // set the brushless position according to the scaled value
    Serial.println("Potentiometer value: ");
    Serial.println(throttlevalue);   // print the value to the serial monitor
    Serial.println("Mapped value: ");
    Serial.println(brushless_power_val);
    //////display
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    // Display static text
    display.println("motor armed");
    display.setCursor(0, 20);
    display.println("throttle value: ");
    display.println(throttlevalue);
    display.setCursor(0, 40);
    display.println("mapped value: ");
    display.println(brushless_power_val);
    display.display();

    }


    if(menu_selection == 1) {
    //calibrate throttle
    //////display
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    // Display static text
    display.println("do u want to calibrate throttle?");
    display.setCursor(0, 20);
    display.println("1. yes");
    display.setCursor(0, 40);
    display.println("2. no");
    display.display();

    
    }


    if(menu_selection == 2) {
    //sensor output screen
    //////display
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    // Display static text
    display.println(String(throttlevalue));

    display.setCursor(75, 0);
    // Display static text
    display.println(String(distanceCm) + "cm");

    display.setCursor(0, 20);
    // Display static text
    display.println(joystick_xvalue);
    display.display(); 
    //delay(100);
    }

  
}
}