// OUTPUTS
const byte pinLedGreen = 2;
const byte pinLedRed = 4;

bool blinkGreenOn = false;
bool blinkYellowOn = false;
bool blinkRedOn = false;

struct led {
  byte pin;
  bool active;
  byte state;
  unsigned long intervalOn;
  unsigned long intervalOff;
  unsigned long timeOn;
  unsigned long timeOff;
};
led yellow = {3, false, 0, 0, 0, 0, 0}; // battery indicator


// LEDs

void setupOutputs() {
  // set up outputs
  pinMode(pinLedGreen, OUTPUT);
  pinMode(pinLedRed, OUTPUT);
  digitalWrite(pinLedGreen, LOW);
  digitalWrite(pinLedRed, LOW);
  blinkGreenOn = true;


  pinMode(yellow.pin, OUTPUT);
  digitalWrite(yellow.pin, yellow.state);
  yellow.active = true;
  yellow.intervalOn = 25;
  unsigned long tmp = millis();
  yellow.timeOn = tmp;
  yellow.timeOff = tmp;

}

void updateBatteryIndicator() {
  switch (batteryLevel) {
    case 7:
      yellow.intervalOff = 1975;
      break;
    case 6:
      yellow.intervalOff = 1725;
      break;
    case 5:
      yellow.intervalOff = 1475;
      break;
    case 4:
      yellow.intervalOff = 975;
      break;
    case 3:
      yellow.intervalOff = 475;
      break;
    case 2:
      yellow.intervalOff = 75;
      break;
    case 1:
      yellow.intervalOff = 50;
      break;
    case 0:
      yellow.intervalOff = 25;
      break;
  }
}

void updateLED(led* colour) {
  if (colour->active == false && colour->state == 0) {
  }
  else if (colour->active == false && colour->state == 1) {
    colour->state = 0;
    digitalWrite(colour->pin, colour->state);
  }
  else {
    // led is active, check whether to turn off or on
    unsigned long currentTime = millis();
    if (colour->state == 1 && (currentTime - colour->timeOn) >= colour->intervalOn) {
      colour->state = 0;
      digitalWrite(colour->pin, colour->state);
      colour->timeOff = currentTime;
    }
    else if (colour->state == 0 && (currentTime - colour->timeOff) >= colour->intervalOff) {
      colour->state = 1;
      digitalWrite(colour->pin, colour->state);
      colour->timeOn = currentTime;
    }
  }
}


void updateOutputs() {
  if (controllerState == TRANSMIT) {
    digitalWrite(pinLedGreen, HIGH); // Green LED will indicate there is a transmission
    digitalWrite(pinLedRed, !txSuccess);  // Turn on Red LED if transmission has failed
  }
  updateBatteryIndicator();
  updateLED(&yellow);
}


