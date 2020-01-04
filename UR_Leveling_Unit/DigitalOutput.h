#ifndef DigitalOutput_h
#define DigitalOutput_h
#include "Arduino.h"

class DigitalOutput
{
public:
	DigitalOutput(int,int);
        boolean SetInitial(boolean);
        boolean On();
        boolean Off();
        boolean SetOnFor(int);
        boolean SetOffFor(int);        
        void _ISR();
        boolean Update();
        
private:
        boolean _UseDelay,_CurrentState,_NextState,_NewWriteReq;
        int _PinNumber,_IntTime,_DelayCounter,_DelayCompare;
};

#endif
