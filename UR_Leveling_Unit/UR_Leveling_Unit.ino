
#include <Wire.h>
#include <math.h>
#include <TinyGPS.h>
#include <EEPROM.h>
#include <mcp_can.h>
#include <SPI.h>
#include <MsTimer2.h>
#include "Scheduler.h"
#include "Canbus.h"
#include "AHRS.h"
#include "DigitalOutput.h"
#include "GlobalDefines.h"
#include "GlobalStructs.h"
#include "GlobalVariables.h"

void setup(){
  InitIO();                                             // Configure I/O
  CAN.begin(CAN_500KBPS);
  Serial.begin(57600);                                  // Serial 0 is for diagnosis and tuning
  Serial2.begin(9600);                                  // Serial 2 is for GPS
  Wire.begin();                                         // Start I2C hardware 
  FastI2C();                                            // Set I2C to 400kHz
  attachInterrupt(0,ExtInt_0_ISR, RISING);
  attachInterrupt(1,ExtInt_1_ISR, FALLING);
}

void loop(){
  SerialStatus();                                       // Send/Recieve data to/from display unit
  GPSStatus();                                          // Process GPS data
}






