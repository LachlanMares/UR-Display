
#include <MCP2515.h>
#include <SPI.h>
#include <Wire.h>
#include <TinyGPS.h>
#include <Butterworth.h>
#include <Heartbeat_V2.h>	
#include <LiquidCrystal_I2C.h>
#include <Scheduler.h>
#include <FaultLog.h>
#include "Leveller_UR.h"
#include <DualAnalog.h>
#include <DigitalOut.h>

#include <UR_Global_Define.h>
#include <UR_Global_Structures.h>

// CAN
#define CAN_SLAVE_SELECT                   53

// Analog In Pins
#define LEFT_RAM_CH0_PIN                   10
#define LEFT_RAM_CH1_PIN                   11
#define RIGHT_RAM_CH0_PIN                  14
#define RIGHT_RAM_CH1_PIN                  15

// Digital In Pins
#define LEFT_PROX_SWITCH                   11
#define RIGHT_PROX_SWITCH                  10
#define AUTO_SWITCH                        13
#define SPARE_SWITCH                       12

// Digital Out Pins
#define READY_LED                          42
#define AUTO_MODE_LED                      45
#define LEFT_UP_RELAY                      47
#define LEFT_DOWN_RELAY                    46
#define RIGHT_UP_RELAY                     49
#define RIGHT_DOWN_RELAY                   48
#define LED_0                              43

// LCD
#define LCD_ADDR                           0x27
#define LCDROWS                            4                       
#define LCDCOLS                            20                      
#define LCD_PIN                            4
#define LCD_SCAN_INTERVAL                  5000
#define LCD_UPDATE_INTERVAL                1000

// Global Variables
boolean NewCANMsg = false;
boolean FirstMasterStatus = false;
boolean LcdConnected = false;
unsigned char LevellerActiveMsgs,LastMasterSeqNum;
unsigned long LcdScanMillis,LcdUpdateMillis,Gps_Age;

// Structs
StdCANStruct CANTemp;
MasterStatusStruct master_status;
PositionStruct gps_position;
DatetimeStruct datetime;

// Objects
Heartbeat_V2 heartbeat(LVLR_HEARTBEAT_PIN,LVLR_HEARTBEAT_INTERVAL);
Scheduler canbus_scheduler(LVLR_STATUS_MSG_INTERVAL,LVLR_RAM_POSITIONS_MSG_INTERVAL,LVLR_ROLL_PITCH_MSG_INTERVAL,LVLR_FAULTLOG_MSG_INTERVAL,LVLR_POSITION_MSG_INTERVAL,LVLR_DATETIME_MSG_INTERVAL,0,0,0,0);
Leveller_UR leveller(LVLR_FILTER_UPDATE_INTERVAL,LVLR_ACTION_INTERVAL);
MCP2515 canbus;
FaultLog faultlog(LVLR_FAULT_LED,LVLR_FAULT_LED_INTERVAL);
TinyGPS gps;
LiquidCrystal_I2C lcd(LCD_ADDR,LCDCOLS,LCDROWS);

void setup(){
  heartbeat.Init();
  pinMode(READY_LED,OUTPUT);
  digitalWrite(READY_LED,LOW);
  Serial.begin(57600);
  Serial2.begin(9600);    
  Wire.begin(); 
  leveller.Init(LEFT_UP_RELAY,LEFT_DOWN_RELAY,RIGHT_UP_RELAY,RIGHT_DOWN_RELAY,AUTO_SWITCH,AUTO_MODE_LED,LEFT_PROX_SWITCH,RIGHT_PROX_SWITCH);                                                                
  leveller.InitRams(LEFT_RAM_CH0_PIN,LEFT_RAM_CH1_PIN,RIGHT_RAM_CH0_PIN,RIGHT_RAM_CH1_PIN,LVLR_RAM_UPDATE_INTERVAL);
  canbus.Init(CAN_SPEED,CAN_SLAVE_SELECT);                                              
  faultlog.Init(LVLR_FAULTLOG_SIZE,LVLR_FAULTLOG_RESET_INTERVAL);
  LcdInit();
  leveller.Start();
  StartCAN();
  digitalWrite(READY_LED,HIGH);
}

void loop(){      
  if(leveller.Update() != NO_ERROR){
    faultlog.AddFault(leveller.CurrentError());
  }    
  faultlog.Update();
  heartbeat.Update();
  CANUpdate();
  LcdUpdate();
  GPSUpdate();   
}




