
void ExtInt_0_ISR(){
  NewCANMsg = true;
}

void UpdateCAN(){
  if(NewCANMsg){
    NewCANMsg = false;   
    unsigned char NumOfMsgs = canbus.CheckReceiveBuffers();
    for(unsigned char i=0;i<NumOfMsgs;i++){
      canbus.ReadMsg(&CANTemp.CANid,&CANTemp.EXTid,&CANTemp.Len,&CANTemp.Buffer);
      
      switch(CANTemp.CANid){
        case MASTER_STATUS_MSG_ID:
          MasterStatusMsg();
          break;
        case MASTER_IO_RELAY_COMMAND_MSG_ID: 
          MasterRelayIOCmd(); 
          break;  
        case MASTER_EDIT_MSG_COMMAND_MSG_ID:
          MasterEditMsgCmd();
          break;       
        case MASTER_SET_ANALOG_FILTER_MSG_ID:
          MasterEnableFilterCmd();
          break;   
        default:          
          break;
      }  
    }
  }
    
  canbus_scheduler.Update();
  
  if(canbus_scheduler.TaskReady(IO_UNIT_STATUS_MSG_TASK_NUM)){
    io_unit.StsMsg.SeqCnt++;
    io_unit.StsMsg.Inputs = 0;
    io_unit.StsMsg.Outputs = 0;
    bitWrite(io_unit.StsMsg.Inputs,0,digitalRead(3));
    bitWrite(io_unit.StsMsg.Inputs,1,digitalRead(4));
    bitWrite(io_unit.StsMsg.Inputs,2,digitalRead(5));
    bitWrite(io_unit.StsMsg.Inputs,3,digitalRead(6));
    bitWrite(io_unit.StsMsg.Outputs,0,relay0.Status());
    bitWrite(io_unit.StsMsg.Outputs,1,relay1.Status());
    if(!canbus.SendMsg(IO_UNIT_STATUS_MSG_ID,0,CAN_SIZE,&io_unit.StsMsg)){
      faultlog.AddFault(IOUNIT_CAN_TRANSMIT_BUFFER_FULL);
    }
  }
  
  if(canbus_scheduler.TaskReady(IO_UNIT_ANALOG_01_MSG_TASK_NUM)){  
    io_unit.Analog01.Value0 = (unsigned int)(analog0.Output());
    io_unit.Analog01.Value1 = (unsigned int)(analog1.Output());
    if(!canbus.SendMsg(IO_UNIT_ANALOG_01_MSG_ID,0,CAN_SIZE,&io_unit.Analog01)){
      faultlog.AddFault(IOUNIT_CAN_TRANSMIT_BUFFER_FULL);
    }
  }  
  
  if(canbus_scheduler.TaskReady(IO_UNIT_ANALOG_23_MSG_TASK_NUM)){
    io_unit.Analog23.Value0 = (unsigned int)(analog2.Output());
    io_unit.Analog23.Value1 = (unsigned int)(analog3.Output());
    if(!canbus.SendMsg(IO_UNIT_ANALOG_23_MSG_ID,0,CAN_SIZE,&io_unit.Analog23)){
      faultlog.AddFault(IOUNIT_CAN_TRANSMIT_BUFFER_FULL);
    }
  }  
  
  if(canbus_scheduler.TaskReady(IO_UNIT_COUNTER_A_MSG_TASK_NUM)){
    io_unit.CntrA.SeqCnt++;
    mcpcounter.ReadCounter16Bit('A',&io_unit.CntrA.Count,&io_unit.CntrA.Difference,&io_unit.CntrA.Period);
    if(!canbus.SendMsg(IO_UNIT_COUNTER_A_MSG_ID,0,CAN_SIZE,&io_unit.CntrA)){
      faultlog.AddFault(IOUNIT_CAN_TRANSMIT_BUFFER_FULL);
    }
  }  
  
  if(canbus_scheduler.TaskReady(IO_UNIT_COUNTER_B_MSG_TASK_NUM)){
    io_unit.CntrB.SeqCnt++;
    mcpcounter.ReadCounter16Bit('B',&io_unit.CntrB.Count,&io_unit.CntrB.Difference,&io_unit.CntrB.Period);
    if(!canbus.SendMsg(IO_UNIT_COUNTER_B_MSG_ID,0,CAN_SIZE,&io_unit.CntrB)){
      faultlog.AddFault(IOUNIT_CAN_TRANSMIT_BUFFER_FULL);
    }
  }  
  
  if(canbus_scheduler.TaskReady(IO_UNIT_FAULTLOG_MSG_TASK_NUM)){
    if(faultlog.FaultActive()){
      faultlog.GetFaultLog(&io_unit.Log.Buffer);
      if(!canbus.SendMsg(IO_UNIT_FAULTLOG_MSG_ID,0,CAN_SIZE,&io_unit.Log)){
        faultlog.AddFault(IOUNIT_CAN_TRANSMIT_BUFFER_FULL);
      }
    }
  }  

  if(canbus_scheduler.TaskReady(IO_UNIT_DUAL_COUNTER_MSG_TASK_NUM)){
    unsigned int TempCount,TempPeriod; 
    io_unit.DualCntr.SeqCnt++;
    mcpcounter.ReadCounter16Bit('A',&TempCount,&io_unit.DualCntr.DiffA,&TempPeriod);
    mcpcounter.ReadCounter16Bit('B',&TempCount,&io_unit.DualCntr.DiffB,&io_unit.DualCntr.Period);
    if(!canbus.SendMsg(IO_UNIT_DUAL_COUNTER_MSG_ID,0,CAN_SIZE,&io_unit.DualCntr)){
      faultlog.AddFault(IOUNIT_CAN_TRANSMIT_BUFFER_FULL);
    }
  }    
}

void StartCAN(){
  canbus_scheduler.Start();
  canbus_scheduler.EnableTask(IO_UNIT_STATUS_MSG_TASK_NUM);                    // Set default startup messages
  bitSet(io_unit.StsMsg.ActiveMsgs,IO_UNIT_STATUS_MSG_TASK_NUM); 
  io_unit.StsMsg.CanId = CAN_NODE_ID;
  io_unit.StsMsg.SeqCnt = 0;
  io_unit.StsMsg.ActiveMsgs = 0;

  //canbus_scheduler.EnableTask(IO_UNIT_ANALOG_01_MSG_TASK_NUM);
  //bitSet(io_unit.StsMsg.ActiveMsgs,IO_UNIT_ANALOG_01_MSG_TASK_NUM);
  io_unit.Analog01.CanId = CAN_NODE_ID;
  io_unit.Analog01.Channels = 0x03;
  //analog0.SetLimits(0,0.0f,100.0f);
  //analog0.EnableLPFilter(IOUNIT_1_ANALOG_CUTOFFFREQ);
  //analog0.EnableMAFilter(IOUNIT_1_ANALOG_MA_FILTER_WINDOW);
  //analog1.SetLimits(0,0.0f,100.0f);
  //analog1.EnableLPFilter(IOUNIT_1_ANALOG_CUTOFFFREQ);
  //analog1.EnableMAFilter(IOUNIT_1_ANALOG_MA_FILTER_WINDOW);
  
  //canbus_scheduler.EnableTask(IO_UNIT_ANALOG_23_MSG_TASK_NUM);
  //bitSet(io_unit.StsMsg.ActiveMsgs,IO_UNIT_ANALOG_23_MSG_TASK_NUM); 
  io_unit.Analog23.CanId = CAN_NODE_ID;
  io_unit.Analog23.Channels = 0x0C;
  //analog2.SetLimits(0,0.0f,100.0f);
  //analog2.EnableLPFilter(IOUNIT_1_ANALOG_CUTOFFFREQ);
  //analog2.EnableMAFilter(IOUNIT_1_ANALOG_MA_FILTER_WINDOW);
  //analog3.SetLimits(0,0.0f,100.0f); 
  //analog3.EnableLPFilter(IOUNIT_1_ANALOG_CUTOFFFREQ);
  //analog3.EnableMAFilter(IOUNIT_1_ANALOG_MA_FILTER_WINDOW);  
   
  //canbus_scheduler.EnableTask(IO_UNIT_COUNTER_A_MSG_TASK_NUM);
  //bitSet(io_unit.StsMsg.ActiveMsgs,IO_UNIT_COUNTER_A_MSG_TASK_NUM);
  io_unit.CntrA.CanId = CAN_NODE_ID;
  io_unit.CntrA.SeqCnt = 0;
  
  //canbus_scheduler.EnableTask(IO_UNIT_COUNTER_B_MSG_TASK_NUM);
  //bitSet(io_unit.StsMsg.ActiveMsgs,IO_UNIT_COUNTER_B_MSG_TASK_NUM);  
  io_unit.CntrB.CanId = CAN_NODE_ID;
  io_unit.CntrB.SeqCnt = 0;
  
  canbus_scheduler.EnableTask(IO_UNIT_FAULTLOG_MSG_TASK_NUM);
  bitSet(io_unit.StsMsg.ActiveMsgs,IO_UNIT_FAULTLOG_MSG_TASK_NUM);
  io_unit.Log.CanId = CAN_NODE_ID;
  
  canbus_scheduler.EnableTask(IO_UNIT_DUAL_COUNTER_MSG_TASK_NUM);
  bitSet(io_unit.StsMsg.ActiveMsgs,IO_UNIT_DUAL_COUNTER_MSG_TASK_NUM); 
  io_unit.DualCntr.CanId = CAN_NODE_ID;
  io_unit.DualCntr.SeqCnt = 0;
}

void MasterStatusMsg(){
  memcpy(&master_status,&CANTemp.Buffer,CAN_SIZE);
  if(!io_unit.CanMon.FirstMsg){
    io_unit.CanMon.FirstMsg = true;
    io_unit.CanMon.SeqCnt = master_status.SeqCnt;
  } else if((master_status.SeqCnt - io_unit.CanMon.SeqCnt) > MAX_MISSING_MSGS){
    faultlog.AddFault(IOUNIT_MASTER_STATUS_MSG_TIMEOUT);
    }
  io_unit.CanMon.SeqCnt = master_status.SeqCnt;
}

void MasterRelayIOCmd(){
  MasterIORelayCommandStruct master_io_cmd;
  memcpy(&master_io_cmd,&CANTemp.Buffer,CAN_SIZE);
  if(master_io_cmd.CanId == CAN_NODE_ID){
    switch(master_io_cmd.RelayId){
      case 0:
        if(master_io_cmd.RelayState == 0){
          if(master_io_cmd.Period == 0){
            relay0.Off();
          } else {
              relay0.SetOffFor(master_io_cmd.Period);
            }
        } else {
            if(master_io_cmd.Period == 0){
              relay0.On();
            } else {
                relay0.SetOnFor(master_io_cmd.Period);
              }             
          }    
        break;
      case 1:
        if(master_io_cmd.RelayState == 0){
          if(master_io_cmd.Period == 0){
            relay1.Off();
          } else {
              relay1.SetOffFor(master_io_cmd.Period);
            }
        } else {
            if(master_io_cmd.Period == 0){
              relay1.On();
            } else {
                relay1.SetOnFor(master_io_cmd.Period);
              }             
          }
        break;
      default:
        faultlog.AddFault(IOUNIT_INVALID_RELAY_CMD); 
        break;
    }
  } 
}

void MasterEditMsgCmd(){
  MasterEditMsgCommandStruct edit_msg;
  memcpy(&edit_msg,&CANTemp.Buffer,CAN_SIZE);        
  if(edit_msg.CanId == CAN_NODE_ID){
    if(edit_msg.StartStop == 0){
      canbus_scheduler.DisableTask(edit_msg.MsgId);
      bitClear(io_unit.StsMsg.ActiveMsgs,edit_msg.MsgId);
    } else {
        switch(edit_msg.MsgId){
          case IO_UNIT_COUNTER_A_MSG_TASK_NUM:
            canbus_scheduler.EnableTask(IO_UNIT_COUNTER_A_MSG_TASK_NUM);
            bitSet(io_unit.StsMsg.ActiveMsgs,IO_UNIT_COUNTER_A_MSG_TASK_NUM);
            canbus_scheduler.DisableTask(IO_UNIT_DUAL_COUNTER_MSG_TASK_NUM);
            bitClear(io_unit.StsMsg.ActiveMsgs,IO_UNIT_DUAL_COUNTER_MSG_TASK_NUM);
            break;
          case IO_UNIT_COUNTER_B_MSG_TASK_NUM:
            canbus_scheduler.EnableTask(IO_UNIT_COUNTER_B_MSG_TASK_NUM);
            bitSet(io_unit.StsMsg.ActiveMsgs,IO_UNIT_COUNTER_B_MSG_TASK_NUM);
            canbus_scheduler.DisableTask(IO_UNIT_DUAL_COUNTER_MSG_TASK_NUM);
            bitClear(io_unit.StsMsg.ActiveMsgs,IO_UNIT_DUAL_COUNTER_MSG_TASK_NUM);
            break;
          case IO_UNIT_DUAL_COUNTER_MSG_TASK_NUM:
            canbus_scheduler.DisableTask(IO_UNIT_COUNTER_A_MSG_TASK_NUM);
            bitClear(io_unit.StsMsg.ActiveMsgs,IO_UNIT_COUNTER_A_MSG_TASK_NUM);
            canbus_scheduler.DisableTask(IO_UNIT_COUNTER_B_MSG_TASK_NUM);
            bitClear(io_unit.StsMsg.ActiveMsgs,IO_UNIT_COUNTER_B_MSG_TASK_NUM);
            canbus_scheduler.EnableTask(IO_UNIT_DUAL_COUNTER_MSG_TASK_NUM);
            bitSet(io_unit.StsMsg.ActiveMsgs,IO_UNIT_DUAL_COUNTER_MSG_TASK_NUM);
            break;
          default:
            canbus_scheduler.EnableTask(edit_msg.MsgId);
            bitSet(io_unit.StsMsg.ActiveMsgs,edit_msg.MsgId);
            break;
        } 
        if(edit_msg.Period >= MINIMUM_CAN_PERIOD){
          canbus_scheduler.EditTime(edit_msg.MsgId,edit_msg.Period);
        } else {
            canbus_scheduler.EditTime(edit_msg.MsgId,DEFAULT_CAN_PERIOD);
            faultlog.AddFault(IOUNIT_INVALID_EDITMSG_CMD); 
          }
      }
  }
}

void MasterEnableFilterCmd(){
  SetAnalogFilterStruct set_filt;
  memcpy(&set_filt,&CANTemp.Buffer,CAN_SIZE);   
  if(set_filt.CanId == CAN_NODE_ID){
    switch (set_filt.Channel){
      case 0x01:
        if(set_filt.EnableDisable == DISABLE_FILTER){
          analog0.DisableFilter();
        } else if (set_filt.EnableDisable == BUTTERWORTH_FILTER){
            analog0.EnableLPFilter(set_filt.ConfigValue);
          } else if (set_filt.EnableDisable == MOVING_AVG_FILTER){
              analog0.EnableMAFilter(set_filt.ConfigValue);
            } else {
                faultlog.AddFault(IOUNIT_INVALID_SETFILTER_CMD);
              }
        break;
      case 0x02:
        if(set_filt.EnableDisable == 0){
          analog1.DisableFilter();
        } else if (set_filt.EnableDisable == 1){
            analog1.EnableLPFilter(set_filt.ConfigValue);
          } else if (set_filt.EnableDisable == MOVING_AVG_FILTER){
              analog1.EnableMAFilter(set_filt.ConfigValue);
            } else {
                faultlog.AddFault(IOUNIT_INVALID_SETFILTER_CMD);
              }
        break;  
      case 0x04:
        if(set_filt.EnableDisable == 0){
          analog2.DisableFilter();
        } else if (set_filt.EnableDisable == 1){
            analog2.EnableLPFilter(set_filt.ConfigValue);
          } else if (set_filt.EnableDisable == MOVING_AVG_FILTER){
              analog2.EnableMAFilter(set_filt.ConfigValue);
            } else {
                faultlog.AddFault(IOUNIT_INVALID_SETFILTER_CMD);
              }
        break;         
      case 0x08:
        if(set_filt.EnableDisable == 0){
          analog3.DisableFilter();
        } else if (set_filt.EnableDisable == 1){
            analog3.EnableLPFilter(set_filt.ConfigValue);
          } else if (set_filt.EnableDisable == MOVING_AVG_FILTER){
              analog3.EnableMAFilter(set_filt.ConfigValue);
            } else {
                faultlog.AddFault(IOUNIT_INVALID_SETFILTER_CMD);
              }
        break;  
      default:
        faultlog.AddFault(IOUNIT_INVALID_SETFILTER_CMD); 
        break;      
    }
  }
}
