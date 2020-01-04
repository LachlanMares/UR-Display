void InitFaultLog(){
  faultlog.Init(MASTER_FAULTLOG_SIZE,MASTER_FAULTLOG_RESET_INTERVAL);            // Initalise master faultlog, set time interval for clearing fault
  FaultLogMillis = millis();                                                     
}

void UpdateFaultlog(){
  faultlog.Update();                                                             // Update faultlog functions
  if((millis() - FaultLogMillis) >= MASTER_ERROR_UPDATE_INTERVAL){               // If set update interval has expired 
    FaultLogMillis = millis();    
    faultlog.GetFaultLog(&FaultBuffer);                                          // Load faults from faultlog into a local array
  }
}
