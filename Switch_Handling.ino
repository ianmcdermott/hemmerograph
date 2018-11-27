
// Check switches to see the curent mode
void handleSwitches() {
  // If p1 and p2 switches are activated, but mode is not (drawing mode) start drawing
  if (PERSON_SWITCH_1 && PERSON_SWITCH_2 && !MODE_SWITCH) {
    previousMode = true;
  }
  // If person switch 1 is off, turn on Person 1 and send bpm signal
  if (digitalRead(PERSON_SWITCH_1_INPUT)) {
    digitalWrite(PERSON_SWITCH_1, HIGH); // Replace w/ BPM INPUT HERE
    bpmIndex = false;
    Serial.println("Person 1");
  } else if (digitalRead(PERSON_SWITCH_2_INPUT) && digitalRead(!PERSON_SWITCH_1_INPUT)) {
    bpmIndex = true;
    digitalWrite(PERSON_SWITCH_2, HIGH); // Replace w/ BPM INPUT HERE
    Serial.println("Person 2");
  }    else if (digitalRead(!PERSON_SWITCH_2_INPUT) && digitalRead(!PERSON_SWITCH_1_INPUT)) {
    bpmIndex = true;
    digitalWrite(PERSON_SWITCH_2, HIGH); // Replace w/ BPM INPUT HERE
    Serial.println("Person 2");
  }
  // Flipping person's switch locks in the pulse

  // Flipping the mode switch activates the drawing only if both pulses have been stored,
  // otherwise blink red around the heart(s) that needs the pulse
}

// Reset plotter arms to center
void reset() {
  // If mode switch has changed back to false (pulse mode) and previously was true (drawing mode)
  // reset steppers to center position (0, 0)
  if (!MODE_SWITCH && previousMode) {
    previousMode = false;
  }
}

void checkMode() {
  //if the input is receiving voltage, mark as pulse mode
  if (digitalRead(MODE_SWITCH)) {
//    previousPulseMode1 = true;
    //    if (!previousDrawingMode1) {
    pulseModeCount++;
    if (pulseModeCount > 10) {
      pulseMode = true;
      drawingMode = false;
      previousDrawingMode2 = false;
      //      }
      pulseModeCount = 0;
    }
    previousDrawingMode1 = false;
  } else if (digitalRead(!MODE_SWITCH) && digitalRead(PERSON_SWITCH_1_INPUT) && digitalRead(PERSON_SWITCH_2_INPUT)) {
    pulseMode = false;
    drawingMode = true;
    previousDrawingMode1 = true;
    if (previousPulseMode1) {
      previousDrawingMode2 = false;
    }
  }



  //if Person 1 is active, show live BPM
  //  if(!bpmIndex) showLiveBPM(person1)
}

// If mode_switch is false but person 1 & 2 switches are true, drawing mode on

