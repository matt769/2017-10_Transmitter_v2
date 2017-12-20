
// Control byte
// bit 0:                                         Stick 1 button pressed
// bit 1:
// bit 2: 1 = Attitude mode, 0 = rate mode        Toggle switch
// bit 3: 1 = auto-level on, 0 = auto-level off   Stick 2 button pressed
// bit 4: 
// bit 5: 
// bit 6: 
// bit 7; 1 = TURN OFF MOTORS                     Push button


// Acknowledgement byte
// bit 0:
// bit 1:
// bit 2:
// bit 3:
// bit 4: 
// bit 5: Battery
// bit 6: Battery
// bit 7: Battery


// RADIO
const byte addresses[][6] = {"1Node", "2Node"};
const bool radioNumber = 0; // receiver should be 1 (or anythin not 0)
RF24 radio(9,10); // CE, CSN (SPI SS)

struct dataStruct {
  int inputOne; // number 0 to 1000
  int inputTwo;     // number 0 to 1000
  int inputThree;    // number 0 to 1000
  int inputFour;     // number 0 to 1000
  byte control; // for some control bits
  byte alive; //this will increment every time the data is sent
  byte checksum;
} rcPackage;

byte batteryLevel = 0;

const int txFreq = 50; // in milliseconds i.e. txFreq = 100 ==> 1000/100 = 10Hz
// actual frequency will be slightly lower
unsigned long lastTransmission = 0;
unsigned long lastAcknowledgement = 0;
bool txSuccess = false;
byte ack = 0;

bool incAliveVal = true;

void setupRadio() {
  // RADIO
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);  // MIN, LOW, HIGH, MAX
  // Open a writing and reading pipe on each radio, MUST BE OPPOSITE addresses to the receiver
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.setDataRate(RF24_250KBPS);

  if(radioNumber){
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
  }else{
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
  }
}

void parseAcknowledgement(){
  // just have battery for the moment
  if (ack & 1 == 1){  // all acknowledgements should have bit 0 = 1
    byte mask = 224;
    batteryLevel = (ack & mask) >> 5;
  }
}

byte calculateCheckSum(){
  byte sum = 0;
  sum += rcPackage.inputOne;
  sum += rcPackage.inputTwo;
  sum += rcPackage.inputThree;
  sum += rcPackage.inputFour;
  sum += rcPackage.control;
  sum += rcPackage.alive;
  sum = 1 - sum;
  return sum;
}

void buildPackage(bool increment) {
  static byte alive = 0;
  rcPackage.inputOne = joystickAy;
  rcPackage.inputTwo = joystickBx;
  rcPackage.inputThree = joystickBy;
  rcPackage.inputFour = joystickAx;
  byte tmp = 0;
  tmp |= !switchD << 2;  // note switch D on = toggle switch up = rate mode. If rate mode is not selected then must be attitude mode
  tmp |= switchE << 3;  // switchE on = toggle switch down = auto-level
//  tmp |= (buttonF || buttonG) << 7;
  rcPackage.control = tmp;
  if(increment){
    alive++;
  }
  rcPackage.alive = alive;
  rcPackage.checksum = calculateCheckSum();
}

void sendPackage() {
  radio.flush_rx();
  txSuccess = radio.write( &rcPackage, sizeof(rcPackage));
}


void printPackage(){
  Serial.print(rcPackage.inputOne);Serial.print('\t');
  Serial.print(rcPackage.inputTwo);Serial.print('\t');
  Serial.print(rcPackage.inputThree);Serial.print('\t');
  Serial.print(rcPackage.inputFour);Serial.print('\t');
  Serial.print(rcPackage.control);Serial.print('\t');
  Serial.print(rcPackage.alive);Serial.print('\t');
  Serial.print(rcPackage.checksum);Serial.print('\n');
}

