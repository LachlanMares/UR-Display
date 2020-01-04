#include "Canbus.h"

Canbus::Canbus(){
  _Enable = false;
  _NewMsgReady = false;
}

void Canbus::Start(){
  _Enable = true;
}

void Canbus::Stop(){
  _Enable = false;
}

void Canbus::CAN_ISR(){
  _NewMsgReady = true;
}
