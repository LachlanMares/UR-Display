void ExtInt_1_ISR(){
  NewCANMsg = true;
}

void CANUpdate(){
  if(NewCANMsg){
    NewCANMsg = false;   
    unsigned char NumOfMsgs = canbus.CheckReceiveBuffers();
    for(unsigned char i=0;i<NumOfMsgs;i++){
      canbus.ReadMsg(&CANTemp.CANid,&CANTemp.EXTid,&CANTemp.Len,&CANTemp.Buffer);      
      switch(CANTemp.CANid){
        case MASTER_STATUS_MSG_ID:
          MasterStatusMsg();
          break;
        case MASTER_EDIT_MSG_COMMAND_MSG_ID:
          MasterEditMsgCmd();
          break;        
        default:          
          break;
      }  
    }
  }
  
  canbus_scheduler.Update();
  
  if(canbus_scheduler.TaskReady(LVLR_STATUS_MSG_TASK_NUM)){
    leveller.Leveller_Msg(&CANTemp.Buffer,LevellerActiveMsgs);
    if(!canbus.SendMsg(LVLR_STATUS_MSG_ID,0,CAN_SIZE,&CANTemp.Buffer)){
      faultlog.AddFault(LVLR_CAN_TRANSMIT_BUFFER_FULL);
    }
  }
  
  if(canbus_scheduler.TaskReady(LVLR_RAM_POSITIONS_MSG_TASK_NUM)){
    leveller.RamPos_Msg(&CANTemp.Buffer);
    if(!canbus.SendMsg(LVLR_RAM_POSITIONS_MSG_ID,0,CAN_SIZE,&CANTemp.Buffer)){
      faultlog.AddFault(LVLR_CAN_TRANSMIT_BUFFER_FULL);
    }
  }  
  
  if(canbus_scheduler.TaskReady(LVLR_ROLL_PITCH_MSG_TASK_NUM)){
    if(!canbus.SendMsg(LVLR_ROLL_PITCH_MSG_ID,0,CAN_SIZE,&CANTemp.Buffer)){
      faultlog.AddFault(LVLR_CAN_TRANSMIT_BUFFER_FULL);
    }
  }
  
  if(canbus_scheduler.TaskReady(LVLR_FAULTLOG_MSG_TASK_NUM)){
    if(faultlog.FaultActive()){
      faultlog.GetFaultLog(&CANTemp.Buffer);
      if(!canbus.SendMsg(LVLR_FAULTLOG_MSG_ID,0,CAN_SIZE,&CANTemp.Buffer)){
        faultlog.AddFault(LVLR_CAN_TRANSMIT_BUFFER_FULL);
      }
    }
  }  
  
  if(canbus_scheduler.TaskReady(LVLR_POSITION_MSG_TASK_NUM)){
    if(!canbus.SendMsg(LVLR_POSITION_MSG_ID,0,CAN_SIZE,&gps_position)){
      faultlog.AddFault(LVLR_CAN_TRANSMIT_BUFFER_FULL);
    }
  }   
  
  if(canbus_scheduler.TaskReady(LVLR_DATETIME_MSG_TASK_NUM)){
    if(!canbus.SendMsg(LVLR_DATETIME_MSG_ID,0,CAN_SIZE,&datetime)){
      faultlog.AddFault(LVLR_CAN_TRANSMIT_BUFFER_FULL);
    }      
  }   
}

void StartCAN(){
  attachInterrupt(1,ExtInt_1_ISR,FALLING);
  LevellerActiveMsgs = 0;
  canbus_scheduler.Start();
                                                                             
  canbus_scheduler.EnableTask(LVLR_STATUS_MSG_TASK_NUM);                     // Set default startup messages
  bitSet(LevellerActiveMsgs,LVLR_STATUS_MSG_TASK_NUM); 

  canbus_scheduler.EnableTask(LVLR_RAM_POSITIONS_MSG_TASK_NUM);
  bitSet(LevellerActiveMsgs,LVLR_RAM_POSITIONS_MSG_TASK_NUM);

  //canbus_scheduler.EnableTask(LVLR_ROLL_PITCH_MSG_TASK_NUM);
  //bitSet(LevellerActiveMsgs,LVLR_ROLL_PITCH_MSG_TASK_NUM);  

  canbus_scheduler.EnableTask(LVLR_FAULTLOG_MSG_TASK_NUM);
  bitSet(LevellerActiveMsgs,LVLR_FAULTLOG_MSG_TASK_NUM); 

  canbus_scheduler.EnableTask(LVLR_POSITION_MSG_TASK_NUM);
  bitSet(LevellerActiveMsgs,LVLR_POSITION_MSG_TASK_NUM);

  canbus_scheduler.EnableTask(LVLR_DATETIME_MSG_TASK_NUM);
  bitSet(LevellerActiveMsgs,LVLR_DATETIME_MSG_TASK_NUM);
}

void MasterStatusMsg(){
  memcpy(&master_status,&CANTemp.Buffer,CAN_SIZE);
  if(!FirstMasterStatus){
    FirstMasterStatus = true;
    LastMasterSeqNum = master_status.SeqCnt;
  }
  if(master_status.SeqCnt - LastMasterSeqNum >= MAX_MISSING_MSGS){
    faultlog.AddFault(LVLR_CAN_TIMEOUT_ERROR);
  }
  LastMasterSeqNum = master_status.SeqCnt;
}

void MasterEditMsgCmd(){
  MasterEditMsgCommandStruct edit_msg;
  memcpy(&edit_msg,&CANTemp.Buffer,CAN_SIZE);        
  if(edit_msg.CanId == LEVELER_CAN_ID){
    if(edit_msg.StartStop == 0){
      canbus_scheduler.DisableTask(edit_msg.MsgId);
      bitClear(LevellerActiveMsgs,edit_msg.MsgId);
    } else {
        canbus_scheduler.EnableTask(edit_msg.MsgId);
        bitSet(LevellerActiveMsgs,edit_msg.MsgId);
        if(edit_msg.Period >= MINIMUM_CAN_PERIOD){
          canbus_scheduler.EditTime(edit_msg.MsgId,edit_msg.Period);
        } else {
            canbus_scheduler.EditTime(edit_msg.MsgId,DEFAULT_CAN_PERIOD);
            faultlog.AddFault(LVLR_INVALID_EDITMSG_CMD);
          }
      }
  }
}
  
