 
void SerialStatus(){
  if (Serial3.available() > 2){
    Buffer[0] = Serial3.read();
    if (Buffer[0] == STX){
      Buffer[1] = Serial3.read();
      Buffer[2] = Serial3.read();
      if (ValidMsg(Buffer[2])){
        for(int i=3;i<Buffer[1];i++){
          Buffer[i] = Serial3.read();
        }  
        ProcessMsg();
      }
    }
  }
  if (SendStatusMsg){
    UpdateStatusMsg();
    byte b[sizeof(StatusMsg)];
    memcpy(b,&StsMsg,sizeof(StatusMsg));
    for (int i=0;i<sizeof(StatusMsg);i++){
      Serial3.write(b[i]);
    }
    SendStatusMsg = false; 
  }
}

boolean ValidMsg(byte Type){
  boolean ValidFlag = false;
  switch(Type){
    case SERVER_MSG:
      ValidFlag = true;
      break;
  }
  return ValidFlag;
}

void ProcessMsg(){
  switch(Buffer[2]){    
    case SERVER_MSG:
      memcpy(&SvrMsg, Buffer, sizeof(ServerMsg));
      break;      
  }
}

void UpdateStatusMsg(){
  StsMsg.Pitch = TO_DEG(euler.phi);                                                                                                        // Update status message with new euler angles
  StsMsg.Roll = TO_DEG(euler.theta);
  StsMsg.Heading = TO_DEG(euler.psi);                                                                                                      
  gps.f_get_position(&StsMsg.Latitude,&StsMsg.Longitude,&StsMsg.Age);                                                                      // Update status message with GPS position data
  gps.crack_datetime(&StsMsg.Year,&StsMsg.Month,&StsMsg.Day,&StsMsg.Hour,&StsMsg.Minute,&StsMsg.Second,&StsMsg.Hundredths,&StsMsg.Age);    // Update status message with GPS time data
  StsMsg.Minute += GMT_MINUTE;                                                                                                             // Adjust for current timezone
  if (StsMsg.Minute > 59){                                                     
    StsMsg.Minute -= 60;
    StsMsg.Hour += 1;  
  }
  StsMsg.Hour += GMT_HOUR;
  if (StsMsg.Hour > 23){
    StsMsg.Hour -= 24;
    StsMsg.Day += 1;
  }    
  StsMsg.Speed = gps.f_speed_kmph();                                                                                                       // Update status message with GPS speed
}

