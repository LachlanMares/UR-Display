
void InitIO(){
  keypad_ur.Init();                                                                        // Intialise Keypad
  curr.SetChannel(ADC_CURRENT_PIN,500);
  curr.SetLimits(ADC_CURRENT_OFFSET,ADC_CURRENT_MIN_SCALE,ADC_CURRENT_MAX_SCALE);          // Set limits/scaling for measuring current
  vcc.SetChannel(ADC_VOLTAGE_PIN,500);
  vcc.SetLimits(ADC_VOLTAGE_OFFSET,ADC_VOLTAGE_MIN_SCALE,ADC_VOLTAGE_MAX_SCALE);           // Set limits/scaling for measuring voltage

  relay0.SetInitial(MASTER_RELAY_0,LOW);                                                  // Set initial state for relays
  relay1.SetInitial(MASTER_RELAY_1,LOW);
  relay2.SetInitial(MASTER_RELAY_2,LOW); 
  relay3.SetInitial(MASTER_RELAY_3,LOW);
  relay4.SetInitial(MASTER_RELAY_4,LOW);
  relay5.SetInitial(MASTER_RELAY_5,LOW); 
  relay6.SetInitial(MASTER_RELAY_6,LOW);
  relay7.SetInitial(MASTER_RELAY_7,LOW);
  
  io0rly0.Init(MASTER_SWITCH_0,INPUT,330);                                                 // Use OneTimeSwitch & map switch 0 to io unit 0 relay 0, set scan period mS
  io0rly1.Init(MASTER_SWITCH_1,INPUT,100);                                                 // Use OneTimeSwitch & map switch 1 to io unit 0 relay 1, set scan period mS
  io1rly0.Init(MASTER_SWITCH_2,INPUT,330);                                                 // Use OneTimeSwitch & map switch 2 to io unit 1 relay 0, set scan period mS     
  io1rly1.Init(MASTER_SWITCH_3,INPUT,100);                                                 // Use OneTimeSwitch & map switch 3 to io unit 1 relay 1, set scan period mS
  canmonen.Init(MASTER_SWITCH_4,INPUT,1000);                                               // Use OneTimeSwitch & map switch 4 to enable/disable can functions
  screenrstpb.Init(MASTER_SWITCH_5,INPUT,500);                                             // Use OneTimeSwitch & map switch 5 to reset screen if required
  
  pinMode(MASTER_LED_0,OUTPUT);                                                            // Set initial state for LEDs
  digitalWrite(MASTER_LED_0,LOW);
  pinMode(MASTER_LED_1,OUTPUT);
  digitalWrite(MASTER_LED_1,LOW);
  pinMode(MASTER_LED_2,OUTPUT);
  digitalWrite(MASTER_LED_2,LOW);
  pinMode(MASTER_LED_3,OUTPUT);
  digitalWrite(MASTER_LED_3,LOW);
  pinMode(MASTER_LED_4,OUTPUT);
  digitalWrite(MASTER_LED_4,LOW);
  pinMode(MASTER_LED_5,OUTPUT);
  digitalWrite(MASTER_LED_5,LOW);

  scheduler.EnableTask(SCHEDULER_1000MS_TASK_NUM); 
  
  bucketspeed.Init(0.0f);
  headspeed.Init(0.0f);
}

void UpdateIO(){
  unsigned long MillisNow = millis();                                                      // Read and store millis    
  
  keypad_ur.Update();                                                                      // Update keypad functions  
  switch(keypad_ur.Output()){                                                              // Check for a key press
    case KPD_ONE:
      LcdDisplayScreen = LCD_POSITION_SCREEN;
      LcdDisplayMillis = MillisNow;
      break;
    case KPD_TWO:
      LcdDisplayScreen = LCD_SEQ_CNTR_SCREEN;
      LcdDisplayMillis = MillisNow;
      break;
    case KPD_THREE:
      break;
    case KPD_FOUR:
      break;
    case KPD_FIVE:
      break;
    case KPD_SIX:
      break;
    case KPD_SEVEN:
      break;
    case KPD_EIGHT:
      break;
    case KPD_NINE:
      break;
    case KPD_ZERO:
      break;

    case KPD_START:
      break;
    case KPD_STOP:
      break;

    case KPD_F1:
      LcdDisplayScreen = LCD_UNIT_0_CNTR_SCREEN;                                           // Switch display screen   
      LcdDisplayMillis = MillisNow;                                                        // Start display timer
      break;
    case KPD_F2:
      LcdDisplayScreen = LCD_UNIT_1_CNTR_SCREEN;
      LcdDisplayMillis = MillisNow;
      break;
    case KPD_F3:
      LcdDisplayScreen = LCD_POWER_SUPPLY_SCREEN;
      LcdDisplayMillis = MillisNow;
      break;
    case KPD_F4:
      LcdDisplayScreen = LCD_FAULT_SCREEN;
      LcdDisplayMillis = MillisNow;
      break;

    case KPD_NOKEY:
      break;
  }
  
  curr.Update();                                                                           // Update Analog functions 
  if(curr.NewReading()){
    if(curr.Output() > ADC_CURRENT_LIMIT){                                                 // If current reading exceeds set limit
      if(!OverCurrent){                                                                    // If first occurence
        OverCurrentMillis = MillisNow;
        OverCurrent = true;
        faultlog.AddFault(MASTER_SUPPLY_OVERCURRENT);                                      // Add faultcode to faultlog
      }
      if((MillisNow - OverCurrentMillis) >= ADC_OVERCURRENT_PERIOD){                       // If fault condition has exceeded acceptable period
        DieHard();                                                                         // Disable external power connections
      }
    } else {                                                                                  
        OverCurrent = false;                                                               // Reset first reading flag
      }
  }
  
  vcc.Update();                                                                            // Update Analog functions
  if(vcc.NewReading()){
    if(vcc.Output() > ADC_VOLTAGE_HIGH_LIMIT || vcc.Output() < ADC_VOLTAGE_LOW_LIMIT){     // If voltage reading outside set limits
      if(!VoltageFault){                                                                   // If first occurence
        VoltageFaultMillis = MillisNow;
        VoltageFault = true;   
        faultlog.AddFault(MASTER_SUPPLY_VOLTAGE_FAULT);                                    // Add faultcode to faultlog   
      }
      if((MillisNow - VoltageFaultMillis) >= ADC_VOLTAGE_FAULT_PERIOD){                    // If fault condition has exceeded acceptable period  
        DieHard();                                                                         // Disable external power connections
      }
    } else {
        VoltageFault = false;                                                              // Reset first reading flag
      }
  }
  
  relay0.Update();                                                                         // Update DigitalOut functions
  relay1.Update();
  relay2.Update();
  relay3.Update();
  relay4.Update();
  relay5.Update();
  relay6.Update();
  relay7.Update();
    
  io0rly0.Update();                                                                        // Update OneTimeSwitch functions 
  switch(io0rly0.CheckForPulse()){                                                         // CheckForPulse is used for rising and falling transitions
    case OTS_RISING_PULSE:
      IORelayCmd(CAN_IO_BOARD_0_ID,IOUNIT_RELAY_0,IOUNIT_RELAY_ON,IOUNIT_NO_DELAY);        // Send can msg to io unit 0, turn on relay 0, stay on
      break;
    case OTS_FALLING_PULSE:
      IORelayCmd(CAN_IO_BOARD_0_ID,IOUNIT_RELAY_0,IOUNIT_RELAY_OFF,IOUNIT_NO_DELAY);       // Send can msg to io unit 0, turn off relay 0, stay off
      break;
  }
  
  io0rly1.Update();                                                                        // Update OneTimeSwitch functions
  if(io0rly1.RisingPulse()){                                                               // RisingPulse is used for rising transitions
    IORelayCmd(CAN_IO_BOARD_0_ID,IOUNIT_RELAY_1,IOUNIT_RELAY_ON,5000);                     // RisingPulse send can msg to io unit 0, turn on relay 1, stay on for X mS
  }  
  
  io1rly0.Update();                                                                        // Update OneTimeSwitch functions
  switch(io1rly0.CheckForPulse()){                                                         // CheckForPulse is used for rising and falling transitions
    case OTS_RISING_PULSE:
      IORelayCmd(CAN_IO_BOARD_1_ID,IOUNIT_RELAY_0,IOUNIT_RELAY_ON,IOUNIT_NO_DELAY);        // Send can msg to io unit 1, turn on relay 0, stay on
      break;
    case OTS_FALLING_PULSE:
      IORelayCmd(CAN_IO_BOARD_1_ID,IOUNIT_RELAY_0,IOUNIT_RELAY_OFF,IOUNIT_NO_DELAY);       // Send can msg to io unit 1, turn off relay 0, stay off
      break;
  }
  
  io1rly1.Update();                                                                        // Update OneTimeSwitch functions
  if(io1rly1.RisingPulse()){                                                               // RisingPulse is used for rising transitions
    IORelayCmd(CAN_IO_BOARD_1_ID,IOUNIT_RELAY_1,IOUNIT_RELAY_ON,5000);                     // Send can msg to io unit 1, turn on relay 1, stay on for X mS
  }
  
  screenrstpb.Update();                                                                    // Update OneTimeSwitch functions
  if(screenrstpb.RisingPulse()){                                                           // RisingPulse is used for rising transitions
    vga.ResetVGA();
  }  
  
  canmonen.Update();
  switch(canmonen.CheckForPulse()){                                                        // CheckForPulse is used for rising and falling transitions
    case OTS_RISING_PULSE:
      UseCanMonitoring = true;                                                             // Enable can monitoring
      break;
    case OTS_FALLING_PULSE:
      UseCanMonitoring = false;                                                            // Disable can monitoring
      break;
  }
  
  if(scheduler.TaskReady(SCHEDULER_1000MS_TASK_NUM)){                                      // If general purpose timer is ready
    supply_voltage_4ds.Value = (unsigned int)(vcc.Output());                               // Convert from float to uint
    supply_current_4ds.Value = (unsigned int)(curr.Output());                              // Convert from float to uint 
    vga.AddObjToBuffer(&supply_voltage_4ds);                                               // Update voltage reading on 4DS
    vga.AddObjToBuffer(&supply_current_4ds);                                               // Update current reading on 4DS 
  }
}

void DieHard(){                                                                            // Turn off all relays             
  relay0.On();
  relay1.On();
  relay2.On(); 
  relay3.On();
  relay4.On();
  relay5.On(); 
  relay6.On();
  relay7.On();
}
