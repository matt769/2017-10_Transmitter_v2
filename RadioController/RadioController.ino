/* Have re-built the transmitter inside a pre-bought shell with much better sticks
   A few code changes required but nothing major - mainly pin numbers
   No LCD
*/


// deadband shouldn't be applied to the throttle
// add radio.isChipConnected() check in order for green light to be lit


#include <SPI.h>
#include <RF24.h>
#include <EEPROM.h>

// STATES
const byte STANDBY = 0;
const byte TRANSMIT = 1;
const byte TX_ERROR = 2;
const byte CALIBRATE = 3;
byte controllerState = STANDBY;


#include "Inputs.h"
#include "Radio.h"
#include "UserOutputs.h"

void setup() {
  Serial.begin(115200);
  Serial.println(F("Starting..."));
  setupInputs();
  setupOutputs();
  setupRadio();
  processInputs();
//    controllerState = CALIBRATE;  // for testing
  if (controllerState == CALIBRATE) {
    calibration();
  }
  controllerState = TRANSMIT;  // for testing
} // SETUP


void loop() {
  if (controllerState == TRANSMIT) {
    processInputs();
    if (incAliveVal) {
      buildPackage(true);
      incAliveVal = false;
    }
    else {
      buildPackage(false);
    }

    if (millis() - lastTransmission > txFreq) {
            printInputs();
//            printPackage();
//      Serial.println("Sending...");
      sendPackage();
      lastTransmission += txFreq;
      incAliveVal = true;
      // printInputs();
      // Serial.println(txSuccess);
      // Serial.print(F("Ack: "));
      if (radio.isAckPayloadAvailable()) {
        radio.read(&ack, sizeof(ack));
        //        Serial.println(ack);
        parseAcknowledgement();
//                Serial.println(batteryLevel);
      }
      else {
        // Serial.println(F("None"));
      }
    }

  }
  updateOutputs();
  //  printInputs();
  //  delay(500);
//  Serial.println(batteryLevel);
} // LOOP
