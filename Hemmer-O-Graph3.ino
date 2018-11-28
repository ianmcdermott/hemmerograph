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

#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>

PulseSensorPlayground pulseSensor;

int lcm;

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
float pulseVal;
bool hitBeat1;

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
long newBpm0, newBpm1;
// LED INDICATORS ///
unsigned long currentMillis;

int pulseModeCount = 0;

const int PWM1 = 5;
const int PWM2 = 6;
const int PWM3 = 9;
const int PWM4 = 10;
const int PWM5 = 11;

long unsigned int lockedBeat1, lockedBeat2;

// you can change these to DOUBLE or INTERLEAVE or MICROSTEP!
// wrappers for the first motor!
void forwardstep1() {
  myStepper1->onestep(FORWARD, SINGLE);
}
void backwardstep1() {
  myStepper1->onestep(BACKWARD, SINGLE);
}
// wrappers for the second motor!
void forwardstep2() {
  myStepper2->onestep(FORWARD, DOUBLE);
}
void backwardstep2() {
  myStepper2->onestep(BACKWARD, DOUBLE);
}

bool i = false;

int bpm0 ;
int bpm1 ;

// Now we'll wrap the 3 steppers in an AccelStepper object
AccelStepper stepper1(forwardstep1, backwardstep1);
AccelStepper stepper2(forwardstep2, backwardstep2);

// variables to allow leds to blink without delays
unsigned long previousMillis = 0;
unsigned long previousMillisL1 = 0;
unsigned long previousMillisL2 = 0;

const long interval = 50;
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
  int bpm0 = 240;
  int bpm1 = 120;
  //get least common multiple of bpms to calculate moveto
  newBpm0 = bpm0 * 200L * 60L;
  newBpm1 = bpm1 * 200L * 60L;
  lcm = getLCM(bpm0, bpm1);

  stepper1.setMaxSpeed(newBpm0);
  stepper1.setAcceleration(newBpm0);
  stepper1.moveTo(50000);
  delay(5);


  stepper2.setMaxSpeed(newBpm1);
  stepper2.setAcceleration(newBpm1);
  stepper2.moveTo(50000 );


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
      //      // Flash the led to show things didn't work.
      //      digitalWrite(PULSE_BLINK, LOW);
      //      delay(50);
      //      digitalWrite(PULSE_BLINK, HIGH);
      //      delay(50);
    }
  }

  //  }

  digitalWrite(PERSON_SWITCH_1, HIGH);
  digitalWrite(PERSON_SWITCH_2, HIGH);

}

void loop() {
  currentMillis = millis();



  //  delay(20);
  //  pulseSensor.outputSample();
  pulseVal = pulseSensor.getLatestSample();
  // Reset the current position to 0 to start a new drawing
  // Check if we're in Pulse Mode or Drawing Mode
  checkMode();
  checkPerson();

  if (drawingMode) {
    if (bpm0 != 0 && bpm1 != 0) {
      if (newBpm0 != bpm0 || newBpm1 != bpm1) {
        updateRPM();
      }
      drawImage();
    }
  }

  if (pulseMode) {
    if (!bpmIndex && !digitalRead(PERSON_SWITCH_1_INPUT)) {
      if (pulseSensor.sawStartOfBeat() ) {
        previousMillis = currentMillis;
      }
      bpm0 = int(pulseSensor.getBeatsPerMinute());  // Calls function on our pulseSensor object that returns BPM as an "int".
      lightBeat(0, 8, false) ;
      lockedBeat1 = bpm0 / 60 * 1000;


    } else  if (bpmIndex && !digitalRead(PERSON_SWITCH_2_INPUT)) {
      if (pulseSensor.sawStartOfBeat() ) {
        previousMillis = currentMillis;
      }
      bpm1 = int(pulseSensor.getBeatsPerMinute());  // Calls function on our pulseSensor object that returns BPM as an "int".
      lightBeat(8, 16, false) ;
      lockedBeat2 = bpm1 / 60 * 1000;
    }
    newBpm0 = bpm0 * 12000L * 2;
    newBpm1 = bpm1 * 12000L * 2;
  }
  if (digitalRead(PERSON_SWITCH_1_INPUT)) {
    if (currentMillis % lockedBeat1 == 0) {
      previousMillisL1 = currentMillis026;
      Serial.print("yes!");
    }
    lockedBeat(0, 8,  previousMillisL1) ;
  }
  if (digitalRead(PERSON_SWITCH_2_INPUT)) {
    if (currentMillis % lockedBeat2  == 0) {
      previousMillisL2 = currentMillis;
    }
    lockedBeat(8, 16,  previousMillisL2) ;
  }
}

void lightBeat(int minimum, int maximum, bool locked) {
  for (int i = minimum; i < maximum; i++) {
    if (currentMillis - previousMillis >= interval * i) {
      turnOn(i);
    }

  }
}

void lockedBeat(int minimum, int maximum, long unsigned int mills) {
  for (int i = minimum; i < maximum; i++) {
    if (i % 2 == 1) {
      if (currentMillis - mills >= interval * i) {
        turnOn(i);
      }
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

  Serial.print("Stepper 1 RPM: "); Serial.print(newBpm0); Serial.print(" ||| "); Serial.print("Dist to go: "); Serial.print(stepper1.distanceToGo());
  Serial.print("  Stepper 2 RPM: "); Serial.print(newBpm1); Serial.print(" ||| "); Serial.print("Dist to go: "); Serial.println(stepper2 .distanceToGo());
}

void updateRPM() {
  lcm = getLCM( newBpm0,  newBpm1) * 200 / 6;

  stepper1.setSpeed(newBpm0);
  stepper1.setAcceleration(100);
  stepper1.moveTo(lcm);
  delay(5);
  //set max speed to bpm since speed is set to rpm
  stepper2.setSpeed(newBpm1);
  stepper2.setAcceleration(100);
  stepper2.moveTo(lcm);
  delay(5);
}

int getLCM(int n1, int n2)
{
  int i, gcd, lcm;

  for (i = 1; i <= n1 && i <= n2; ++i)
  {
    // Checks if i is factor of both integers
    if (n1 % i == 0 && n2 % i == 0)
      gcd = i;
  }

  lcm = (n1 * n2) / gcd;
  printf(lcm);
  return lcm;
}

