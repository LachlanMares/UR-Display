
void MCP2515_ISR(){
  NewCANMsg = true;                                                                                                     // External interrupt triggered by MCP2515
}

void CANUpdate(){
  if(NewCANMsg){                                                                                                        // If an interrupt has been triggered 
    NewCANMsg = false;
    boolean TempBit;
    unsigned char NumOfMsgs = canbus.CheckReceiveBuffers();                                                             // MCP2515 can store 2 recieved messages  
    StdCANStruct CANTemp;                                                                                               // Allocate memory to store CAN message
    
    for(unsigned char i=0;i<NumOfMsgs;i++){                                                                
      canbus.ReadMsg(&CANTemp.CANid,&CANTemp.EXTid,&CANTemp.Len,&CANTemp.Buffer);                                       // Extract first message from MCP2515 buffer

      switch(CANTemp.CANid){                                                                                            // Examine Msg ID 
        case LVLR_STATUS_MSG_ID:                                                                           
          memcpy(&leveling_unit.StsMsg,&CANTemp.Buffer,CAN_SIZE);                                                       // Copy Buffer into leveling unit structure
          leveling_unit.CanMon.LastMsgMillis = millis();                                                                // Track message recieve time with CAN monitor
          if(!leveling_unit.CanMon.FirstMsg){                                                                           // Note first message recieved
            leveling_unit.CanMon.FirstMsg = true;
            leveling_unit.CanMon.SeqCnt = leveling_unit.StsMsg.SeqCnt;                                                  // Align sequence counters first instance
            vga.AddObjToBuffer(&left_level_led_4ds);                                                                    // Set initial values
            vga.AddObjToBuffer(&right_level_led_4ds);    
            vga.AddObjToBuffer(&master_left_led_4ds);
            vga.AddObjToBuffer(&master_right_led_4ds);
            vga.AddObjToBuffer(&left_ram_position_4ds);
            vga.AddObjToBuffer(&right_ram_position_4ds);
            vga.AddObjToBuffer(&leveler_mode_4ds);          
          } else if(leveling_unit.StsMsg.SeqCnt - leveling_unit.CanMon.SeqCnt > MAX_MISSING_MSGS){                      // Try to determine if messaged are going missing, add fault if too many
              faultlog.AddFault(MASTER_LEVELER_STATUS_MSGS_MISSING);
            }
          leveling_unit.CanMon.SeqCnt = leveling_unit.StsMsg.SeqCnt;                                                    // Align sequence counters    
          
          left_level_led_4ds.Value = bitRead(leveling_unit.StsMsg.IO,4);                                                  // Read bit from stsmsg io                                                                    
          vga.AddObjToBuffer(&left_level_led_4ds);                                                                      // Add 4DS objects to send buffer
          right_level_led_4ds.Value = bitRead(leveling_unit.StsMsg.IO,5);
          vga.AddObjToBuffer(&right_level_led_4ds);
          TempBit = bitRead(leveling_unit.StsMsg.IO,6);
          if(master_left_led_4ds.Value != TempBit){
            master_left_led_4ds.Value = TempBit;
            vga.AddObjToBuffer(&master_left_led_4ds);
          }
          TempBit = bitRead(leveling_unit.StsMsg.IO,7);
          if(master_right_led_4ds.Value != TempBit){
            master_right_led_4ds.Value = TempBit;              
            vga.AddObjToBuffer(&master_right_led_4ds);
          }     
          leveler_mode_4ds.Value = leveling_unit.StsMsg.LevelerMode;
          vga.AddObjToBuffer(&leveler_mode_4ds);        
          break;
        
        case LVLR_RAM_POSITIONS_MSG_ID:                                                                    
          memcpy(&leveling_unit.Positions,&CANTemp.Buffer,CAN_SIZE);                                                  // Copy Buffer into leveling unit structure                         
          left_ram_position_4ds.Value = (unsigned int)(leveling_unit.Positions.LeftRamPosition);                      
          vga.AddObjToBuffer(&left_ram_position_4ds);                                                                 // Add 4DS objects to send buffer
          right_ram_position_4ds.Value = (unsigned int)(leveling_unit.Positions.RightRamPosition);
          vga.AddObjToBuffer(&right_ram_position_4ds);
          break;      
        
        case LVLR_ROLL_PITCH_MSG_ID:
          memcpy(&leveling_unit.RollPitch,&CANTemp.Buffer,CAN_SIZE);                                                    // Copy Buffer into leveling unit structure
          break;  
        
        case LVLR_FAULTLOG_MSG_ID:
          memcpy(&leveling_unit.Log.Buffer,&CANTemp.Buffer,CAN_SIZE);                                                   // Copy Buffer into leveling unit structure
          for(int i=0;i<CAN_SIZE;i++){
            if(leveling_unit.Log.Buffer[i] != NO_ERROR){                                                                // Add leveler faults to master faultlog, if they exist
              faultlog.AddFault(leveling_unit.Log.Buffer[i]);
            }    
          }
          break;  
        
        case LVLR_POSITION_MSG_ID:
          memcpy(&leveling_unit.GPS,&CANTemp.Buffer,CAN_SIZE);                                                          // Copy Buffer into leveling unit structure
          break;    
        
        case LVLR_DATETIME_MSG_ID:
          memcpy(&leveling_unit.DateTime,&CANTemp.Buffer,CAN_SIZE);                                                     // Copy Buffer into leveling unit structure         
          minutes_4ds.Value = leveling_unit.DateTime.Minute;
          vga.AddObjToBuffer(&minutes_4ds);
          hours_4ds.Value = leveling_unit.DateTime.Hour;
          vga.AddObjToBuffer(&hours_4ds);
          date_4ds.Value = leveling_unit.DateTime.Day;
          vga.AddObjToBuffer(&date_4ds);
          month_4ds.Value = leveling_unit.DateTime.Month;
          vga.AddObjToBuffer(&month_4ds);
          break;      
        
        case IO_UNIT_STATUS_MSG_ID:
          switch (CANTemp.Buffer[0]){                                                                                   // IO status msg byte 0 is CAN Address
            case CAN_IO_BOARD_0_ID:
              memcpy(&io_unit_0.StsMsg,&CANTemp.Buffer,CAN_SIZE);                                                       // Copy Buffer into io unit 0 structure
              io_unit_0.CanMon.LastMsgMillis = millis();                                                                // Track message recieve time with CAN monitor
              if(!io_unit_0.CanMon.FirstMsg){                                                                           // Note first message recieved
                io_unit_0.CanMon.FirstMsg = true;
                io_unit_0.CanMon.SeqCnt = io_unit_0.StsMsg.SeqCnt;                                                      // Align sequence counters first instance
                vga.AddObjToBuffer(&ground_speed_4ds);
              } else if(io_unit_0.StsMsg.SeqCnt - io_unit_0.CanMon.SeqCnt > MAX_MISSING_MSGS){                          // Try to determine if messaged are going missing, add fault if too many
                  faultlog.AddFault(MASTER_IOUNIT_0_STATUS_MSGS_MISSING);
                }
              io_unit_0.CanMon.SeqCnt = io_unit_0.StsMsg.SeqCnt;                                                        // Align sequence counters
              digitalWrite(MASTER_LED_0,bitRead(io_unit_0.StsMsg.Outputs,0));                                           // Update LED to reflect io unit relay
              digitalWrite(MASTER_LED_1,bitRead(io_unit_0.StsMsg.Outputs,1));                                           // Update LED to reflect io unit relay
              break;
        
            case CAN_IO_BOARD_1_ID:
              memcpy(&io_unit_1.StsMsg,&CANTemp.Buffer,CAN_SIZE);                                                       // Copy Buffer into io unit 1 structure
              io_unit_1.CanMon.LastMsgMillis = millis();                                                                // Track message recieve time with CAN monitor
              if(!io_unit_1.CanMon.FirstMsg){                                                                           // Note first message recieved
                io_unit_1.CanMon.FirstMsg = true;
                io_unit_1.CanMon.SeqCnt = io_unit_1.StsMsg.SeqCnt;                                                      // Align sequence counters first instance
                vga.AddObjToBuffer(&bucket_speed_4ds);
                vga.AddObjToBuffer(&head_speed_4ds);
              } else if(io_unit_1.StsMsg.SeqCnt - io_unit_1.CanMon.SeqCnt > MAX_MISSING_MSGS){                          // Try to determine if messaged are going missing, add fault if too many
                  faultlog.AddFault(MASTER_IOUNIT_1_STATUS_MSGS_MISSING);
                }
              io_unit_1.CanMon.SeqCnt = io_unit_1.StsMsg.SeqCnt;                                                        // Align sequence counters
              digitalWrite(MASTER_LED_2,bitRead(io_unit_1.StsMsg.Outputs,0));                                           // Update LED to reflect io unit relay
              digitalWrite(MASTER_LED_3,bitRead(io_unit_1.StsMsg.Outputs,1));                                           // Update LED to reflect io unit relay
              break;    
        
            case CAN_IO_BOARD_2_ID:
              break;         
        
            case CAN_IO_BOARD_3_ID:
              break;              
          }
          break;  
        
        case IO_UNIT_ANALOG_01_MSG_ID:                                                                                   // Analog01 msg byte 0 is CAN Address
          switch (CANTemp.Buffer[0]){
            case CAN_IO_BOARD_0_ID:
              memcpy(&io_unit_0.Analog01,&CANTemp.Buffer,CAN_SIZE);                                                      // Copy Buffer into io unit 0 structure
              break;
        
            case CAN_IO_BOARD_1_ID:
              memcpy(&io_unit_1.Analog01,&CANTemp.Buffer,CAN_SIZE);                                                      // Copy Buffer into io unit 1 structure
              break;    
        
            case CAN_IO_BOARD_2_ID:
              break;         
        
            case CAN_IO_BOARD_3_ID:
              break;
          }  
          break;        
        
        case IO_UNIT_ANALOG_23_MSG_ID:
          switch (CANTemp.Buffer[0]){                                                                                    // Analog01 msg byte 0 is CAN Address
            case CAN_IO_BOARD_0_ID:
              memcpy(&io_unit_0.Analog23,&CANTemp.Buffer,CAN_SIZE);                                                      // Copy Buffer into io unit 0 structure
              break;
        
            case CAN_IO_BOARD_1_ID:
              memcpy(&io_unit_1.Analog23,&CANTemp.Buffer,CAN_SIZE);                                                      // Copy Buffer into io unit 1 structure
              break;    
        
            case CAN_IO_BOARD_2_ID:
              break;         
        
            case CAN_IO_BOARD_3_ID:
              break;
          }  
          break;  
        
        case IO_UNIT_COUNTER_A_MSG_ID:                                                                                   // Counter A msg byte 0 is CAN Address
          switch (CANTemp.Buffer[0]){
            case CAN_IO_BOARD_0_ID:
              memcpy(&io_unit_0.CntrA,&CANTemp.Buffer,CAN_SIZE);                                                         // Copy Buffer into io unit 0 structure
              ground_speed_4ds.Value = SpeedScale(io_unit_0.CntrA.Difference,io_unit_0.CntrA.Period,UR_UNIT_0_A_SPEED_SCALE);
              vga.AddObjToBuffer(&ground_speed_4ds);
              break;
        
            case CAN_IO_BOARD_1_ID:
              memcpy(&io_unit_1.CntrA,&CANTemp.Buffer,CAN_SIZE);                                                         // Copy Buffer into io unit 1 structure
              bucketspeed.UpdateFilter(((float)(io_unit_1.CntrA.Difference))*UR_UNIT_1_A_SPEED_SCALE);
              bucket_speed_4ds.Value = bucketspeed.OutputInt();
              vga.AddObjToBuffer(&bucket_speed_4ds);
              break;    
        
            case CAN_IO_BOARD_2_ID:
              break;         
        
            case CAN_IO_BOARD_3_ID:
              break;
          }  
          break;
        
        case IO_UNIT_COUNTER_B_MSG_ID:
          switch (CANTemp.Buffer[0]){                                                                                    // Counter B msg byte 0 is CAN Address
            case CAN_IO_BOARD_0_ID:
              memcpy(&io_unit_0.CntrB,&CANTemp.Buffer,CAN_SIZE);                                                         // Copy Buffer into io unit 0 structure
              break;
        
            case CAN_IO_BOARD_1_ID:
              memcpy(&io_unit_1.CntrB,&CANTemp.Buffer,CAN_SIZE);                                                         // Copy Buffer into io unit 1 structure
              headspeed.UpdateFilter(((float)(io_unit_1.CntrB.Difference))*UR_UNIT_1_B_SPEED_SCALE);
              head_speed_4ds.Value = headspeed.OutputInt();              
              vga.AddObjToBuffer(&head_speed_4ds);
              break;    
        
            case CAN_IO_BOARD_2_ID:
              break;         
        
            case CAN_IO_BOARD_3_ID:
              break;
          }  
          break;  
        
        case IO_UNIT_FAULTLOG_MSG_ID:
          switch (CANTemp.Buffer[0]){                                                                                    // IO unit Faultlog msg byte 0 is CAN Address
            case CAN_IO_BOARD_0_ID:
              memcpy(&io_unit_0.Log.Buffer,&CANTemp.Buffer[1],IOUNIT_FAULTLOG_SIZE);                                     // Add io unit faults to master faultlog, if they exist              
              for(int i=0;i<IOUNIT_FAULTLOG_SIZE;i++){
                if(io_unit_0.Log.Buffer[i] != NO_ERROR){
                  faultlog.AddFault(io_unit_0.Log.Buffer[i]);
                }
              }
              break;
        
            case CAN_IO_BOARD_1_ID:
              memcpy(&io_unit_1.Log.Buffer,&CANTemp.Buffer[1],IOUNIT_FAULTLOG_SIZE);                                     // Add io unit faults to master faultlog, if they exist
              for(int i=0;i<IOUNIT_FAULTLOG_SIZE;i++){
                if(io_unit_1.Log.Buffer[i] != NO_ERROR){
                  faultlog.AddFault(io_unit_1.Log.Buffer[i]);
                }                
              }
              break;    
        
            case CAN_IO_BOARD_2_ID:
              break;         
        
            case CAN_IO_BOARD_3_ID:
              break;
          }  
          break; 
       case IO_UNIT_DUAL_COUNTER_MSG_ID:
          switch (CANTemp.Buffer[0]){                                                                                    // Counter B msg byte 0 is CAN Address
            case CAN_IO_BOARD_0_ID:
              memcpy(&io_unit_0.DualCntr,&CANTemp.Buffer,CAN_SIZE);                                                      // Copy Buffer into io unit 0 structure
              ground_speed_4ds.Value = SpeedScale(io_unit_0.DualCntr.DiffA,io_unit_0.DualCntr.Period,UR_UNIT_0_A_SPEED_SCALE);
              vga.AddObjToBuffer(&ground_speed_4ds);                      
              break;
        
            case CAN_IO_BOARD_1_ID:
              memcpy(&io_unit_1.DualCntr,&CANTemp.Buffer,CAN_SIZE);                                                      // Copy Buffer into io unit 1 structure
              bucketspeed.UpdateFilter(((float)(io_unit_1.CntrA.Difference))*UR_UNIT_1_A_SPEED_SCALE);
              bucket_speed_4ds.Value = bucketspeed.OutputInt();
              vga.AddObjToBuffer(&bucket_speed_4ds);             
              headspeed.UpdateFilter(((float)(io_unit_1.CntrB.Difference))*UR_UNIT_1_B_SPEED_SCALE);
              head_speed_4ds.Value = headspeed.OutputInt();
              vga.AddObjToBuffer(&head_speed_4ds);  
              break;    
        
            case CAN_IO_BOARD_2_ID:
              break;         
        
            case CAN_IO_BOARD_3_ID:
              break;
          }  
          break;          
        default:
          break;  
      }
    }
  }
  
  unsigned long MillisNow = millis();                                                                                    // Retrieve current millis() value
  
  if(UseCanMonitoring){
    if(leveling_unit.CanMon.FirstMsg && (MillisNow - leveling_unit.CanMon.LastMsgMillis) > LEVELER_CAN_TIMEOUT_PERIOD){  // Check time period between leveler status messages
      faultlog.AddFault(MASTER_LEVELER_CAN_TIMEOUT);
      leveling_unit.CanMon.FirstMsg = false;
      relay0.SetOnFor(CAN_RELAY_RESET_PERIOD);                                                                          // Cycle relay if timeout period has been exceeded
    }
  
    if(io_unit_0.CanMon.FirstMsg && (MillisNow - io_unit_0.CanMon.LastMsgMillis) > IOUNIT_CAN_TIMEOUT_PERIOD){           // Check time period between io unit 0 status messages
      faultlog.AddFault(MASTER_IOUNIT_0_CAN_TIMEOUT);
      io_unit_0.CanMon.FirstMsg = false;
      relay1.SetOnFor(CAN_RELAY_RESET_PERIOD);                                                                          // Cycle relay if timeout period has been exceeded
    }  
  
    if(io_unit_1.CanMon.FirstMsg && (MillisNow - io_unit_1.CanMon.LastMsgMillis) > IOUNIT_CAN_TIMEOUT_PERIOD){           // Check time period between io unit 1 status messages
      faultlog.AddFault(MASTER_IOUNIT_1_CAN_TIMEOUT);
      io_unit_1.CanMon.FirstMsg = false;
      relay2.SetOnFor(CAN_RELAY_RESET_PERIOD);                                                                          // Cycle relay if timeout period has been exceeded
    }
  }  
  
  scheduler.Update();                                                                                                    // Update scheduler
  
  if(scheduler.TaskReady(MASTER_STATUS_MSG_TASK_NUM)){                                                                   // If it is time for a master status message
    if(!canbus.SendMsg(MASTER_STATUS_MSG_ID,0,CAN_SIZE,&master_unit.StsMsg)){                                            // Add message to send buffer, MCP2515 has 3 transmit buffers
      faultlog.AddFault(MASTER_CAN_TRANSMIT_BUFFER_FULL);                                                                // Fault if all buffers are full
    }
  }  
  
  if(MasterEditMsgInBuffer){                                                                                             // If there is a edit msg waiting to be sent 
    if(canbus.SendMsg(MASTER_EDIT_MSG_COMMAND_MSG_ID,0,CAN_SIZE,&master_unit.EditMsgCmd)){                               // Try to add to a send buffer
      MasterEditMsgInBuffer = false;                                                                                     // Reset if succesful
    } else {
        faultlog.AddFault(MASTER_CAN_TRANSMIT_BUFFER_FULL);                                                              // Fault if not succesful
      }
  } 
  
  for(int i=0;i<MASTER_RELAY_CMD_BUF_SIZE;i++){                                                                          // Scan the io command buffer
    if(relay_cmd_buf[i].Ready){                                                                                          // Check for a valid msg
      master_unit.IORelayCmd.CanId = relay_cmd_buf[i].CanId;                                                             // Transfer to command structure
      master_unit.IORelayCmd.RelayId = relay_cmd_buf[i].RelayId;
      master_unit.IORelayCmd.RelayState = relay_cmd_buf[i].RelayState;
      master_unit.IORelayCmd.Period = relay_cmd_buf[i].Period;
      if(canbus.SendMsg(MASTER_IO_RELAY_COMMAND_MSG_ID,0,CAN_SIZE,&master_unit.IORelayCmd)){                             // Try to add to a send buffer
        relay_cmd_buf[i].Ready = false;                                                                                  // Reset if succesful
      } else {
          faultlog.AddFault(MASTER_CAN_TRANSMIT_BUFFER_FULL);                                                            // Fault if not succesful
        }
      i = MASTER_RELAY_CMD_BUF_SIZE;                                                                                     // Only process one message at a time
    }
  } 
  
  if(MasterSetFilterMsgInBuffer){                                                                                        // If there is a set filter msg waiting to be sent
    if(canbus.SendMsg(MASTER_SET_ANALOG_FILTER_MSG_ID,0,CAN_SIZE,&master_unit.SetFilter)){                               // Try to add to a send buffer
      MasterSetFilterMsgInBuffer = false;                                                                                // Reset if succesful
    } else {
        faultlog.AddFault(MASTER_CAN_TRANSMIT_BUFFER_FULL);                                                              // Fault if not succesful
      }
  } 
}

void StartCAN(){
  master_unit.StsMsg.Mode = MASTER_RUNNING;                                                                              // Set master mode to running, serves no purpose yet
  master_unit.StsMsg.ActiveMsgs = 0x00;                                                                                  // Reset active messages
  leveling_unit.CanMon.FirstMsg = false;                                                                                 // Reset canbus monitor first message flags
  io_unit_0.CanMon.FirstMsg = false;
  io_unit_1.CanMon.FirstMsg = false;
  scheduler.EnableTask(MASTER_STATUS_MSG_TASK_NUM);                                                                      // Enable master status msg
  bitSet(master_unit.StsMsg.ActiveMsgs,MASTER_STATUS_MSG_TASK_NUM);                                                      // Set master status msg bit in active messages byte
  scheduler.Start();                                                                                                     // Start scheduler
}

unsigned char EditMsgCmd(unsigned char CanId,unsigned char MsgId,unsigned char StartStop,int Period){
  if(!MasterEditMsgInBuffer){
    MasterEditMsgInBuffer = true;
    master_unit.EditMsgCmd.CanId = CanId;
    master_unit.EditMsgCmd.MsgId = MsgId;
    master_unit.EditMsgCmd.StartStop = StartStop;
    master_unit.EditMsgCmd.Period = Period;
    return NO_ERROR;
  } else {
      return MASTER_EDIT_MSG_IN_BUFFER_ERROR;
    }
}

unsigned char IORelayCmd(unsigned char CanId,unsigned char RelayId,unsigned char RelayState,int Period){                 // Add an io relay command to buffer
  unsigned char RetVal = MASTER_IORELAY_MSG_IN_BUFFER_ERROR;
  for(int i=0;i<MASTER_RELAY_CMD_BUF_SIZE;i++){                                                                          // Scan for a free space            
    if(!relay_cmd_buf[i].Ready){                                                                                         // If space found
      relay_cmd_buf[i].Ready = true;
      relay_cmd_buf[i].CanId = CanId;
      relay_cmd_buf[i].RelayId = RelayId;
      relay_cmd_buf[i].RelayState = RelayState;
      relay_cmd_buf[i].Period = Period;  
      i = MASTER_RELAY_CMD_BUF_SIZE;                                                                                     // Exit Loop
      RetVal = NO_ERROR;                                                                                                 // Return no error
    }
  }
  return RetVal;                                                                                                         // Return buffer full fault 
}

unsigned char SetFilterCmd(unsigned char CanId,unsigned char Channel,unsigned char EnableDisable,float ConfigValue){     // Send a set filter command
  if(!MasterSetFilterMsgInBuffer){                                                                                       // Check that buffer is not occupied
    MasterSetFilterMsgInBuffer = true;
    master_unit.SetFilter.CanId = CanId;
    master_unit.SetFilter.Channel = Channel;
    master_unit.SetFilter.EnableDisable = EnableDisable;
    master_unit.SetFilter.ConfigValue = ConfigValue;
    return NO_ERROR;
  } else {
      return MASTER_SETFILTER_MSG_IN_BUFFER_ERROR;                                                                       // Return buffer full fault
    }
}  

unsigned int SpeedScale(unsigned int Diff,unsigned int Period,float SpeedScale){
  float TempFloat = (((float)(Diff))/(((float)(Period))/1000.0f))*SpeedScale;
  return (unsigned int)(TempFloat);
}

