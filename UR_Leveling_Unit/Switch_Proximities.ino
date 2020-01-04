
void SwitchAndProxStatus(){  
  StsMsg.LeftCmdUp    = !digitalRead(LeftUpButton);                         
  StsMsg.LeftCmdDown  = !digitalRead(LeftDownButton);
  StsMsg.RightCmdUp   = !digitalRead(RightUpButton);
  StsMsg.RightCmdDown = !digitalRead(RightDownButton);
  AutoMode = !digitalRead(AutoModeButton);
  
  if (AutoMode){
    StsMsg.Mode = STATUS_AUTO; 
  } else {
      StsMsg.Mode = STATUS_MANUAL;
    }
  
  if (StsMsg.LeftCmdUp || StsMsg.LeftCmdDown){                        // Manual command registered, either manual or auto modes
    StsMsg.Mode = STATUS_USER_CMD; 
    if (StsMsg.LeftCmdUp && !StsMsg.LeftCmdDown){                     // Left up button pressed
      LeftSideSwitches = UP_CMD;
    } else if (!StsMsg.LeftCmdUp && StsMsg.LeftCmdDown){              // Left down button pressed
        LeftSideSwitches = DOWN_CMD;
      } else {                                                        // If both switches on   
          LeftSideSwitches = SW_ERROR;
          Serial.println("Left Switch Error");
          ProcessError(LEFT_SWITCH_ERROR);                            // Process error into log
        }                   
  } else {
      LeftSideSwitches = NO_CMD;
    }
  if (StsMsg.RightCmdUp || StsMsg.RightCmdDown){
    StsMsg.Mode = STATUS_USER_CMD; 
    if (StsMsg.RightCmdUp && !StsMsg.RightCmdDown){                   // Right up button pressed
      RightSideSwitches = UP_CMD;
    } else if (!StsMsg.RightCmdUp && StsMsg.RightCmdDown){            // Right down button pressed
        RightSideSwitches = DOWN_CMD;
      } else {                                                        // If both switches on       
          RightSideSwitches = SW_ERROR;
          Serial.println("Right Switch Error");
          ProcessError(RIGHT_SWITCH_ERROR);                           // Process error into log                    
        }
  } else {
      RightSideSwitches = NO_CMD;
    }
    
  if (LeftSideSwitches != NO_CMD || RightSideSwitches != NO_CMD){     // If a manual command is given update relays
    UpdateRelays();
  }

  StsMsg.LeftProx1 = !digitalRead(LeftProx1);                         // Read digitals once
  StsMsg.LeftProx2 = !digitalRead(LeftProx2);
  StsMsg.RightProx1 = !digitalRead(RightProx1); 
  StsMsg.RightProx2 = !digitalRead(RightProx2);     

  if (!StsMsg.LeftProx1 && !StsMsg.LeftProx2){                        // There should always be at least one one prox on
    LeftProxCounter++;                                                // Increment Left Prox Counter
    if (LeftProxCounter >= PROXIMITY_ERROR){                          // Check to see if counter has reached threshold
      LeftSideStatus = BOTH_OFF;
      Serial.println("Left Prox Error");
      ProcessError(LEFT_PROX_ERROR);                                  // Process error into log
    }
  } else {
      LeftProxCounter = 0;                                            // Reset left prox counter 
      if (!StsMsg.LeftProx1 && StsMsg.LeftProx2){                     // if Prox2 and not Prox1, this means the left side is too low 
        LeftSideStatus = P2_NOT_P1;
      } else if (StsMsg.LeftProx1 && StsMsg.LeftProx2){               // When master both proxs means it is at optimal height
          LeftSideStatus = BOTH_ON;
        } else if (StsMsg.LeftProx1 && !StsMsg.LeftProx2){                                
            LeftSideStatus = P1_NOT_P2;
          } 
    }         
  if (!StsMsg.RightProx1 && !StsMsg.RightProx2){                      // There should always be at least one one prox on
    RightProxCounter++;                                               // Increment Right Prox Counter
    if (RightProxCounter >= PROXIMITY_ERROR){                         // Check to see if counter has reached threshold
      RightSideStatus = BOTH_OFF;
      Serial.println("Right Prox Error");
      ProcessError(RIGHT_PROX_ERROR);                                 // Process error into log
    }
  } else {
      RightProxCounter = 0;                                           // Reset right prox counter   
      if (!StsMsg.RightProx1 && StsMsg.RightProx2){                   // if Prox2 and not Prox1, this means the right side is too low 
        RightSideStatus = P2_NOT_P1;
      } else if (StsMsg.RightProx1 && StsMsg.RightProx2){             // When master both proxs means it is at optimal height
          RightSideStatus = BOTH_ON;
        } else if (StsMsg.RightProx1 && !StsMsg.RightProx2){          // When master with Prox1 and not Prox2, this means master side is too high
            RightSideStatus = P1_NOT_P2;
          }
    }
  if (LeftSideSwitches == SW_ERROR || RightSideSwitches == SW_ERROR || LeftSideStatus == BOTH_OFF || RightSideStatus == BOTH_OFF){
    StsMsg.Mode = STATUS_FAULT;
  }
}
