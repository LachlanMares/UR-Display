#ifndef Leveller_UR_h
#define Leveller_UR_h
#include <Butterworth.h>
#include <DualAnalog.h>
#include <DigitalOut.h>

#include "Arduino.h"
#include "UR_Global_Define.h"
#include "UR_Global_Structures.h"

#define LEFT_UP_RELAY_BIT                      0
#define LEFT_DOWN_RELAY_BIT                    1
#define RIGHT_UP_RELAY_BIT                     2
#define RIGHT_DOWN_RELAY_BIT                   3 
#define LEFT_PROX_BIT                          4
#define RIGHT_PROX_BIT                         5
#define MASTER_LEFT_BIT		               6
#define MASTER_RIGHT_BIT		       7

#define LVLR_OUT_OF_LEVEL_COUNTER_THRESHOLD    1000/LVLR_FILTER_UPDATE_INTERVAL

class Leveller_UR
{
public:
	Leveller_UR(int,int);
        void Init(int,int,int,int,int,int,int,int);  
        void InitRams(int,int,int,int,unsigned int);
        void Start();
        void Stop();
        unsigned char Update();
        unsigned char Mode(); 
        boolean Master();
        unsigned char LeftStatus();
        float LeftRamPosition();
        unsigned char RightStatus();
        float RightRamPosition();
        unsigned char LevelState();
        void Leveller_Msg(void*,unsigned char);
        void RamPos_Msg(void*);
        unsigned char CurrentError();

private:
        void SetLeftUpRelay();
        void SetLeftDownRelay();
        void SetRightUpRelay();
        void SetRightDownRelay();
        unsigned char RamState(float);
        
        boolean _Enable,_MasterLeft,_ProxLeftState,_ProxRightState,_LeftUpRelayState,_LeftDownRelayState,_RightUpRelayState,_RightDownRelayState;
        int _FilterInterval,_RamPositionUpdateInterval,_NewActionInterval;
        int _AutoPBPin,_AutoModeLedPin,_LeftProx,_RightProx;
        unsigned int _LeftADCRaw,_RightADCRaw,_PosCounter,_NegCounter;
        long _FilterMillis,_RamPositionUpdateMillis,_NewActionMillis; 
  
        DualAnalog _LeftRam,_RightRam;
        DigitalOut _LeftUpRelay,_LeftDownRelay,_RightUpRelay,_RightDownRelay;
        RamPositionStruct _RamPos;
        LevelerStatusStruct _Sts_Struct;
};

#endif
