#include "DigitalInput.h"	

DigitalInput::DigitalInput(int PinNumber,int IntTime){
  _UseDebounce = false;
  _PinNumber = PinNumber;
  _IntTime = IntTime;
}

void DigitalInput::SetMode(int Mode,int DebounceTime){
  switch(Mode){
    case 0:
      pinMode(_PinNumber,INPUT);
      break;
    case 1:
      pinMode(_PinNumber,INPUT_PULLUP);
      break;
  }
  if(DebounceTime > 0){
    _UseDebounce = true;
    _DebounceCounter = 0;
    _DebounceCompare = DebounceTime/_IntTime;
  } else {
      _UseDebounce = false;
    }
}

void DigitalInput::SetDebounce(int DebounceTime){
  _UseDebounce = true;
  _DebounceCounter = 0;
  _DebounceCompare = DebounceTime/_IntTime;
}

void DigitalInput::StopDebounce(){
  _UseDebounce = false;
}

void DigitalInput::_ISR(){
  _NewReadReq = true;
}

void DigitalInput::Update(){
  if(_NewReadReq){
    _NewReadReq = false;
    boolean _NewState = digitalRead(_PinNumber);
    if(_UseDebounce){
      if(_NewState == _PrevState){
        _DebounceCounter++;
        if(_DebounceCounter >= _DebounceCompare){
          _CurrentState = _NewState;
        } 
      } else {
          _DebounceCounter = 0;
        }
      _PrevState = _NewState;
    } else {
        _CurrentState = _NewState;
      }        
  }
}

boolean DigitalInput::Read(){
  return _CurrentState;
}



