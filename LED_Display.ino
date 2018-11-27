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
//  for (int i = 0; i < 10; i++) {
//    if (i = ledNumber) {
//      for (int i = 0; i < 5; i++) {
//        if (decimalPlace = 3) {
//          if (i % 2 == 0) {
//            digitalWrite(GREEN4, HIGH);
//            delay(50);
//            digitalWrite(GREEN4, LOW);
//            delay(50);
//          } else {
//            digitalWrite(ledNumber, HIGH);
//            delay(50);
//            digitalWrite(ledNumber, LOW);
//            delay(50);
//          }
//        }
//      }
//    } else {
//      if (i % 2 == 0) {
//        digitalWrite(i, HIGH);
//        delay(50);
//        digitalWrite(i, LOW);
//        delay(50);
//      } else {
//        digitalWrite(i, HIGH);
//        delay(50);
//        digitalWrite(i, LOW);
//        delay(50);
//      }
//    }
//  }
//
//  delay(5000);
}

void display10s() {
}

void display1s() {
}

void displayPulse(bool bpm) {
  display100s(5, 100);
  display10s();
  display1s();
}

void rotatePulse() {
  //  delay(20);
  if (pulseSensor.isInsideBeat()) {
//    analogWrite(GREEN4, HIGH); // Light First LED Green
//    delay(rotateDelay);
//    analogWrite(RED4, HIGH);   // Light First LED Red
//    analogWrite(GREEN3, HIGH); // Light Second LED Green
//    delay(rotateDelay);
//    analogWrite(RED3, HIGH);   // Light Second LED Red
//    analogWrite(GREEN4, LOW);  // Light First LED Red
//    delay(rotateDelay);
//    analogWrite(GREEN2, HIGH);
//    analogWrite(RED4, LOW);
//    delay(rotateDelay);
//    analogWrite(RED2, HIGH);
//    analogWrite(GREEN3, LOW);
//    delay(rotateDelay);
//    analogWrite(GREEN1, HIGH);
//    analogWrite(RED3, LOW);
//    delay(rotateDelay);
//    analogWrite(RED1, HIGH);
//    analogWrite(GREEN2, LOW);
//    delay(rotateDelay);
//    analogWrite(RED2, LOW);
//    analogWrite(GREEN1, LOW);
//    delay(rotateDelay);
//    analogWrite(RED1, LOW);
//    delay(rotateDelay);
  }
}

// Animate LEDs to current person's pulse
void animatePulse(int  pins[]) {
  int pl = sizeof(pins);
  int numLights = 0;
  for (int i = 0; i < pl; i++) {
    if (i == 0) {
      analogWrite(pins[i], 255);
      delay(rotateDelay);
    } else if (i == 1) {
      analogWrite(pins[i], 255);
      analogWrite(pins[i + 1], 255);
      delay(rotateDelay);
    } else if (i > 1 && i < pl - 2) {
      analogWrite(pins[i], 255);
      analogWrite(pins[i - 2], 0);
      delay(rotateDelay);
    } else if (i == pl - 2) {
      analogWrite(pins[i], 0);
      analogWrite(pins[i - 1], 0);
    } else {
      analogWrite(pins[i], 0);
    }
  }
}

