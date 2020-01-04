#include "Leveller_UR.h"	

Leveller_UR::Leveller_UR(int FilterInterval,int NewActionInterval){  
  _Enable = false;
  _MasterLeft = true;
  _ProxLeftState = false;
  _ProxRightState = false;
  _LeftUpRelayState = LOW;
  _LeftDownRelayState = LOW;
  _RightUpRelayState = LOW;
  _RightDownRelayState = LOW;
  _FilterInterval = FilterInterval;                                                                                        
  _NewActionInterval = NewActionInterval;
  _Sts_Struct.LevelerMode = LVLR_STATUS_INIT;
  _Sts_Struct.SeqCnt = 0;
  _PosCounter = 0;
  _NegCounter = 0;
  
  DualAnalog _LeftRam,_RightRam;
  DigitalOut _LeftUpRelay,_LeftDownRelay,_RightUpRelay,_RightDownRelay;
}

void Leveller_UR::Init(int LUR,int LDR,int RUR,int RDR,int AMS,int AML,int LPR,int RPR){
  _LeftUpRelay.SetInitial(LUR,LOW);   
  _LeftDownRelay.SetInitial(LDR,LOW);  
  _RightUpRelay.SetInitial(RUR,LOW); 
  _RightDownRelay.SetInitial(RDR,LOW);
 
  _AutoPBPin = AMS;
  _LeftProx = LPR;
  _RightProx = RPR;
  _AutoModeLedPin = AML; 
  
  pinMode(_AutoPBPin,INPUT_PULLUP);
  pinMode(_LeftProx,INPUT);
  pinMode(_RightProx,INPUT); 
  pinMode(_AutoModeLedPin,OUTPUT);
  digitalWrite(_AutoModeLedPin,LOW);
  
  _FilterMillis = millis();
  _RamPositionUpdateMillis = _FilterMillis;
  _NewActionMillis = _FilterMillis;

  _Sts_Struct.LevelerMode = LVLR_STATUS_READY;
}

void Leveller_UR::InitRams(int LR0,int LR1,int RR0,int RR1,unsigned int SamplePeriod){
  _LeftRam.SetChannels(LR0,LR1,SamplePeriod);
  _LeftRam.SetLimits(LVLR_RAW_OFFSET_LEFT,LVLR_RAM_MIN_POSITION,LVLR_RAM_MAX_POSITION);
  _LeftRam.SetErrorCodes(NO_ERROR,LVLR_LEFT_TRACKS_OUT_OF_RANGE,LVLR_LEFT_SENSOR_DIFF_ERROR,LVLR_LEFT_SENSOR_FAIL);
  _LeftRam.EnableLPFilter(1.0f);

  _RightRam.SetChannels(RR0,RR1,SamplePeriod);
  _RightRam.SetLimits(LVLR_RAW_OFFSET_RIGHT,LVLR_RAM_MIN_POSITION,LVLR_RAM_MAX_POSITION);
  _RightRam.SetErrorCodes(NO_ERROR,LVLR_RIGHT_TRACKS_OUT_OF_RANGE,LVLR_RIGHT_SENSOR_DIFF_ERROR,LVLR_RIGHT_SENSOR_FAIL);
  _RightRam.EnableLPFilter(1.0f);
}

void Leveller_UR::Start(){
  _Enable = true;
}

void Leveller_UR::Stop(){
  _Enable = false;
}

unsigned char Leveller_UR::Update(){
  long _MillisNow = millis();
  _Sts_Struct.CurrentError = NO_ERROR; 
  
  if((_MillisNow - _FilterMillis) >= _FilterInterval){
    _FilterMillis = _MillisNow; 
    
    if(!digitalRead(_LeftProx)){
      _ProxLeftState = true;
      bitSet(_Sts_Struct.IO,LEFT_PROX_BIT);     
      _NegCounter++;    
      if(_NegCounter > LVLR_OUT_OF_LEVEL_COUNTER_THRESHOLD){
        _Sts_Struct.LevelState = LVLR_OUT_OF_TOLERANCE_NEGATIVE; 
      }  
    } else {
        _ProxLeftState = false;
        _NegCounter = 0;
        bitClear(_Sts_Struct.IO,LEFT_PROX_BIT);
      }
      
    if(!digitalRead(_RightProx)){
      _ProxRightState = true;
      bitSet(_Sts_Struct.IO,RIGHT_PROX_BIT);
      _PosCounter++;
      if(_PosCounter > LVLR_OUT_OF_LEVEL_COUNTER_THRESHOLD){
        _Sts_Struct.LevelState = LVLR_OUT_OF_TOLERANCE_POSITIVE; 
      }  
    } else {
        _ProxRightState = false;
        _PosCounter = 0;
        bitClear(_Sts_Struct.IO,RIGHT_PROX_BIT);
      }
     
    if(!_ProxLeftState && !_ProxRightState){
      _Sts_Struct.LevelState = LVLR_LEVEL;
    }
  }  
  
  _LeftRam.Update();
  if(_LeftRam.NewReading()){
    _RamPos.LeftRamPosition = _LeftRam.Output();
    _Sts_Struct.LeftStatus = _LeftRam.Status();
    if(_Sts_Struct.LeftStatus == LVLR_LEFT_SENSOR_FAIL){
      _Sts_Struct.LevelerMode = LVLR_STATUS_FAULT;      
    }    
  }
  
  _RightRam.Update();
  if(_RightRam.NewReading()){
    _RamPos.RightRamPosition = _RightRam.Output(); 
    _Sts_Struct.RightStatus = _RightRam.Status();
    if(_Sts_Struct.RightStatus == LVLR_RIGHT_SENSOR_FAIL){
      _Sts_Struct.LevelerMode = LVLR_STATUS_FAULT;
    } 
  }
  
  if((_MillisNow - _NewActionMillis) >= _NewActionInterval){    
    _NewActionMillis = _MillisNow;
    _LeftUpRelayState = LOW;
    _LeftDownRelayState = LOW;  
    _RightUpRelayState = LOW;
    _RightDownRelayState = LOW; 
      
    if(_Enable){
      if(digitalRead(_AutoPBPin)){
        _Sts_Struct.LevelerMode = LVLR_STATUS_MANUAL;
        digitalWrite(_AutoModeLedPin,LOW);
      } else if(_Sts_Struct.LeftStatus != LVLR_LEFT_SENSOR_FAIL && _Sts_Struct.RightStatus != LVLR_RIGHT_SENSOR_FAIL){         
          _Sts_Struct.LevelerMode = LVLR_STATUS_AUTO;       
          digitalWrite(_AutoModeLedPin,HIGH);
        } else {
            _Sts_Struct.LevelerMode = LVLR_STATUS_FAULT;
          }
      
      if(_Sts_Struct.LevelerMode == LVLR_STATUS_AUTO){          
        switch(RamState(_RamPos.LeftRamPosition)){
          case LVLR_ABOVE_TARGET_ZONE:
            if (_MasterLeft){
              _LeftUpRelayState = LOW;
              _LeftDownRelayState = HIGH; 
            } else {
                switch(_Sts_Struct.LevelState){
                  case LVLR_OUT_OF_TOLERANCE_NEGATIVE:                                    // If roll angle is out of tolerance negative adjust left side to correct
                    _LeftUpRelayState = HIGH;                                             // Deactivate Left up relay
                    _LeftDownRelayState = LOW;                                            // Activate Left down relay 
                    break;
                  case LVLR_OUT_OF_TOLERANCE_POSITIVE:                                    // If roll angle is out of tolerance positive adjust left side to correct 
                    _LeftUpRelayState = LOW;                                              // Activate Left up relay
                    _LeftDownRelayState = HIGH;                                           // Deactivate Left down relay
                    break;
                  case LVLR_LEVEL:                                                        // Roll angle is within tolerance
                    _LeftUpRelayState = LOW;                                              // Deactivate Left up relay
                    _LeftDownRelayState = LOW;                                            // Deactivate Left down relay
                    break;  
                }
              }
            break;
          case LVLR_WITHIN_TARGET_ZONE:
            if (_MasterLeft){
              _LeftUpRelayState = LOW;                                                    // Deactivate Left up relay
              _LeftDownRelayState = LOW;                                                  // Deactivate Left down relay
            } else {
                switch(_Sts_Struct.LevelState){
                  case LVLR_OUT_OF_TOLERANCE_NEGATIVE:                                    // If roll angle is out of tolerance negative adjust left side to correct
                    _LeftUpRelayState = HIGH;                                             // Deactivate Left up relay
                    _LeftDownRelayState = LOW;                                            // Activate Left down relay
                    break;
                  case LVLR_OUT_OF_TOLERANCE_POSITIVE:                                    // If roll angle is out of tolerance positive adjust left side to correct 
                    _LeftUpRelayState = LOW;                                              // Activate Left up relay
                    _LeftDownRelayState = HIGH;                                           // Deactivate Left down relay
                    break;
                  case LVLR_LEVEL:                                                        // Roll angle is within tolerance
                    _LeftUpRelayState = LOW;                                              // Deactivate Left up relay
                    _LeftDownRelayState = LOW;                                            // Deactivate Left down relay
                    break;
                  }
              }
            break;
          case LVLR_BELOW_TARGET_ZONE:
            _MasterLeft = true;
            _LeftUpRelayState = HIGH;                                                     // Activate Left up relay
            _LeftDownRelayState = LOW;                                                    // Deactivate Left down relay                  
            break;
        }
        switch(RamState(_RamPos.RightRamPosition)){
          case LVLR_ABOVE_TARGET_ZONE:
            if (!_MasterLeft){
              _RightUpRelayState = LOW;                                                   // Deactivate Right up relay
              _RightDownRelayState = HIGH;                                                // Activate Right down relay
            } else {
                switch(_Sts_Struct.LevelState){
                  case LVLR_OUT_OF_TOLERANCE_NEGATIVE:                                    // If roll angle is out of tolerance negative adjust Right side to correct
                    _RightUpRelayState = LOW;                                             // Activate Right up relay
                    _RightDownRelayState = HIGH;                                          // Deactivate Right down relay
                    break;
                  case LVLR_OUT_OF_TOLERANCE_POSITIVE:                                    // If roll angle is out of tolerance positive adjust Right side to correct 
                    _RightUpRelayState = HIGH;                                            // Deactivate Right Up relay
                    _RightDownRelayState = LOW;                                           // Activate Right Down relay
                    break;
                  case LVLR_LEVEL:                                                        // Roll angle is within tolerance
                    _RightUpRelayState = LOW;                                             // Deactivate Right up relay
                    _RightDownRelayState = LOW;                                           // Deactivate Right down relay
                    break;  
                }
              }
            break;
          case LVLR_WITHIN_TARGET_ZONE:
            if (!_MasterLeft){
              _RightUpRelayState = LOW;                                                   // Deactivate Right up relay
              _RightDownRelayState = LOW;                                                 // Deativate Right down relay
            } else {
                switch(_Sts_Struct.LevelState){
                  case LVLR_OUT_OF_TOLERANCE_NEGATIVE:                                    // If roll angle is out of tolerance negative adjust left side to correct
                    _RightUpRelayState = LOW;                                             // Deactivate Right up relay
                    _RightDownRelayState = HIGH;                                          // Activate Right down relay
                    break;
                  case LVLR_OUT_OF_TOLERANCE_POSITIVE:                                    // If roll angle is out of tolerance positive adjust left side to correct 
                    _RightUpRelayState = HIGH;                                            // Activate Right up relay
                    _RightDownRelayState = LOW;                                           // Deactivate Right down relay
                    break;
                  case LVLR_LEVEL:                                                        // Roll angle is within tolerance
                    _RightUpRelayState = LOW;                                             // Deactivate Right up relay
                    _RightDownRelayState = LOW;                                           // Deactivate Right down relay
                    break;
                }
              }
            break;
          case LVLR_BELOW_TARGET_ZONE:
            _MasterLeft = false;
            _RightUpRelayState = HIGH;                                                    // Activate Right up relay
            _RightDownRelayState = LOW;                                                   // Deactivate Right down relay                  
            break;
        }      
      }
    }     
    SetLeftUpRelay();
    SetLeftDownRelay();  
    SetRightUpRelay();
    SetRightDownRelay();
  }
  
  _LeftUpRelay.Update();
  _LeftDownRelay.Update();
  _RightUpRelay.Update();
  _RightDownRelay.Update();
  
  return _Sts_Struct.CurrentError;
}

unsigned char Leveller_UR::Mode(){
  return _Sts_Struct.LevelerMode;
}

boolean Leveller_UR::Master(){
  return _MasterLeft;
}

unsigned char Leveller_UR::LeftStatus(){
  return _Sts_Struct.LeftStatus;
}

float Leveller_UR::LeftRamPosition(){
  return _RamPos.LeftRamPosition;
}

unsigned char Leveller_UR::RightStatus(){
  return _Sts_Struct.RightStatus;
}

float Leveller_UR::RightRamPosition(){
  return _RamPos.RightRamPosition;
}

unsigned char Leveller_UR::LevelState(){
  return _Sts_Struct.LevelState;
}

void Leveller_UR::Leveller_Msg(void* StartPtr,unsigned char ActiveMsgs){
  _Sts_Struct.SeqCnt++;
  _Sts_Struct.ActiveMsgs = ActiveMsgs;
  bitWrite(_Sts_Struct.IO,MASTER_LEFT_BIT,_MasterLeft);
  bitWrite(_Sts_Struct.IO,MASTER_RIGHT_BIT,!_MasterLeft);
  memcpy(StartPtr,&_Sts_Struct,CAN_SIZE);
  _Sts_Struct.CurrentError =  NO_ERROR;
}

void Leveller_UR::RamPos_Msg(void* StartPtr){
  memcpy(StartPtr,&_RamPos,CAN_SIZE);
}

unsigned char Leveller_UR::CurrentError(){
  return _Sts_Struct.CurrentError;
}

void Leveller_UR::SetLeftUpRelay(){
  if(_LeftUpRelayState){
    _LeftUpRelay.On();
    bitSet(_Sts_Struct.IO,LEFT_UP_RELAY_BIT);
  } else {
      _LeftUpRelay.Off();
      bitClear(_Sts_Struct.IO,LEFT_UP_RELAY_BIT);
    }
}

void Leveller_UR::SetLeftDownRelay(){
  if(_LeftDownRelayState){
    _LeftDownRelay.On();
    bitSet(_Sts_Struct.IO,LEFT_DOWN_RELAY_BIT);
  } else {
      _LeftDownRelay.Off();
      bitClear(_Sts_Struct.IO,LEFT_DOWN_RELAY_BIT);
    }
}

void Leveller_UR::SetRightUpRelay(){
  if(_RightUpRelayState){
    _RightUpRelay.On();
    bitSet(_Sts_Struct.IO,RIGHT_UP_RELAY_BIT);
  } else {
      _RightUpRelay.Off();
      bitClear(_Sts_Struct.IO,RIGHT_UP_RELAY_BIT);
    } 
}

void Leveller_UR::SetRightDownRelay(){
  if(_RightDownRelayState){
    _RightDownRelay.On();
    bitSet(_Sts_Struct.IO,RIGHT_DOWN_RELAY_BIT);
  } else {
      _RightDownRelay.Off();
      bitClear(_Sts_Struct.IO,RIGHT_DOWN_RELAY_BIT);
    }
}

unsigned char Leveller_UR::RamState(float RamPos){
  if(RamPos < LVLR_TARGET_ZONE_LOWER){
    return LVLR_BELOW_TARGET_ZONE;
  } else if (RamPos > LVLR_TARGET_ZONE_UPPER){
      return LVLR_ABOVE_TARGET_ZONE;
    } else {
        return LVLR_WITHIN_TARGET_ZONE;    
      }
}

