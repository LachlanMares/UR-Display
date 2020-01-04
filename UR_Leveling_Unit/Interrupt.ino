
void ExtInt_0_ISR(){
  ahrs.AHRS_ISR();
}

void ExtInt_1_ISR(){
  can.CAN_ISR();
}

void Timer2_ISR(){                                                      
  scheduler.Scheduler_ISR();
}

