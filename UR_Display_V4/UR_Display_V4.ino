#include <MCP2515.h>
#include <SPI.h>
#include <Wire.h>
#include <Scheduler.h>
#include <Butterworth.h>
#include <MovingAvg.h>
#include <Heartbeat_V2.h>
#include <LiquidCrystal_I2C.h>
#include <DigitalOut.h>
#include <FaultLog.h>
#include <Analog.h>
#include <Keypad_UR.h>
#include <FaultLog.h>
#include <OneTimeSwitch.h>
#include <UR_Global_Define.h>                                                                                   // Location for configurable parameters common to all modules
#include <UR_Global_Structures.h>                                                                               // Location for structures common to all modules 

#include "VGA4ds.h"
#include "VGA4ds_Objects.h"

#define SCHEDULER_1000MS_TASK_NUM          1                                                                    // Used task 1 of scheduler as general purpose timer
#define SCHEDULER_1000MS_INTERVAL          1000                                                                 // Task 1 interval 1000mS
#define SCREEN_RESET_PIN                   3                                                                    // Digital pin attached to reset of 4DS VGA

// Analogs
#define ADC_CURRENT_PIN                    0                                                                    // Analog 0 is connected to ACS713 current measurement IC
#define ADC_CURRENT_OFFSET                 102                                                                  // ACS713 has a 0.5v or 10% bias so raw offset is 102
#define ADC_CURRENT_MIN_SCALE              0.0f                                                                 // ACS713 minimum reading mA
#define ADC_CURRENT_MAX_SCALE              20000.0f                                                             // ACS713 maximum reading mA
#define ADC_CURRENT_LIMIT                  10000.0f                                                             // Set current limit for overcurrent
#define ADC_OVERCURRENT_PERIOD             4000                                                                 // Set time mS before shutting off in overcurrent situation
#define ADC_VOLTAGE_PIN                    1                                                                    // Analog 1 is connected to power supply voltage via 1/3 voltage divide
#define ADC_VOLTAGE_OFFSET                 0                                                                    // No offset
#define ADC_VOLTAGE_MIN_SCALE              0.0f                                                                 // Minimum voltage mV
#define ADC_VOLTAGE_MAX_SCALE              15210.0f                                                             // Maximum voltage mV
#define ADC_VOLTAGE_LOW_LIMIT              10500.0f                                                             // Set limit for undervoltage
#define ADC_VOLTAGE_HIGH_LIMIT             14500.0f                                                             // Set limit for overvoltage
#define ADC_VOLTAGE_FAULT_PERIOD           4000                                                                 // Set time mS before shutting off in undervoltage situation

// CAN Power Relays
#define MASTER_RELAY_0                     48                                                                   // Used for Leveler supply
#define MASTER_RELAY_1                     49                                                                   // Used for IO unit 0 supply
#define MASTER_RELAY_2                     46                                                                   // Used for IO unit 1 supply
#define MASTER_RELAY_3                     47                                                                   // Used for VGA Monitor supply
#define MASTER_RELAY_4                     44                                                                   // Spare
#define MASTER_RELAY_5                     45                                                                   // Spare 
#define MASTER_RELAY_6                     42                                                                   // Spare 
#define MASTER_RELAY_7                     43                                                                   // Spare 

// Master Unit Switches
#define MASTER_SWITCH_0                    12                                                                   // IO unit 0 Relay 0
#define MASTER_SWITCH_1                    7                                                                    // IO unit 0 Relay 1
#define MASTER_SWITCH_2                    10                                                                   // IO unit 1 Relay 0
#define MASTER_SWITCH_3                    9                                                                    // IO unit 1 Relay 1  
#define MASTER_SWITCH_4                    8                                                                    // Used to enable CAN monitoring timers 
#define MASTER_SWITCH_5                    11                                                                   // Used to reset 4DS screen 
#define MASTER_SWITCH_6                    6                                                                    // Spare 
#define MASTER_SWITCH_7                    13                                                                   // Spare 

// Master Unit Switches
#define MASTER_LED_0                       34                                                                   // IO unit 0 Relay 0 Status
#define MASTER_LED_1                       35                                                                   // IO unit 0 Relay 1 Status
#define MASTER_LED_2                       36                                                                   // IO unit 1 Relay 0 Status
#define MASTER_LED_3                       37                                                                   // IO unit 1 Relay 1 Status
#define MASTER_LED_4                       38                                                                   // Spare 
#define MASTER_LED_5                       39                                                                   // Spare
#define MASTER_LED_6                       40                                                                   // Fault Led
#define MASTER_LED_7                       41                                                                   // Heartbeat Led

// LCD
#define LCD_0_ADDR                         0x20                                                                 // I2C Address for lcd 0, configurable
#define LCD_1_ADDR                         0x21                                                                 // I2C Address for lcd 1, used to display leveler data 
#define LCDROWS                            4                                                                    // Number of rows on LCD
#define LCDCOLS                            20                                                                   // Number of columns on LCD
#define LCD_0_UPDATE_INTERVAL              1000                                                                 // Period for LCD0 update
#define LCD_0_DISPLAY_PERIOD               5000                                                                 // Period for how long LCD0 displays selected data
#define LCD_1_UPDATE_INTERVAL              1000                                                                 // Period for LCD1 update

MasterUnitStruct master_unit;                                                                                   // Structure containing master status canbus message data 
MasterIORelayStruct relay_cmd_buf[MASTER_RELAY_CMD_BUF_SIZE];  
LevelingUnitStruct leveling_unit;                                                                               // Structure of structures containing data from the leveling unit
CanIOUnitStruct io_unit_0,io_unit_1;                                                                            // Structure of structures containing data from the io units

Heartbeat_V2 heartbeat(MASTER_HEARTBEAT_PIN,MASTER_HEARTBEAT_INTERVAL,MASTER_LED_7,MASTER_EXT_HBLED_INTERVAL);  // Master uses a DS1232 external watchdog, disabled by removing jumper on PCB
DigitalOut relay0,relay1,relay2,relay3,relay4,relay5,relay6,relay7;                                             // Use Library DigitalOut for relays
OneTimeSwitch io0rly0,io0rly1,io1rly0,io1rly1,canmonen,screenrstpb;                                             // Use Library OneTimeSwitch for triggering io relay commands
MCP2515 canbus;                                                                                                 // Library for using microchip MCP2515 CAN controller IC   
Scheduler scheduler(MASTER_STATUS_MSG_INTERVAL,SCHEDULER_1000MS_INTERVAL,0,0,0);                                // General purpose timer/scheduler used to send regular master status messages
LiquidCrystal_I2C lcd(LCD_0_ADDR,LCDCOLS,LCDROWS);                                                              // Library for I2C LCD modules, 20 x 4
LiquidCrystal_I2C lcd1(LCD_1_ADDR,LCDCOLS,LCDROWS);                                                             // Library for I2C LCD modules, 20 x 4 
VGA4ds vga;                                                                                                     // Library for using 4DSystems uVGA display 
Analog curr;                                                                                                    // Use Library Analog to measure/monitor powers supply current, samples at 500mS
Analog vcc;                                                                                                     // Use Library Analog to measure/monitor powers supply voltage, samples at 500mS 
Keypad_UR keypad_ur(26,27,28,29,30,31,32,33,250);                                                               // Use Library Keypad_UR to read 4x4 keypad, sample at 250mS 
FaultLog faultlog(MASTER_LED_6,MASTER_FAULT_LED_INTERVAL);                                                      // Use Library Faultlog to collate all faults, use LED to indicate a fault
MovingAvg bucketspeed,headspeed;

boolean NewCANMsg = false;                                                                                      // Used to indicate if CAN controller has generated an interrupt  
boolean MasterEditMsgInBuffer = false;                                                                          // Used to indicate if there is an edit msg command waiting to be sent
boolean MasterIORelayMsgInBuffer = false;                                                                       // Used to indicate if there is an io relay command waiting to be sent     
boolean MasterSetFilterMsgInBuffer = false;                                                                     // Used to indicate if there is an set filter command waiting to be sent
boolean OverCurrent = false;                                                                                    // Used to indicate if there is an ongoing overcurrent situation  
boolean VoltageFault = false;                                                                                   // Used to indicate if there is an ongoing voltage situation 
boolean UseCanMonitoring = false;                                                                               // Used to enable/disable can monitring functions
byte LcdDisplayScreen = LCD_DEFAULT_SCREEN;                                                                     // Used to select which lcd screen to display on lcd 0 
byte FaultBuffer[MASTER_FAULTLOG_SIZE];                                                                         // Array of bytes holding the latest faults
unsigned long Lcd0UpdateMillis,Lcd1UpdateMillis;                                                                // Used to schedule lcd updates 
unsigned long LcdDisplayMillis;                                                                                 // Used to retain selected lcd display screen
unsigned long FaultLogMillis;                                                                                   // Used to periodically retrieve faults from faultlog 
unsigned long OverCurrentMillis,VoltageFaultMillis;                                                             // Used to determine length of current and voltage faults 

void setup(){
  heartbeat.Init();                                                                                             // Initialise heartbeat     
  Serial.begin(57600);                                                                                          // Initialise Serial 0
  InitLcds();                                                                                                   // Initialise lcds
  canbus.Init(CAN_SPEED,MASTER_CAN_SLAVE_SELECT);                                                               // Initialise CAN Controller
  attachInterrupt(0,MCP2515_ISR,FALLING);                                                                       // Attach external interrupt for CAN Controller 
  StartCAN();                                                                                                   // Initialise local CAN objects 
  vga.Init(2,115200,SCREEN_RESET_PIN);                                                                          // Initialise 4DSystems uVGA 
  Init4DSObjects();                                                                                             // Initialise VGA display objects
  InitIO();                                                                                                     // Initialise inputs and outpts
  InitFaultLog();                                                                                               // Initialise faultlog
}

void loop(){
  heartbeat.Update();                                                                                           // Update heartbeat functions
  CANUpdate();                                                                                                  // Update CAN functions
  LcdUpdate();                                                                                                  // Update lcds
  UpdateIO();                                                                                                   // Update inputs and outputs
  vga.Update();                                                                                                 // Update 4DSystems uVGA
  UpdateFaultlog();                                                                                             // Update faultlog
}


