// the transmitter must be sending data in the expected structure

// remove all but 1 address
// only open 1 pipe if possible
// create class?

#include <SPI.h>
#include "RF24.h"

const byte address[] = "1Node";
const byte pipeNumber = 1;

// Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(9, 10); // CE and CSN

struct dataStruct {
  byte throttle;
  byte roll;
  byte pitch;
  byte yaw;
  byte control; // for some control bits
  byte alive;
  byte checksum;
} rcPackage;

byte statusForAck = 0;
bool rxHeartbeat = false;
unsigned long lastRxReceived = 0;
static unsigned long heartbeatTimeout = 1000;

void setup() {
  Serial.begin(115200);
  setupRadio();
  Serial.println(F("Receiver starting..."));
}

void loop() {
  if (checkRadioForInput()) {
    printPackage();
  }
} // Loop

void setupRadio() {
  radio.begin();
  radio.setPALevel(RF24_PA_HIGH);  // MIN, LOW, HIGH, MAX
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.setDataRate(RF24_250KBPS); // RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  radio.openReadingPipe(pipeNumber, address);
  radio.startListening();
}

// modify to return information as required
void updateAckStatusForTx() {
  static byte inc = 0;
  inc ++;
  statusForAck = inc;
}

byte calculateCheckSum() {
  // note that some of these are not bytes so not a complete check
  // TO DO - UPDATE
  byte sum = 0;
  sum += rcPackage.throttle;
  sum += rcPackage.pitch;
  sum += rcPackage.roll;
  sum += rcPackage.yaw;
  sum += rcPackage.control;
  sum += rcPackage.alive;
  sum = 1 - sum;
  return sum;
}

bool checkRadioForInput() {
  if ( radio.available()) {
    while (radio.available()) {
      radio.read( &rcPackage, sizeof(rcPackage) );
    }
    // load acknowledgement payload for the next transmission (first transmission will not get any ack payload (but will get normal ack))
    radio.writeAckPayload(1, &statusForAck, sizeof(statusForAck));
    if (rcPackage.checksum != calculateCheckSum()) {
      radio.flush_rx();
      Serial.println(F("Failed checksum"));
      return false;
    }
    lastRxReceived = millis();
    radio.flush_rx(); // probably remove
    updateAckStatusForTx(); // for next time
    return true;
  }
  return false;
}

bool checkHeartbeat() {
  if (millis() - lastRxReceived > heartbeatTimeout) {
    rxHeartbeat = false;
  }
  else {
    rxHeartbeat = true;
  }
  return rxHeartbeat;
}

void printPackage() {
  Serial.print(rcPackage.throttle); Serial.print('\t');
  Serial.print(rcPackage.roll); Serial.print('\t');
  Serial.print(rcPackage.pitch); Serial.print('\t');
  Serial.print(rcPackage.yaw); Serial.print('\t');
  Serial.print(rcPackage.control); Serial.print('\t');
  Serial.print(rcPackage.alive); Serial.print('\t');
  Serial.print(rcPackage.checksum); Serial.print('\n');
}
