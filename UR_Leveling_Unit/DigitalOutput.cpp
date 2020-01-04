#include "DigitalOutput.h"	

DigitalOutput::DigitalOutput(int PinNumber,int IntTime){
  _UseDelay = false;
  _NewWriteReq = false;
  _PinNumber = PinNumber;
  _IntTime = IntTime;
}

boolean DigitalOutput::SetInitial(boolean State){
  pinMode(_PinNumber, OUTPUT);
  digitalWrite(_PinNumber,State);
  _CurrentState = State;
  return _CurrentState;
}

boolean DigitalOutput::On(){
  _UseDelay = false;
  _CurrentState = HIGH;
  digitalWrite(_PinNumber,_CurrentState);
  return _CurrentState;
}

boolean DigitalOutput::Off(){
  _UseDelay = false;
  _CurrentState = LOW;
  digitalWrite(_PinNumber,_CurrentState);
  return _CurrentState;
}

boolean DigitalOutput::SetOnFor(int TimerValue){
  if(TimerValue > 0){
    _UseDelay = true;
    _NextState = LOW;
    _CurrentState = HIGH;
    digitalWrite(_PinNumber,_CurrentState);
    _DelayCounter = 0;
    _DelayCompare = TimerValue/_IntTime;
  } else {
      _CurrentState = LOW;
      digitalWrite(_PinNumber,_CurrentState);
    }
  return _CurrentState;
}

boolean DigitalOutput::SetOffFor(int TimerValue){
  if(TimerValue > 0){
    _UseDelay = true;
    _NextState = HIGH;
    _CurrentState = LOW;
    digitalWrite(_PinNumber,_CurrentState);
    _DelayCounter = 0;
    _DelayCompare = TimerValue/_IntTime;
  } else {
      _CurrentState = HIGH;
      digitalWrite(_PinNumber,HIGH);
    }
  return _CurrentState;
}

void DigitalOutput::_ISR(){
  if(_UseDelay){
    _NewWriteReq = true;
  } 
}

boolean DigitalOutput::Update(){
  if(_NewWriteReq){
    _NewWriteReq = false;
    _DelayCounter++;
    if(_DelayCounter >= _DelayCompare){
      _CurrentState = _NextState;
      digitalWrite(_PinNumber,_CurrentState);
      _UseDelay = false;
    } 
  } 
  return _CurrentState;
}





