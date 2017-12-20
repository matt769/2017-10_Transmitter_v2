// the transmitter must be sending data in the expected structure

// remove all but 1 address
// only open 1 pipe if possible
// create class?

#include <SPI.h>
#include "RF24.h"

byte addresses[][6] = {"1Node", "2Node"};
bool radioNumber = 1;

// Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(9, 10); // CE and CSN

struct dataStruct {
  //  unsigned long micro; // might not be part of package but just including anyway
  int throttle; // number 0 to 1000
  int roll;    // number 0 to 1000
  int pitch;     // number 0 to 1000
  int yaw;     // number 0 to 1000
  byte control; // for some control bits
  byte alive;
  byte checksum;
} rcPackage;

byte statusForAck = 0;
bool rxHeartbeat = false;
unsigned long lastRxReceived = 0;
unsigned long heartbeatTimeout = 500;

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


// modify to return information as required
void updateAckStatusForTx() {
  static byte inc = 0;
  inc ++;
  statusForAck = inc;
}


void setupRadio() {
  // RADIO
  radio.begin();
  radio.setPALevel(RF24_PA_HIGH);  // MIN, LOW, HIGH, MAX
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  // RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps // slower is more reliable and gives longer range
  radio.setDataRate(RF24_250KBPS);
  //   * @param delay How long to wait between each retry, in multiples of 250us,
  //   * max is 15.  0 means 250us, 15 means 4000us.
  //   * @param count How many retries before giving up, max 15
  //  radio.setRetries();   // default is setRetries(5,15) // note restrictions due to ack payload

  // Open a writing and reading pipe on each radio, MUST BE OPPOSITE addresses to the receiver
  if (radioNumber) {
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1, addresses[0]);
  } else {
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1, addresses[1]);
  }
  radio.startListening();
  //  Serial.println(radio.isChipConnected());
}


byte calculateCheckSum() {
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
    //    Serial.println(statusForAck);
    lastRxReceived = millis();
    radio.flush_rx(); // probably remove
    updateAckStatusForTx(); // for next time
    return true;
  }
  return false;
}

bool checkHeartbeat() {
  if (millis() - lastRxReceived > heartbeatTimeout) {
    //    Serial.println(F("Lost connection"));
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
