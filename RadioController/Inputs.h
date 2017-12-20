// INPUTS
const byte pinJoystickAx = A4;
const byte pinJoystickAy = A5;
const byte pinJoystickBx = A2;
const byte pinJoystickBy = A3;
const byte pinSwitchD = 8;    // right toggle switch up
const byte pinSwitchE = 7;    // right toggle switch down
const byte pinSwitchF = 5;    // left toggle
const byte pinSwitchG = 6;    // left toggle


// INPUT VARIABLES
int joystickAx;
int joystickAy;
int joystickBx;
int joystickBy;
byte switchDTmp;
byte switchD;
byte switchETmp;
byte switchE;
byte switchFTmp;
byte switchF;
byte switchGTmp;
byte switchG;

// STICK CALIBRATION
int AX_CENTRE = 522;
int AX_MIN = 184;
int AX_MAX = 870;
int AY_CENTRE = 554;
int AY_MIN = 174;
int AY_MAX = 830;
int BX_CENTRE = 520;
int BX_MIN = 880;
int BX_MAX = 140;
int BY_CENTRE = 498;
int BY_MIN = 163;
int BY_MAX = 825;

const int OUTPUT_MIN = 0;
const int OUTPUT_MAX = 255;
const int STICK_DEADBAND = 5; // applied to the analog read

byte switchDPrevious = 0;  // DEFAULT LOW
byte switchEPrevious = 0;  // DEFAULT LOW
byte switchFPrevious = 0;  // DEFAULT LOW
byte switchGPrevious = 0;  // DEFAULT LOW

void setupInputs() {
  // set up user inputs (buttons, joysticks etc)
  pinMode(pinJoystickAx, INPUT); // not strictly required
  pinMode(pinJoystickAy, INPUT); // not strictly required
  pinMode(pinJoystickBx, INPUT); // not strictly required
  pinMode(pinJoystickBy, INPUT); // not strictly required
  pinMode(pinSwitchD, INPUT_PULLUP);
  pinMode(pinSwitchE, INPUT_PULLUP);
  pinMode(pinSwitchF, INPUT_PULLUP);
  pinMode(pinSwitchG, INPUT_PULLUP);
}

void readInputs() {
  delay(10);
  joystickAx = analogRead(pinJoystickAx);
  delay(10);
  joystickAy = analogRead(pinJoystickAy);
  delay(10);
  joystickBx = analogRead(pinJoystickBx);
  delay(10);
  joystickBy = analogRead(pinJoystickBy);
  delay(10);
  switchDTmp = digitalRead(pinSwitchD);
  switchETmp = digitalRead(pinSwitchE);
  switchFTmp = digitalRead(pinSwitchF);
  switchGTmp = digitalRead(pinSwitchG);
}


void debounceInputs() {
  static unsigned long debounceDelay = 50;
  static unsigned long switchDLastDebounceTime = 0;
  static unsigned long switchELastDebounceTime = 0;
  static unsigned long switchFLastDebounceTime = 0;
  static unsigned long switchGLastDebounceTime = 0;
  unsigned long currentTime = millis();
  
  if (switchETmp != switchFPrevious) {
    switchELastDebounceTime = currentTime;
  }
  if (switchFTmp != switchFPrevious) {
    switchFLastDebounceTime = currentTime;
  }
  if (switchGTmp != switchEPrevious) {
    switchGLastDebounceTime = currentTime;
  }
  if (switchDTmp != switchDPrevious) {
    switchDLastDebounceTime = currentTime;
  }
  if (currentTime - switchELastDebounceTime > debounceDelay) {
    switchE = switchETmp;
  }
  if (currentTime - switchFLastDebounceTime > debounceDelay) {
    switchF = switchFTmp;
  }
  if (currentTime - switchGLastDebounceTime > debounceDelay) {
    switchG = switchGTmp;
  }
  if (currentTime - switchDLastDebounceTime > debounceDelay) {
    switchD = switchDTmp;
  }

  switchEPrevious = switchETmp;
  switchFPrevious = switchFTmp;
  switchGPrevious = switchGTmp;
  switchDPrevious = switchDTmp;

}


//493 2 984
// WHERE TO LIMIT THE ACTUAL READINGS??
// WHERE TO ADD DEADBAND

void constrainInput(int *input, int *centre, int *minimum, int *maximum) {
  // account for high mins and low maxs
  int tmpInput = *input;
  
  if ((*centre - *minimum) > 1) {
    tmpInput = min(tmpInput, *maximum);
    tmpInput = max(tmpInput, *minimum);
  }
  else {
    tmpInput = max(tmpInput, *maximum);
    tmpInput = min(tmpInput, *minimum);
  }

  if (abs(*input - *centre) > STICK_DEADBAND) {
    *input = tmpInput;
  }
  else {
    *input = *centre;
  }
}

void constrainInputsAll(){
  constrainInput(&joystickAx, &AX_CENTRE, &AX_MIN, &AX_MAX);
//  constrainInput(&joystickAy, &AY_CENTRE, &AY_MIN, &AY_MAX);
  constrainInput(&joystickBx, &BX_CENTRE, &BX_MIN, &BX_MAX);
  constrainInput(&joystickBy, &BY_CENTRE, &BY_MIN, &BY_MAX);  
}

void addDeadband(){
  if(abs(joystickAx-127) < STICK_DEADBAND){
    joystickAx = 127;
  }
  if(abs(joystickBx-127) < STICK_DEADBAND){
    joystickBx = 127;
  }
  if(abs(joystickBy-127) < STICK_DEADBAND){
    joystickBy = 127;
  }
}



void enforceLimits(){
  joystickAx = constrain(joystickAx, 0, 255);
  joystickAy = constrain(joystickAy, 0, 255);
  joystickBx = constrain(joystickBx, 0, 255);
  joystickBy = constrain(joystickBy, 0, 255);
}




void mapInputs() {
  joystickAx = map(joystickAx, AX_MIN, AX_MAX, OUTPUT_MIN, OUTPUT_MAX);
  joystickAy = map(joystickAy, AY_MIN, AY_MAX, OUTPUT_MIN, OUTPUT_MAX);
  joystickBx = map(joystickBx, BX_MIN, BX_MAX, OUTPUT_MIN, OUTPUT_MAX);
  joystickBy = map(joystickBy, BY_MIN, BY_MAX, OUTPUT_MIN, OUTPUT_MAX);
  switchD = !switchDPrevious;
  switchE = !switchEPrevious;
  switchF = !switchFPrevious;
  switchG = !switchGPrevious;
}

int findCentre(byte pin) {
  int centre;
  unsigned long readingSum = 0;
  byte repetitions = 200;
  for (byte i = 0; i < repetitions; i++) {
    readingSum += analogRead(pin);
  }
  centre = readingSum / repetitions;
  return centre;
}

int findInputMax(byte pin, int centreReading) {
  int dir;
  int maxDiff = 0;
  int maxReading;
  // wait until reading changes from centre (beyond some threshold)
  while (abs(analogRead(pin) - centreReading) < 100) {
    // wait until input difference exceeds initial value
  }
  // check which way stick has been moved
  if (analogRead(pin) - centreReading > 0) {
    // reading larger values than initial
    dir = 1;
  }
  else {
    dir = -1;
  }
  Serial.print("dir: "); Serial.println(dir);
  // now look for the maximum value stick is at until reading is back close to initial value
  while (abs(analogRead(pin) - centreReading) > 50) {
    maxDiff = max(maxDiff, abs(analogRead(pin) - centreReading));
  }
  maxReading = centreReading + (maxDiff * dir);
  return maxReading;
}


int sign(int a) {
  int i;
  if (a > 0) {
    i = 1;
  }
  else if (a < 0) {
    i = -1;
  }
  else {
    i = 0;
  }
  return i;
}

void modifyInputRange(int *centre, int *minimum, int *maximum) {
  // check if limits are equally distant from the centre
  int minRange = min(abs(*centre - *minimum), abs(*centre - *maximum));
  // constrain
  *minimum = *centre - (sign(*centre - *minimum) * minRange);
  *maximum = *centre - (sign(*centre - *maximum) * minRange);
}

void calibration() {
  Serial.println(F("StickA X axis"));
  AX_CENTRE = findCentre(pinJoystickAx);
//  Serial.println(AX_CENTRE);
  Serial.println(F("Move to minimum"));
  AX_MIN = findInputMax(pinJoystickAx, AX_CENTRE);
//  Serial.println(AX_MIN);
  Serial.println(F("Move to maximum"));
  AX_MAX = findInputMax(pinJoystickAx, AX_CENTRE);
//  Serial.println(AX_MAX);
  modifyInputRange(&AX_CENTRE, &AX_MIN, &AX_MAX);
  Serial.println(AX_CENTRE);
  Serial.println(AX_MIN);
  Serial.println(AX_MAX);

  Serial.println(F("StickA Y axis"));
  AY_CENTRE = findCentre(pinJoystickAy);
  Serial.println(F("Move to minimum"));
  AY_MIN = findInputMax(pinJoystickAy, AY_CENTRE);
  Serial.println(F("Move to maximum"));
  AY_MAX = findInputMax(pinJoystickAy, AY_CENTRE);
  modifyInputRange(&AY_CENTRE, &AY_MIN, &AY_MAX);
  Serial.println(AY_CENTRE);
  Serial.println(AY_MIN);
  Serial.println(AY_MAX);

  Serial.println(F("StickB X axis"));
  BX_CENTRE = findCentre(pinJoystickBx);
  Serial.println(F("Move to minimum"));
  BX_MIN = findInputMax(pinJoystickBx, BX_CENTRE);
  Serial.println(F("Move to maximum"));
  BX_MAX = findInputMax(pinJoystickBx, BX_CENTRE);
  modifyInputRange(&BX_CENTRE, &BX_MIN, &BX_MAX);
  Serial.println(BX_CENTRE);
  Serial.println(BX_MIN);
  Serial.println(BX_MAX);

  Serial.println(F("StickB Y axis"));
  BY_CENTRE = findCentre(pinJoystickBy);
  Serial.println(F("Move to minimum"));
  BY_MIN = findInputMax(pinJoystickBy, BY_CENTRE);
  Serial.println(F("Move to maximum"));
  BY_MAX = findInputMax(pinJoystickBy, BY_CENTRE);
  modifyInputRange(&BY_CENTRE, &BY_MIN, &BY_MAX);
  Serial.println(BY_CENTRE);
  Serial.println(BY_MIN);
  Serial.println(BY_MAX);

  Serial.println(F("Stick calibration complete"));
  while(1); // currently end here, in future can store in EEPROM and continue
}


void processInputs() {
  readInputs();
//  printInputs();
  debounceInputs();
  constrainInputsAll(); // DON'T REALLY NEED THIS NOW (I THINK)
  mapInputs();
  addDeadband();
  enforceLimits(); // just in case
}

// TO DO - store/retrieve settings in EEPROM
void writeIntegerToEeprom(int address, int data){
  EEPROM.write(address,highByte(data));
  address++;
  EEPROM.write(address,lowByte(data));
}

int readIntegerFromEeprom(int address){
  byte high = EEPROM.read(address);
  byte low = EEPROM.read(address);
  int result=word(high,low);
  return result;
}

void printInputs() {
  Serial.print(joystickAx); Serial.print('\t');
  Serial.print(joystickAy); Serial.print('\t');
  Serial.print(joystickBx); Serial.print('\t');
  Serial.print(joystickBy); Serial.print('\t');
  Serial.print(switchD); Serial.print('\t');
  Serial.print(switchE); Serial.print('\t');
  Serial.print(switchF); Serial.print('\t');
  Serial.print(switchG); Serial.print('\n');
}

