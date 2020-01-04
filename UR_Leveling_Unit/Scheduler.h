#ifndef Scheduler_h
#define Scheduler_h
#include "Arduino.h"

class Scheduler
{
public:
	Scheduler(int,int,int,int,int,int);
        void Start();
        void Stop();
        void Scheduler_ISR();
        boolean Timer_0_Ready();
        boolean Timer_1_Ready();
        boolean Timer_2_Ready();
        boolean Timer_3_Ready();
        boolean Timer_4_Ready();
private:
        boolean _Enable;
        boolean _Timer_0_Ready,_Timer_1_Ready,_Timer_2_Ready,_Timer_3_Ready,_Timer_4_Ready;
        int _Timer_0_Counter,_Timer_0_Compare;  
        int _Timer_1_Counter,_Timer_1_Compare;
        int _Timer_2_Counter,_Timer_2_Compare;
        int _Timer_3_Counter,_Timer_3_Compare;
        int _Timer_4_Counter,_Timer_4_Compare;
};

#endif
