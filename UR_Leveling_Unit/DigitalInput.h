#ifndef DigitalInput_h
#define DigitalInput_h
#include "Arduino.h"


class DigitalInput
{
public:
	DigitalInput(int,int);
        void SetMode(int,int);
        void SetDebounce(int);
        void StopDebounce();
        void _ISR();
        void Update();
        boolean Read();
        
private:
        boolean _UseDebounce,_PrevState,_CurrentState,_NewReadReq;
        int _PinNumber,_IntTime,_DebounceCounter,_DebounceCompare;
};

#endif
