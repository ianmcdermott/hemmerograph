
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
if (!digitalRead(MODE_SWITCH) && digitalRead(PERSON_SWITCH_1_INPUT) && digitalRead(PERSON_SWITCH_2_INPUT)) {
    pulseMode = false;
    drawingMode = true;
//    Serial.println("Draw");

  } else {
    pulseMode = true;
    drawingMode = false;
//    Serial.println("Pulse");
  }
}

void checkPerson() {
  //if the input is receiving voltage, mark as pulse mode
  if (!digitalRead(PERSON_SWITCH_1_INPUT)) {
    bpmIndex = false; // Serial.println("Person 1");
  } else {
    bpmIndex = true;  // Serial.println("Person 2");
  }
}

