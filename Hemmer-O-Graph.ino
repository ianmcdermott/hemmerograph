/*void setup() {
  // put your setup code here, to run once:
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  }

  void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(11, HIGH);
  delay(500);
  digitalWrite(10, HIGH);
  delay(250);
    digitalWrite(11, LOW);
  delay(250);
  digitalWrite(10, LOW);

  }
*/

// Shows how to run three Steppers at once with varying speeds
//
// Requires the Adafruit_Motorshield v2 library
//   https://github.com/adafruit/Adafruit_Motor_Shield_V2_Library
// And AccelStepper with AFMotor support
//   https://github.com/adafruit/AccelStepper

// This tutorial is for Adafruit Motorshield v2 only!
// Will not work with v1 shields

#include <Wire.h>
#include <AccelStepper.h>
#include <Adafruit_MotorShield.h>

#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>

PulseSensorPlayground pulseSensor;

int lcm;

Adafruit_MotorShield AFMSbot(0x61); // Rightmost jumper closed
Adafruit_MotorShield AFMStop(0x60); // Default address, no jumpers

// Connect two steppers with 200 steps per revolution (1.8 degree)
// to the top shield
Adafruit_StepperMotor *myStepper1 = AFMStop.getStepper(200, 1);
Adafruit_StepperMotor *myStepper2 = AFMStop.getStepper(200, 2);

const int OUTPUT_TYPE = SERIAL_PLOTTER;

const int PULSE_INPUT = 0;
const int PULSE_BLINK = 13;    // Pin 13 is the on-board LED
const int PULSE_FADE = 5;
const int THRESHOLD = 550;   // Adjust this number to avoid noise when idle
int rotateDelay = 25;
float pulseVal;

bool pulseMode = true;
bool drawingMode = false;
bool bpmIndex = false;

long newBpm0, newBpm1;
// LED INDICATORS ///

const int RED4 = 13;          // The on-board Arduino LED, close to PIN 13.
const int RED3 = 11;
const int RED2 = 9;
const int RED1 = 7;

const int GREEN4 = 12;          // The on-board Arduino LED, close to PIN 13.
const int GREEN3 = 10;
const int GREEN2 = 8;
const int GREEN1 = 6;

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

void setup()
{
  Serial.begin(115200);
  // Configure the PulseSensor manager.
  pulseSensor.analogInput(PULSE_INPUT);
  //  pulseSensor.blinkOnPulse(PULSE_BLINK);
  //  pulseSensor.fadeOnPulse(PULSE_FADE);

  //  pulseSensor.setSerial(Serial);
  //  pulseSensor.setOutputType(OUTPUT_TYPE);
  pulseSensor.setThreshold(THRESHOLD);

  // Now that everything is ready, start reading the PulseSensor signal.
  if (!pulseSensor.begin()) {
    /*
       PulseSensor initialization failed,
       likely because our particular Arduino platform interrupts
       aren't supported yet.

       If your Sketch hangs here, try changing USE_ARDUINO_INTERRUPTS to false.
       which doesn't use interrupts.
    */
    for (;;) {
      // Flash the led to show things didn't work.
      digitalWrite(PULSE_BLINK, LOW);
      delay(50);
      digitalWrite(PULSE_BLINK, HIGH);
      delay(50);
    }
  }

  AFMSbot.begin(); // Start the bottom shield
  AFMStop.begin(); // Start the top shield
  int bpm0 = 0;
  int bpm1 = 0;
  //get least common multiple of bpms to calculate moveto
  lcm = getLCM(bpm0, bpm1);

  bpm0 *= 200 * 60;

  bpm1 *= 200 * 60;
    stepper1.setMaxSpeed(600000);
    stepper1.setAcceleration(60);
  stepper1.moveTo(100000);
  delay(5);
  //set max speed to bpm since speed is set to rpm
  //  stepper2.setMaxSpeed(bpm1);
  //  stepper2.setAcceleration(20);
  stepper2.moveTo(lcm * 200 / 6);
  //  updateRPM();
  newBpm0 = 1176000;

  // LED INDICATORS ///
  pinMode(RED4, OUTPUT);
  pinMode(GREEN3, OUTPUT);
  pinMode(RED2, OUTPUT);
  pinMode(RED1, OUTPUT);

  pinMode(GREEN4, OUTPUT);
  pinMode(GREEN3, OUTPUT);
  pinMode(GREEN2, OUTPUT);
  pinMode(GREEN1, OUTPUT);

}

void loop()
{

  //  digitalWrite(PULSE_BLINK, LOW);
  //  delay(50);
  //  digitalWrite(PULSE_BLINK, HIGH);


  //  delay(50);

  //  Serial.print("BPM0: ");
  //
  //  Serial.println( bpm0);

  Serial.print("BPM0: ");

  Serial.println( newBpm0);
  //  bpm1 *= 200 * 60;
  //  // "myBPM" hold this BPM value now.
    stepper1.setSpeed(3000 );



  //  delay(20);
  pulseSensor.outputSample();
  pulseVal = pulseSensor.getLatestSample();
  // Reset the current position to 0 to start a new drawing
  if (drawingMode) {
//    stepper1.setAcceleration(60 / 10);
    //    stepper1.setSpeed(11760);
    if (stepper1.distanceToGo() == 0)
      stepper1.setCurrentPosition(0);

    if (stepper2.distanceToGo() == 0)
      stepper2.setCurrentPosition(0);
//    Serial.print("RUnning");
    stepper1.run();
    stepper2.run();

    //  delay(20);
  }
  if (pulseMode) {
    if (!bpmIndex) {
      bpm0 = int(pulseSensor.getBeatsPerMinute());  // Calls function on our pulseSensor object that returns BPM as an "int".
    } else {
      bpm1 = int(pulseSensor.getBeatsPerMinute());  // Calls function on our pulseSensor object that returns BPM as an "int".
    }
    newBpm0 = bpm0 * 12000L;

    updateRPM();
    rotatePulse();
    displayBPM(123);
  }
}

void updateRPM() {

  stepper1.setSpeed(newBpm0 );
  stepper1.setAcceleration(newBpm0 / 10);
  stepper1.moveTo(lcm * 200 / 6);
  delay(5);
  //set max speed to bpm since speed is set to rpm
  stepper2.setSpeed(bpm1 * 20 );
  stepper2.setAcceleration(20);
  stepper2.moveTo(lcm * 200 / 6);
  
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

/*
  displays BPM using LEDs
  Red = 100's
  Green = 10's
  Yellow = 1's

            [Hemmer-O-Graph]
  ________________________________________
  |                                      |
  |      • 0                  • 1        |
  |                                      |
  | • 2   ♥    • 3       • 4   ♥    • 5  |
  |                                      |
  |      • 6   • 7       • 8   • 9       |
  |                                      |
  ________________________________________
*/
void displayBPM(int currentBPM) {
  String BPMString = String(currentBPM);
  if (BPMString.length() == 3) {
    int currentNum = int(BPMString.charAt(0));
    display100s(currentNum, 3);
  } else if (BPMString.length() == 2) {



  } else if (BPMString.length() == 1) {

  }
}

void display100s(int ledNumber, int decimalPlace) {
  for (int i = 0; i < 10; i++) {
    if (i = ledNumber) {
      for (int i = 0; i < 5; i++) {
        if (decimalPlace = 3) {
          if (i % 2 == 0) {
            digitalWrite(GREEN4, HIGH);
            delay(50);
            digitalWrite(GREEN4, LOW);
            delay(50);
          } else {
            digitalWrite(ledNumber, HIGH);
            delay(50);
            digitalWrite(ledNumber, LOW);
            delay(50);
          }
        }
      }
    } else {
      if (i % 2 == 0) {
        digitalWrite(i, HIGH);
        delay(50);
        digitalWrite(i, LOW);
        delay(50);
      } else {
        digitalWrite(i, HIGH);
        delay(50);
        digitalWrite(i, LOW);
        delay(50);
      }
    }
  }

  delay(5000);
}



void displayPulse(bool bpm) {
}

void rotatePulse() {
  //  delay(20);
  if (pulseSensor.isInsideBeat()) {
    digitalWrite(GREEN4, HIGH);
    delay(rotateDelay);
    digitalWrite(RED4, HIGH);
    digitalWrite(GREEN3, HIGH);
    delay(rotateDelay);
    digitalWrite(RED3, HIGH);
    digitalWrite(GREEN4, LOW);
    delay(rotateDelay);
    digitalWrite(GREEN2, HIGH);
    digitalWrite(RED4, LOW);
    delay(rotateDelay);
    digitalWrite(RED2, HIGH);
    digitalWrite(GREEN3, LOW);
    delay(rotateDelay);
    digitalWrite(RED3, LOW);
    digitalWrite(GREEN1, HIGH);
    delay(rotateDelay);
    digitalWrite(RED1, HIGH);
    digitalWrite(GREEN2, LOW);
    delay(rotateDelay);
    digitalWrite(RED2, LOW);
    digitalWrite(GREEN1, LOW);
    delay(rotateDelay);
    digitalWrite(RED1, LOW);
    delay(rotateDelay);
  }
}

/*    digitalWrite(GREEN4, HIGH);
    delay(25);
    digitalWrite(LED12, HIGH);
    digitalWrite(GREEN3, HIGH);
    delay(25);
    digitalWrite(GREEN4, LOW);
    delay(25);
    digitalWrite(LED12, LOW);
    digitalWrite(RED3,HIGH);
    digitalWrite(GREEN2, HIGH);
    delay(25);
    digitalWrite(GREEN3, LOW);
    delay(25);
    digitalWrite(RED3, LOW);
    digitalWrite(RED2, HIGH);
    digitalWrite(GREEN1, HIGH);
    delay(25);
    digitalWrite(GREEN2, LOW);
    delay(25);
    digitalWrite(RED2, HIGH);
    digitalWrite(GREEN1, LOW);
    delay(25);

    digitalWrite(RED1, HIGH);
    digitalWrite(RED2, LOW);
    delay(25);
    digitalWrite(RED1, LOW);*/
