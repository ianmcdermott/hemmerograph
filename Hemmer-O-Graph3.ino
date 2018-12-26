
// Original Code by Andy
//https://www.stuffandymakes.com/blog/2015/4/28/charlieplexing-more-leds-20-to-be-exact

// Made for common-anode tri-color LEDs

// pin defines
#define A 5
#define B 6
#define C 9
#define D 10
#define E 11

#define PIN_COUNT 5

#define PIN_CONFIG 0
#define PIN_STATE 1

#define LED_COUNT 20

#include <Wire.h>
#include <AccelStepper.h>
#include <Adafruit_MotorShield.h>
#include <LED.h>
#include <Charlieplex.h>

#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>
int person2ArrayCount = 0;

PulseSensorPlayground pulseSensor;

long unsigned int lcm;

//Adafruit_MotorShield AFMSbot(0x61); // Rightmost jumper closed
Adafruit_MotorShield AFMStop(0x60); // Default address, no jumpers

// Connect two steppers with 200 steps per revolution (1.8 degree)
// to the top shield
Adafruit_StepperMotor *myStepper1 = AFMStop.getStepper(200, 1);
Adafruit_StepperMotor *myStepper2 = AFMStop.getStepper(200, 2);

const int OUTPUT_TYPE = SERIAL_PLOTTER;

//Pulse sensor variables
const int PULSE_INPUT = A0;
const int THRESHOLD = 550;   // Adjust this number to avoid noise when idle
int rotateDelay = 25;

bool hitBeat1;
int greaterBPM;
const int MODE_SWITCH = 2;
const int PERSON_SWITCH_1 = 3;
const int PERSON_SWITCH_1_INPUT = 4;
const int PERSON_SWITCH_2 = 7;
const int PERSON_SWITCH_2_INPUT = 8;

bool pulseMode;
bool drawingMode;
bool bpmIndex = false;
bool previousDrawingMode1 = false;
bool previousDrawingMode2 = false;
bool previousPulseMode1 = false;
bool     previousMode = false;
long stepperbpm1, stepperbpm2;
// LED INDICATORS ///
unsigned long currentMillis;

// ***** Charlieplex Data ***** //
#define NUMBER_OF_PINS 5
//define pins in the order you want to adress them
byte pins[] = {5, 6, 9, 10, 11};

int currentLed1 = 0;
int currentLed2 = 2;

//initialize object
Charlieplex charlieplex = Charlieplex(pins, NUMBER_OF_PINS, 0);

charliePin leds[] = {
  { 0 , 1 }, { 0 , 2 }, { 0 , 3 }, { 0 , 4 },
  { 1 , 2 }, { 1 , 0 }, { 1 , 3 }, { 1 , 4 },
  { 2 , 0 }, { 2 , 1 }, { 2 , 3 }, { 2 , 4 },
  { 3, 0 }, { 3 , 1 }, { 3 , 2 }, { 3 , 4 },
  { 4 , 0 }, { 4 , 1 }, { 4 , 2}, { 4 , 3 }
};

int person1Array[]  = {2, 3, 6, 7, 8, 9, 4, 5};
int person2Array[]  =  {0, 1, 14, 15, 18, 19, 12, 13}; //{1, 5, 18, 19, 7, 13};//

int pulseModeCount = 0;
int pulseVal ;
int  mappedPulseVal;
int Signal;

const int PWM1 = 5;
const int PWM2 = 6;
const int PWM3 = 9;
const int PWM4 = 10;
const int PWM5 = 11;
const long interval = 10;

long unsigned int lockedBeat1 = 0;
long unsigned int  lockedBeat2 = 0;

// you can change these to DOUBLE or INTERLEAVE or MICROSTEP!
// wrappers for the first motor!
void forwardstep1() {
  myStepper1->onestep(FORWARD, DOUBLE);
}
void backwardstep1() {
  myStepper1->onestep(BACKWARD, DOUBLE);
}
// wrappers for the second motor!
void forwardstep2() {
  myStepper2->onestep(FORWARD, DOUBLE);
}
void backwardstep2() {
  myStepper2->onestep(BACKWARD, DOUBLE);
}

bool i = false;

int bpm1 ;
int bpm2 ;

// Now we'll wrap the 3 steppers in an AccelStepper object
AccelStepper stepper1(forwardstep1, backwardstep1);
AccelStepper stepper2(forwardstep2, backwardstep2);

// variables to allow leds to blink without delays
unsigned long previousMillis = 0;
unsigned long previousMillisL1 = 0;
unsigned long previousMillisL2 = 0;

int blinkCount = 0;


void setup()
{
  Serial.begin(115200);
  checkMode();

  pinMode(MODE_SWITCH, INPUT);
  pinMode(PERSON_SWITCH_1, OUTPUT);
  pinMode(PERSON_SWITCH_1_INPUT, INPUT);
  pinMode(PERSON_SWITCH_2, OUTPUT);
  pinMode(PERSON_SWITCH_2_INPUT, INPUT);

  //  AFMSbot.begin(); // Start the bottom shield
  AFMStop.begin(); // Start the top shield
  int bpm1 = 50;
  int bpm2 = 50;

  // Configure the PulseSensor manager.
  pulseSensor.analogInput(PULSE_INPUT);
  pulseSensor.setSerial(Serial);
  pulseSensor.setOutputType(OUTPUT_TYPE);
  pulseSensor.setThreshold(THRESHOLD);

  if (!pulseSensor.begin()) {
    /*
       PulseSensor initialization failed,
       likely because our particular Arduino platform interrupts
       aren't supported yet.

       If your Sketch hangs here, try PulseSensor_BPM_Alternative.ino,
       which doesn't use interrupts.
    */
    for (;;) {
      // Flash the led to show things didn't work.
      digitalWrite(13, LOW);
      delay(50);
      digitalWrite(13, HIGH);
      delay(50);
    }
  }

  digitalWrite(PERSON_SWITCH_1, HIGH);
  digitalWrite(PERSON_SWITCH_2, HIGH);
  checkMode();
  checkPerson();
}

void loop() {

  checkMode();
  checkPerson();
  pulseVal = pulseSensor.getLatestSample();
  mappedPulseVal = map(pulseVal, 0, THRESHOLD, 0, 255);
  //  If middle switch is up/unlit, start drawing
  if (drawingMode) {
    if (bpm1 != 0 && bpm2 != 0) {
      Serial.println("drawMode");
      drawImage();
      drawnBeat(person1Array, lockedBeat1, previousMillisL1);
      drawnBeat(person2Array, lockedBeat2, previousMillisL2);
    }
  }

  if (pulseMode) {
    Signal = analogRead(PULSE_INPUT);

    if (!bpmIndex && !digitalRead(PERSON_SWITCH_1_INPUT)) {
      //      Serial.println("P1");
      if (!digitalRead(PERSON_SWITCH_2_INPUT)) {
        charlieplex.charlieWrite(leds[16], 255, 16);
        charlieplex.charlieWrite(leds[17], 255, 17);
        charlieplex.clear();
      }

      if (Signal > 30) {
        charlieplex.charlieWrite(leds[10], map(Signal, 0, THRESHOLD, 0, 255), 10);
        charlieplex.clear();
      }

      if (Signal > THRESHOLD) {                        // If the signal is above "550", then "turn-on" Arduino's on-Board LED.
        lightBeat(person1Array);
      } else {
        charlieplex.charlieWrite(leds[11], 255, 11);
        charlieplex.clear();
      }

      bpm1 = int(pulseSensor.getBeatsPerMinute());  // Calls function on our pulseSensor object that returns BPM as an "int".
      lockedBeat1 = int(60.0 / float(bpm1) * 1000.0); //Convert the bpm1 to seconds, multiply by 1000 to do a module equation with the elapsed milliseconds
    } else if (bpmIndex && digitalRead(PERSON_SWITCH_1_INPUT) && !digitalRead(PERSON_SWITCH_2_INPUT)) {
      if (!digitalRead(PERSON_SWITCH_1_INPUT)) charlieplex.charlieWrite(leds[10], 255, 10);

      if (Signal > 30) {
        charlieplex.charlieWrite(leds[17], map(Signal, 0, THRESHOLD, 0, 255), 17);
        charlieplex.clear();
      }

      if (Signal > THRESHOLD) {                        // If the signal is above "550", then "turn-on" Arduino's on-Board LED.
        lightBeat(person2Array);
      } else {
        charlieplex.charlieWrite(leds[16], 255, 16);
        charlieplex.clear();
      }

      bpm2 = int(pulseSensor.getBeatsPerMinute());  // Calls function on our pulseSensor object that returns BPM as an "int".
      lockedBeat2 = int(60.0 / float(bpm2) * 1000.0);
    }
  }
  updateRPM();
  playLockedBeats();
}

void beat(int currentLED, int * personArray) {
  charlieplex.charlieWrite(leds[personArray[currentLED]], 255, currentLED);
  // Clear pin immediately after so it doesn't interfere with others
  charlieplex.clear();
}

void playLockedBeats() {
  if (digitalRead(PERSON_SWITCH_1_INPUT)) {
    charlieplex.charlieWrite(leds[10], 255, 10);

    if (millis() % lockedBeat1 == 0) {
      for (int i = 0; i < sizeof(person1Array) / 2; i += 2) {
        int index = person1Array[i];
        charlieplex.charlieWrite(leds[ i], 255, i);
      }
      charlieplex.clear();
    } else {
      //      if (currentLed1 % 2 == 0) {
      //        beat(currentLed1, person1Array);
      //      }
    }
  }

  if (digitalRead(PERSON_SWITCH_2_INPUT)) {
    charlieplex.charlieWrite(leds[17], 255, 17);

    if (millis() % lockedBeat2 == 0) {
      //      if (currentLed2 % 2 == 0) {
      //        beat(2, person2Array);
      //      }
      //      currentLed2++;
      for (int i = 0; i < sizeof(person2Array) / 2; i += 2) {
        int index = person2Array[i];
        charlieplex.charlieWrite(leds[ i], 255, i);
      }
      charlieplex.clear();
    } else {

    }
  }

  if (currentLed1 > sizeof(person1Array) / 2 ) {
    currentLed1 = 0;
  }
  if (currentLed2 > sizeof(person2Array) / 2 ) {
    currentLed2 = 0;
  }
}


void lightBeat(int personArray[]) {
  charlieplex.clear();

  int mycount = 20;
  for (int i = 1; i < sizeof(person1Array) / 2; i += 2) {
    //    if (i  %  2 == 1) {
    int index = personArray[i];
    charlieplex.charlieWrite(leds[index], 255, index);
    charlieplex.clear();
    //    }
  }
}




void drawnBeat(int personArray[], long unsigned int lockedBeat, long unsigned int mills) {
  for (int i = 0; i < sizeof(person1Array) / 2; i++) {
    int index = personArray[i];
    if (currentMillis - mills >= interval * i) {
      charlieplex.charlieWrite(leds[index], 255, index);
      delay(10);
      charlieplex.clear();
    } else {
      charlieplex.charlieWrite(leds[index], 0, index);
    }
  }
}


void drawImage() {
  //    stepper1.setAcceleration(60 / 10);
  //    stepper1.setSpeed(11760);
  if (stepper1.distanceToGo() <= 0) stepper1.setCurrentPosition(0);
  if (stepper2.distanceToGo() <= 0) stepper2.setCurrentPosition(0);

  stepper1.run();
  stepper2.run();

  Serial.print("LCM: "); Serial.print(lcm); Serial.print("   Stepper 1 RPM: "); Serial.print(stepperbpm1); Serial.print(" ||| "); Serial.print("Dist to go: "); Serial.print(stepper1.distanceToGo());
  Serial.print("  Stepper 2 RPM: "); Serial.print(stepperbpm2); Serial.print(" ||| "); Serial.print("Dist to go: "); Serial.println(stepper2 .distanceToGo());
}

void updateRPM() {
  // Figure out which BPM is greater
  if (bpm1 > bpm2) greaterBPM = stepperbpm1;
  else greaterBPM = stepperbpm2;
  stepperbpm1 = map(bpm1, 0, greaterBPM, 0, 1000);
  stepperbpm2 = map(bpm2, 0, greaterBPM, 0, 1000);

  // Find Lowest common multiple to get stopping point
  lcm = getLCM( bpm1,  bpm2) * 200;

  stepper1.setAcceleration(stepperbpm1 );
  stepper1.setMaxSpeed(stepperbpm1);
  stepper1.moveTo(lcm);
  stepper1.setSpeed(bpm1 );

  //  delay(1);

  //set max speed to bpm since speed is set to rpm
  stepper2.setAcceleration(stepperbpm2);
  stepper2.setMaxSpeed(stepperbpm2 );
  stepper2.moveTo(lcm);
  stepper2.setSpeed(bpm2);

  //  delay(1);
}

