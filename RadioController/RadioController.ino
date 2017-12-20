// TO DO - add radio.isChipConnected() check
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
  controllerState = TRANSMIT;
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
    if (millis() - lastTransmission >= txFreq) {
      printInputs();
      // printPackage();
      sendPackage();
      lastTransmission += txFreq;
      incAliveVal = true;
      // printInputs();
      if (radio.isAckPayloadAvailable()) {
        radio.read(&ack, sizeof(ack));
        // Serial.println(ack);
        parseAcknowledgement();
        // Serial.println(batteryLevel);
      }
      else {
      }
    }
  }
  updateOutputs();
} // LOOP
