#ifndef Canbus_h
#define Canbus_h

#include <stdio.h>
#include "Arduino.h"
#include "GlobalStructs.h"

class Canbus
{
public:
	Canbus();  
        void Start();
        void Stop();
        void CAN_ISR();
        
private:
        boolean _Enable,_NewMsgReady;
};

#endif
