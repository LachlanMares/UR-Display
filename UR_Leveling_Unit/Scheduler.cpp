#include "Scheduler.h"	

Scheduler::Scheduler(int IntTime,int Time_0,int Time_1, int Time_2,int Time_3,int Time_4){
  _Enable = false;
  _Timer_0_Ready = false;
  _Timer_0_Counter = 0;
  _Timer_0_Compare = IntTime/Time_0;
  _Timer_1_Ready = false;
  _Timer_1_Counter = 0;
  _Timer_1_Compare = IntTime/Time_1;
  _Timer_2_Ready = false;
  _Timer_2_Counter = 0;
  _Timer_2_Compare = IntTime/Time_2;
  _Timer_3_Ready = false;
  _Timer_3_Counter = 0;
  _Timer_3_Compare = IntTime/Time_3;
  _Timer_4_Ready = false; 
  _Timer_4_Counter = 0;
  _Timer_4_Compare = IntTime/Time_4;
}

void Scheduler::Start(){
  _Enable = true;
}

void Scheduler::Stop(){
  _Enable = false;
}

void Scheduler::Scheduler_ISR(){
  if(_Enable){
    _Timer_0_Counter++;
    _Timer_1_Counter++;
    _Timer_2_Counter++;
    _Timer_3_Counter++;
    _Timer_4_Counter++;
 
    if(_Timer_0_Counter == _Timer_0_Compare){                                    
      _Timer_0_Ready = true;
      _Timer_0_Counter = 0;
    }
    if(_Timer_1_Counter == _Timer_1_Compare){                                    
      _Timer_1_Ready = true;
      _Timer_1_Counter = 0;
    }  
    if(_Timer_2_Counter == _Timer_2_Compare){                                    
      _Timer_2_Ready = true;
      _Timer_2_Counter = 0;
    }
    if(_Timer_3_Counter == _Timer_3_Compare){                                    
      _Timer_3_Ready = true;
      _Timer_3_Counter = 0;
    }
    if(_Timer_4_Counter == _Timer_4_Compare){                                    
      _Timer_4_Ready = true;
      _Timer_4_Counter = 0;
    }
  }
} 

boolean Scheduler::Timer_0_Ready(){
  boolean RetVal = false;
  if(_Timer_0_Ready){
   _Timer_0_Ready = false;
   RetVal = true;
  }
 return RetVal; 
}

boolean Scheduler::Timer_1_Ready(){
  boolean RetVal = false;
  if(_Timer_1_Ready){
   _Timer_1_Ready = false;
   RetVal = true;
  }
 return RetVal; 
}

boolean Scheduler::Timer_2_Ready(){
  boolean RetVal = false;
  if(_Timer_2_Ready){
   _Timer_2_Ready = false;
   RetVal = true;
  }
 return RetVal; 
}

boolean Scheduler::Timer_3_Ready(){
  boolean RetVal = false;
  if(_Timer_3_Ready){
   _Timer_3_Ready = false;
   RetVal = true;
  }
 return RetVal; 
}

boolean Scheduler::Timer_4_Ready(){
  boolean RetVal = false;
  if(_Timer_4_Ready){
   _Timer_4_Ready = false;
   RetVal = true;
  }
 return RetVal; 
}
