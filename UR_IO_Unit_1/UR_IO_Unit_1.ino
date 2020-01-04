#include <MCP2515.h>
#include <SPI.h>
#include <Wire.h>
#include <MCP23017.h>
#include <MCP_Counter.h>
#include <Heartbeat_V2.h>
#include <DigitalOut.h>
#include <Butterworth.h>
#include <Scheduler.h>
#include <Analog.h>
#include <FaultLog.h>

#include <UR_Global_Structures.h>
#include <UR_Global_Define.h>

// CAN
#define CAN_NODE_ID                        CAN_IO_BOARD_1_ID

MCP2515 canbus;
Scheduler canbus_scheduler(IO_UNIT_STATUS_MSG_INTERVAL,IO_UNIT_ANALOG_01_MSG_INTERVAL,IO_UNIT_ANALOG_23_MSG_INTERVAL,IO_UNIT_COUNTER_A_MSG_INTERVAL,IO_UNIT_COUNTER_B_MSG_INTERVAL,IO_UNIT_FAULTLOG_MSG_INTERVAL,IO_UNIT_DUAL_COUNTER_MSG_INTERVAL,0,0,0);
MCP_Counter mcpcounter;
Heartbeat_V2 heartbeat(IOUNIT_HEARTBEAT_PIN,IOUNIT_HEARTBEAT_INTERVAL);
DigitalOut relay0; 
DigitalOut relay1;
FaultLog faultlog;
Analog analog0;
Analog analog1;
Analog analog2;
Analog analog3;

boolean NewCANMsg = false;

MasterStatusStruct master_status;
StdCANStruct CANTemp;
CanIOUnitStruct io_unit;

void setup(){
  heartbeat.Init();
  faultlog.Init(IOUNIT_FAULTLOG_SIZE,IOUNIT_FAULTLOG_RESET_INTERVAL);
  relay0.SetInitial(IOUNIT_RELAY_0_PIN,LOW);
  relay1.SetInitial(IOUNIT_RELAY_1_PIN,LOW);
  analog0.SetChannel(0,IOUNIT_1_ANALOG_CAPTURE_INTERVAL);
  analog1.SetChannel(1,IOUNIT_1_ANALOG_CAPTURE_INTERVAL);
  analog2.SetChannel(2,IOUNIT_1_ANALOG_CAPTURE_INTERVAL);
  analog3.SetChannel(3,IOUNIT_1_ANALOG_CAPTURE_INTERVAL);
  //Serial.begin(57600);
  canbus.Init(CAN_SPEED,IOUNIT_CAN_SLAVE_SELECT);
  attachInterrupt(0,ExtInt_0_ISR,FALLING);
  mcpcounter.Initialise(MCP23017_ADDR_0);
  StartCAN();
} 

void loop(){  
  heartbeat.Update();
  relay0.Update(); 
  relay1.Update();
  analog0.Update();
  analog1.Update();
  analog2.Update();
  analog3.Update();
  UpdateCAN();
}




