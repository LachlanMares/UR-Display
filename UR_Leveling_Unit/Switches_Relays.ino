
void InitSwitchesRelays(){
  AutoModeButton(1,25mS);
  LeftUpButton(1,25mS);
  LeftDownButton(1,25mS);
  RightUpButton(1,25mS);
  RightDownButton(1,25mS);
  IO_Msg.Relay1 = LeftUpRelay.SetInitial(LOW);
  IO_Msg.Relay2 = LeftDownRelay.SetInitial(LOW);
  IO_Msg.Relay3 = RightUpRelay.SetInitial(LOW);
  IO_Msg.Relay4 = RightDownRelay.SetInitial(LOW);
}

void UpdateSwitchesRelays(){
  if (LeftSideSwitches != NO_CMD || RightSideSwitches != NO_CMD){         // Manual command registered, either manual or auto modes
    switch(LeftSideSwitches){
      case NO_CMD:  
        SetLeftUpRelay(LOW);                                              // Deactivate Left up relay
        SetLeftDownRelay(LOW);                                            // Deactivate Left down relay   
        break;
      case UP_CMD:
        SetLeftUpRelay(HIGH);                                             // Activate Left up relay
        SetLeftDownRelay(LOW);                                            // Deactivate Left down relay  
        break;
      case DOWN_CMD:
        SetLeftUpRelay(LOW);                                              // Deactivate Left up relay
        SetLeftDownRelay(HIGH);                                           // Activate Left down relay
        break;
      case SW_ERROR:
        //SetLeftUpRelay(LOW);                                            // Deactivate Left up relay
        //SetLeftDownRelay(LOW);                                          // Deactivate Left down relay
        break;  
    }
    switch(RightSideSwitches){   
      case NO_CMD:
        SetRightUpRelay(LOW);                                             // Deactivate Right up relay
        SetRightDownRelay(LOW);                                           // Deactivate Right down relay
        break;
      case UP_CMD:
        SetRightUpRelay(HIGH);                                            // Deactivate Right up relay
        SetRightDownRelay(LOW);                                           // Activate Right down relay
        break;
      case DOWN_CMD:                                                      
        SetRightUpRelay(LOW);                                             // Deactivate Right up relay
        SetRightDownRelay(HIGH);                                          // Activate Right down relay
        break;    
      case SW_ERROR:                                                      
        //SetRightUpRelay(LOW);                                           // Deactivate Right up relay
        //SetRightDownRelay(LOW);                                         // Deactivate Right down relay
        break;
    }           
  } else if (AutoMode && StsMsg.Mode != STATUS_FAULT){                    // Auto mode selected, and no fault
      switch(LeftSideStatus){
        case BOTH_OFF:        
          SetLeftUpRelay(LOW);                                            // Deactivate Left up relay
          SetLeftDownRelay(LOW);                                          // Deactivate Left down relay
          break;
        case P1_NOT_P2:
          if (!LevelMaster){
            SetLeftUpRelay(LOW);                                          // Deactivate Left up relay
            SetLeftDownRelay(HIGH);                                       // Activate Left down relay
          } else {
              if (OutOfToleranceNegative){                                // If roll angle is out of tolerance negative adjust left side to correct
                SetLeftUpRelay(LOW);                                      // Activate Left up relay
                SetLeftDownRelay(HIGH);                                   // Deactivate Left down relay
            } else if (OutOfTolerancePositive){                           // If roll angle is out of tolerance positive adjust left side to correct 
                SetLeftUpRelay(HIGH);                                     // Deactivate Left up relay
                SetLeftDownRelay(LOW);                                    // Activate Left down relay
              } else {                                                    // Roll angle is within tolerance
                  SetLeftUpRelay(LOW);                                    // Deactivate Left up relay
                  SetLeftDownRelay(LOW);                                  // Deactivate Left down relay
                }
            }
          break;
        case BOTH_ON:
          if (!LevelMaster){
            SetLeftUpRelay(LOW);                                          // Deactivate Left up relay
            SetLeftDownRelay(LOW);                                        // Activate Left down relay
          } else {
              if (OutOfToleranceNegative){                                // If roll angle is out of tolerance negative adjust left side to correct
                SetLeftUpRelay(LOW);                                      // Activate Left up relay
                SetLeftDownRelay(HIGH);                                   // Deactivate Left down relay
            } else if (OutOfTolerancePositive){                           // If roll angle is out of tolerance positive adjust left side to correct 
                SetLeftUpRelay(HIGH);                                     // Deactivate Left up relay
                SetLeftDownRelay(LOW);                                    // Activate Left down relay
              } else {                                                    // Roll angle is within tolerance
                  SetLeftUpRelay(LOW);                                    // Deactivate Left up relay
                  SetLeftDownRelay(LOW);                                  // Deactivate Left down relay
                }
            }
          break;
        case P2_NOT_P1:
          SetMaster(false);
          SetLeftUpRelay(HIGH);                                           // Activate Left up relay
          SetLeftDownRelay(LOW);                                          // Deactivate Left down relay                  
          break;
      }
      
      switch(RightSideStatus){
        case BOTH_OFF:
          SetRightUpRelay(LOW);                                           // Deactivate Right up relay
          SetRightDownRelay(LOW);                                         // Deactivate Right down relay
          break;
        case P1_NOT_P2:
          if (LevelMaster){  
            SetRightUpRelay(LOW);                                         // Deactivate Right up relay
            SetRightDownRelay(HIGH);                                      // Activate Right down relay
          } else {
              if (OutOfToleranceNegative){                                // If roll angle is out of tolerance negative adjust left side to correct
                SetRightUpRelay(HIGH);                                    // Activate Right up relay
                SetRightDownRelay(LOW);                                   // Deactivate Right down relay
              } else if (OutOfTolerancePositive){                           // If roll angle is out of tolerance positive adjust left side to correct 
                  SetRightUpRelay(LOW);                                   // Deactivate Right up relay
                  SetRightDownRelay(HIGH);                                // Activate Right down relay
                } else {                                                    // Roll angle is within tolerance
                  SetRightUpRelay(LOW);                                 // Deactivate Right up relay
                  SetRightDownRelay(LOW);                               // Deactivate Right down relay 
                }
            }
          break;      
        case BOTH_ON:
          if (LevelMaster){  
            SetRightUpRelay(LOW);                                         // Deactivate Right up relay
            SetRightDownRelay(LOW);                                       // Activate Right down relay
          } else {
              if (OutOfToleranceNegative){                                // If roll angle is out of tolerance negative adjust left side to correct
                SetRightUpRelay(HIGH);                                    // Activate Right up relay
                SetRightDownRelay(LOW);                                   // Deactivate Right down relay
              } else if (OutOfTolerancePositive){                           // If roll angle is out of tolerance positive adjust left side to correct 
                  SetRightUpRelay(LOW);                                   // Deactivate Right up relay
                  SetRightDownRelay(HIGH);                                // Activate Right down relay
                } else {                                                    // Roll angle is within tolerance
                    SetRightUpRelay(LOW);                                 // Deactivate Right up relay
                    SetRightDownRelay(LOW);                               // Deactivate Right down relay 
                  }
            }
          break;
        case P2_NOT_P1:
          SetMaster(true);
          SetRightUpRelay(HIGH);                                           // Activate Left up relay
          SetRightDownRelay(LOW);                                          // Deactivate Left down relay                  
          break;  
      }           
    } else { 
        SetLeftUpRelay(LOW);                                              // Deactivate Left up relay
        SetLeftDownRelay(LOW);                                            // Deactivate Left down relay
        SetRightUpRelay(LOW);                                             // Deactivate Right up relay
        SetRightDownRelay(LOW);                                           // Deactivate Right down relay 
      }
}

void SetLeftUpRelay(boolean State){
  digitalWrite(LeftUpRelay,State);
  StsMsg.LeftUp = State;
}

void SetLeftDownRelay(boolean State){
  digitalWrite(LeftDownRelay,State);
  StsMsg.LeftDown = State;
}

void SetRightUpRelay(boolean State){
  digitalWrite(RightUpRelay,State);
  StsMsg.RightUp = State;
}

void SetRightDownRelay(boolean State){
  digitalWrite(RightDownRelay,State);
  StsMsg.RightDown = State;
}

void SetMaster(boolean State){
  LevelMaster = State;
  StsMsg.Master = State;
}
