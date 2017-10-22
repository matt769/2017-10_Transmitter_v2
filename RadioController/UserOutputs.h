// OUTPUTS
const byte pinLedGreen = 2;
const byte pinLedYellow = 3;
const byte pinLedRed = 4;

bool blinkGreenOn = false;
bool blinkYellowOn = false;
bool blinkRedOn = false;


// LEDs and LCD

void setupOutputs() {
  // set up outputs
  pinMode(pinLedGreen, OUTPUT);
  pinMode(pinLedYellow, OUTPUT);
  pinMode(pinLedRed, OUTPUT);
  digitalWrite(pinLedGreen, LOW);
  digitalWrite(pinLedYellow, LOW);
  digitalWrite(pinLedRed, LOW);
  blinkGreenOn = true;
}

void updateBatteryIndicator(){
  if (batteryLevel < 2) {
  blinkYellowOn = true;
  }
  else if (batteryLevel < 3){
    blinkYellowOn = false;
    digitalWrite(pinLedYellow,HIGH);
  }
  else {
    blinkYellowOn = false;
    digitalWrite(pinLedYellow,LOW);
  }
}

void blinkGreen(){
  static unsigned long lastChange = 0;
  static unsigned long interval = 500;
  if(blinkGreenOn && (millis()-lastChange > interval)){
    digitalWrite(pinLedGreen,!digitalRead(pinLedGreen));
    lastChange += interval;
  }
}

void blinkYellow(){
  static unsigned long lastChange = 0;
  static unsigned long interval = 500;
  if(blinkYellowOn && (millis()-lastChange > interval)){
    digitalWrite(pinLedYellow,!digitalRead(pinLedYellow));
    lastChange += interval;
  }
}


void blinkRed(){
  static unsigned long lastChange = 0;
  static unsigned long interval = 500;
  if(blinkRedOn && (millis()-lastChange > interval)){
    digitalWrite(pinLedRed,!digitalRead(pinLedRed));
    lastChange += interval;
  }
}

void updateOutputs() {
  if (controllerState == TRANSMIT) {
    digitalWrite(pinLedGreen, HIGH); // Green LED will indicate there is a transmission
    blinkGreenOn = false;
    digitalWrite(pinLedRed, !txSuccess);  // Turn on Red LED if transmission has failed
  }
  blinkGreen();
  blinkYellow();
  blinkRed();
}


