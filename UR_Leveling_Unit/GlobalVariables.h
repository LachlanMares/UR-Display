#ifndef GlobalVariables_h
#define GlobalVariables_h

// Global system variables

byte LeftSideStatus = BOTH_OFF;
byte RightSideStatus = BOTH_OFF;
byte LeftSideSwitches = NO_CMD;
byte RightSideSwitches = NO_CMD;

boolean InterruptFlag = true; 
boolean CalMode = true;
boolean MagCalReq = false;
boolean SetGyroBias = true;
boolean SendStatusMsg = false;
boolean LevelMaster = true;
boolean AutoMode = false;
boolean OutOfTolerancePositive = false;
boolean OutOfToleranceNegative = false;

DigitalOutput LeftUpRelay(22,25mS);
DigitalOutput LeftDownRelay(24,25mS);
DigitalOutput RightUpRelay(26,25mS);
DigitalOutput RightDownRelay(28,25mS);

DigitalInput AutoModeButton(25,25mS);
DigitalInput LeftUpButton(31,25mS);
DigitalInput LeftDownButton(23,25mS);
DigitalInput RightUpButton(29,25mS);
DigitalInput RightDownButton(27,25mS);

const int LeftProx1 = 33;
const int LeftProx2 = 35;
const int RightProx1 = 37;
const int RightProx2 = 39;

int PrintCounter = 0;
int GyroSampleCounter = 0;
int ErrorCounter = 0;
int WithinToleranceCounter = 0;
int OutOfTolerancePositiveCounter = 0;
int OutOfToleranceNegativeCounter = 0;
int LeftProxCounter = 0;
int RightProxCounter = 0;

TinyGPS gps;

Canbus can;
AHRS ahrs;
Euler euler;
Scheduler scheduler(TIMER2,50mS,100mS,250mS,1000mS,5000mS);

Status_Msg_Struct Status_Msg;
GPS_Pos_Msg_Struct GPS_Pos_Msg;
GPS_Time_Msg_Struct GPS_Time_Msg;
IMU_PR_Msg_Struct IMU_PR_Msg;
IMU_SH_Msg_Struct IMU_SH_Msg;
IO_Msg_Struct IO_Msg;

#endif

