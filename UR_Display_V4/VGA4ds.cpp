#include "VGA4ds.h"	

VGA4ds::VGA4ds(){
  _AckReq = false;
  _Enable = false;
  DigitalOut _rstpin;
}

void VGA4ds::Init(int Port,long Baud,int ResetPin){
  _Port = Port;
  _ResetPin = ResetPin;
  switch(_Port){
    case 0:
      Serial.begin(Baud);
      break;
    case 1:
      Serial1.begin(Baud); 
      break;
    case 2:
      Serial2.begin(Baud);
      break;
    case 3:
      Serial3.begin(Baud); 
      break;  
  }
  
  _rstpin.SetInitial(_ResetPin,LOW);
  ResetVGA();
}

void VGA4ds::ResetVGA(){
  _rstpin.SetOffFor(VGA4DS_RESET_PERIOD);
    
  for(int _i=0;_i<VGA4DS_OBJ_BUFFER_SIZE;_i++){
    _ObjBufStatus[_i] = false;
    _ObjBuf[_i].Id = VGA4DS_NO_OBJECT;
  }
  
  for(int _i=0;_i<VGA4DS_STR_BUFFER_SIZE;_i++){
    _StrBufStatus[_i] = false;
    _StrBuf[_i].Str = "";
  }
  
  _Enable = false;
  _RstMillis = millis(); 
}

boolean VGA4ds::AddObjToBuffer(VGA4dsObject* Obj){
  boolean _FreeSpace = false;
  for(int _i=0;_i<VGA4DS_OBJ_BUFFER_SIZE;_i++){
    if(!_ObjBufStatus[_i]){
      _ObjBufStatus[_i] = true;
      memcpy(&_ObjBuf[_i],Obj,sizeof(VGA4dsObject));
      _FreeSpace = true;
      _i = VGA4DS_OBJ_BUFFER_SIZE;
    }
  }
  return _FreeSpace;
}

boolean VGA4ds::AddStrToBuffer(VGA4dsString VGAStr){
  boolean _FreeSpace = false;
  for(int _i=0;_i<VGA4DS_STR_BUFFER_SIZE;_i++){
    if(!_StrBufStatus[_i]){
      _StrBufStatus[_i] = true;
      memcpy(&_StrBuf[_i],&VGAStr,sizeof(VGAStr));
      _FreeSpace = true;
      _i = VGA4DS_STR_BUFFER_SIZE;
    }
  }
  return _FreeSpace;
}

void VGA4ds::Update(){
  _rstpin.Update();
  
  if(!_Enable){
    if((millis() - _RstMillis) > VGA4DS_ENABLE_DELAY_PERIOD){
      _Enable = true;
      _AckReq = false;
    }
  } 
  
  if(ReceiveStatus()){
    byte _Rb = Readbyte();
    if(_Rb == VGA4DS_ACK || _Rb == VGA4DS_NAK){
      _AckReq = false;
    }
  } 
  
  if(_Enable && !_AckReq){
    for(int _i=0;_i<VGA4DS_OBJ_BUFFER_SIZE;_i++){
      if(_ObjBufStatus[_i]){
        WriteObject(_ObjBuf[_i]);
        _ObjBufStatus[_i] = false;
        _ObjBuf[_i].Id = VGA4DS_NO_OBJECT;
        _i = VGA4DS_OBJ_BUFFER_SIZE;
      }
    }
    if(!_AckReq){
      for(int _i=0;_i<VGA4DS_STR_BUFFER_SIZE;_i++){
        if(_StrBufStatus[_i]){
          WriteString(_StrBuf[_i]);
          _StrBufStatus[_i] = false;
          _StrBuf[_i].Str = "";
          _i = VGA4DS_STR_BUFFER_SIZE; 
        }  
      }
    }
  } 
}

void VGA4ds::WriteObject(VGA4dsObject Obj){
  byte _Buf[VGA4DS_WRITE_OBJ_LENGTH];
  _Buf[0] = VGA4DS_WRITE_OBJ; 
  _Buf[1] = Obj.Id;
  _Buf[2] = Obj.Index;
  _Buf[3] = Obj.Value >> 8;
  _Buf[4] = Obj.Value;
  _Buf[5] = 0;
  for(int _i=0;_i<VGA4DS_WRITE_OBJ_LENGTH-1;_i++){
    _Buf[5] ^= _Buf[_i];
  } 
  WriteCmd(&_Buf[0],VGA4DS_WRITE_OBJ_LENGTH); 
  _AckReq = true;
  _AckMillis = millis(); 
}

void VGA4ds::WriteString(VGA4dsString VGAStr){
  int _StrLen = VGAStr.Str.length();
  int _MsgLen = _StrLen + VGA4DS_WRITE_STR_LENGTH;
  byte _TempBuf[_MsgLen];
  char _TempStrBuf[_StrLen+1];
  
  VGAStr.Str.toCharArray(_TempStrBuf,_StrLen+1);
  _TempBuf[0] = VGA4DS_WRITE_STR;
  _TempBuf[1] = VGAStr.Index;
  
  for(int i=0;i<_StrLen;i++){
    _TempBuf[i+2] = _TempStrBuf[i];
  }
  _TempBuf[_MsgLen-1] = 0;
  
  for(int _i=0;_i<_MsgLen-1;_i++){
    _TempBuf[_MsgLen-1] ^= _TempBuf[_i];
  }
  WriteCmd(&_TempBuf[0],_MsgLen);
  _AckReq = true;
  _AckMillis = millis();
}

void VGA4ds::WriteCmd(byte* StartByte,int NumBytes){  
  switch(_Port){
    case 0:
      for(int _i=0;_i<NumBytes;_i++){
        Serial.write(*StartByte);
        *StartByte++;
      }
      break;
    case 1:
      for(int _i=0;_i<NumBytes;_i++){
        Serial1.write(*StartByte);
        *StartByte++;
      }
      break;
    case 2:
      for(int _i=0;_i<NumBytes;_i++){
        Serial2.write(*StartByte);
        *StartByte++;
      }
      break;
    case 3:
      for(int _i=0;_i<NumBytes;_i++){
        Serial3.write(*StartByte);
        *StartByte++;
      }
      break;  
  } 
} 

byte VGA4ds::Readbyte(){
  switch(_Port){
    case 0:
      return Serial.read();
      break;
    case 1:
      return Serial1.read();
      break;    
    case 2:
      return Serial2.read();
      break;
    case 3:
      return Serial3.read();
      break; 
  }     
}

boolean VGA4ds::ReceiveStatus(){
  boolean _RetVal = false;
  switch(_Port){
    case 0:
      if (Serial.available()){
        _RetVal = true;
      }
      break;
    case 1:
      if (Serial1.available()){
        _RetVal = true;
      }
      break;    
    case 2:
      if (Serial2.available()){
        _RetVal = true;
      }
      break;
    case 3:
      if (Serial3.available()){
        _RetVal = true;
      }
      break; 
  }     
  return _RetVal;
}

