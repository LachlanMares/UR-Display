
void InitLcds(){
  unsigned long MillisNow = millis();  
  TWBR = ((16000000L/400000L) - 16) / 2; 
  lcd.init();
  lcd1.init(); 
  lcd.backlight();
  lcd1.backlight();
  lcd.print("UR Master Unit");  
  lcd.setCursor(0,1);
  lcd.print("Version ");
  lcd.print(MASTER_VERSION_NUMBER);
  Lcd0UpdateMillis = MillisNow;
  Lcd1UpdateMillis = MillisNow + 500;    
}

void LcdUpdate(){
  unsigned long MillisNow = millis();
  
  if((MillisNow - Lcd0UpdateMillis) >= LCD_0_UPDATE_INTERVAL){
    Lcd0UpdateMillis = MillisNow;
    if(LcdDisplayScreen != LCD_DEFAULT_SCREEN){
      if((MillisNow - LcdDisplayMillis) >= LCD_0_DISPLAY_PERIOD){
        LcdDisplayScreen = LCD_DEFAULT_SCREEN;
      }    
    }
    
    lcd.clear();

    switch(LcdDisplayScreen){
      case LCD_DEFAULT_SCREEN:
        lcd.print("Time: ");
        if(leveling_unit.DateTime.Hour < 10){
          lcd.print("0");
        }
        lcd.print(leveling_unit.DateTime.Hour);
        lcd.print(":");
        if(leveling_unit.DateTime.Minute < 10){
          lcd.print("0");
        }
        lcd.print(leveling_unit.DateTime.Minute);
        lcd.print(":");
        if(leveling_unit.DateTime.Second < 10){
          lcd.print("0");
        }
        lcd.print(leveling_unit.DateTime.Second);

        lcd.setCursor(0,1);
        lcd.print("Speed: ");
        lcd.print(ground_speed_4ds.Value);
        lcd.print("m/s");
        
        lcd.setCursor(0,2);
        lcd.print("Conveyors: ");
        lcd.print(bucket_speed_4ds.Value);
        lcd.setCursor(0,3);
        lcd.print("Heads: ");
        lcd.print(head_speed_4ds.Value);    
        break;

      case LCD_UNIT_0_CNTR_SCREEN:
        lcd.print("Unit 0 Counters");
        lcd.setCursor(0,1);
        lcd.print("Speed A: ");
        lcd.print(io_unit_0.DualCntr.DiffA);
        lcd.setCursor(0,2);
        lcd.print("Speed B: ");
        lcd.print(io_unit_0.DualCntr.DiffB);  
        break;
      
      case LCD_UNIT_1_CNTR_SCREEN:
        lcd.print("Unit 1 Counters");
        
        lcd.setCursor(0,1);
        lcd.print("Speed A: ");
        lcd.print(io_unit_1.DualCntr.DiffA);
        
        lcd.setCursor(0,2);
        lcd.print("Speed B: ");
        lcd.print(io_unit_1.DualCntr.DiffB);
        break;

      case LCD_POWER_SUPPLY_SCREEN:
        lcd.print("Power Supply: ");

        lcd.setCursor(0,1);
        lcd.print("Current: "); 
        lcd.print(supply_current_4ds.Value);

        lcd.setCursor(0,2);
        lcd.print("Voltage: ");
        lcd.print(supply_voltage_4ds.Value);
        break;  
  
      case LCD_SEQ_CNTR_SCREEN:
        lcd.print("Leveller: ");
        lcd.print(leveling_unit.CanMon.SeqCnt);
        lcd.setCursor(0,1);
        lcd.print("IO Unit 0: "); 
        lcd.print(io_unit_0.CanMon.SeqCnt);
        lcd.setCursor(0,2);
        lcd.print("IO Unit 1: "); 
        lcd.print(io_unit_1.CanMon.SeqCnt);
        break;

      case LCD_POSITION_SCREEN:
        lcd.print("Time: ");
        if(leveling_unit.DateTime.Hour < 10){
          lcd.print("0");
        }
        lcd.print(leveling_unit.DateTime.Hour);
        lcd.print(":");
        if(leveling_unit.DateTime.Minute < 10){
          lcd.print("0");
        }
        lcd.print(leveling_unit.DateTime.Minute);
        lcd.print(":");
        if(leveling_unit.DateTime.Second < 10){
          lcd.print("0");
        }
        lcd.print(leveling_unit.DateTime.Second);
        
        lcd.setCursor(0,1);
        lcd.print("Lat:");
        lcd.setCursor(6,1);
        lcd.print(leveling_unit.GPS.Latitude,8);
        
        lcd.setCursor(0,2);
        lcd.print("Long:");
        lcd.setCursor(6,2);
        lcd.print(leveling_unit.GPS.Longitude,8);
        break;  
        
      case LCD_FAULT_SCREEN:
        lcd.print("Error Codes:");
        int j = 0;
        int k = 1;
        for(int i=0;i<MASTER_FAULTLOG_SIZE;i++){
          lcd.setCursor((i-j)*4,k);
          lcd.print(FaultBuffer[i],HEX);      
          if(i==4 || i==9){
            j=i+1;
            k++;
          } else if (i==14){
              i = MASTER_FAULTLOG_SIZE;
            }
        }
        break;    
    }
  }
 
  if((MillisNow - Lcd1UpdateMillis) >= LCD_1_UPDATE_INTERVAL){ 
    Lcd1UpdateMillis = MillisNow;
    lcd1.clear();
    switch(leveling_unit.StsMsg.LevelerMode){
      case LVLR_STATUS_INIT:
        lcd1.print("INIT ");
        break;
      case LVLR_STATUS_READY:
        lcd1.print("READY ");
        break;        
      case LVLR_STATUS_AUTO:
        lcd1.print("AUTO ");
        break;
      case LVLR_STATUS_MANUAL:
        lcd1.print("MANUAL ");
        break;  
      case LVLR_STATUS_FAULT:
        lcd1.print("FAULT ");
        break;  
    }
    
    if(bitRead(leveling_unit.StsMsg.IO,6)){
      lcd1.print("LEFT ");
    } else if(bitRead(leveling_unit.StsMsg.IO,7)){
        lcd1.print("RIGHT ");
      }
      
    lcd1.setCursor(14,0);
    switch(leveling_unit.StsMsg.LevelState){
      case LVLR_LEVEL:
        lcd1.print("LVL");
        break;        
      case LVLR_OUT_OF_TOLERANCE_POSITIVE:
        lcd1.print("POS");
        break;
      case LVLR_OUT_OF_TOLERANCE_NEGATIVE:
        lcd1.print("NEG");
        break; 
    } 
    
    lcd1.setCursor(0,1);
    switch(leveling_unit.StsMsg.CurrentError){
      case NO_ERROR:
        lcd1.print("NO ERROR");
        break;
      case LVLR_LEFT_TOO_LOW_ERROR:
        lcd1.print("LEFT TOO LOW ERROR");
        break;        
      case LVLR_RIGHT_TOO_LOW_ERROR:
        lcd1.print("RIGHT TOO LOW ERROR");
        break;
      case LVLR_LEFT_PROX_ERROR:
        lcd1.print("LEFT PROX ERROR");
        break;  
      case LVLR_RIGHT_PROX_ERROR:
        lcd1.print("RIGHT PROX ERROR");
        break;  
      case LVLR_LEFT_SENSOR_DIFF_ERROR:
        lcd1.print("LEFT DIFF ERROR");
        break;
      case LVLR_LEFT_TRACKS_OUT_OF_RANGE:
        lcd1.print("LEFT TRACKS OOR");
        break;
      case LVLR_RIGHT_SENSOR_DIFF_ERROR:
        lcd1.print("RIGHT DIFF ERROR");
        break;
      case LVLR_RIGHT_TRACKS_OUT_OF_RANGE:
        lcd1.print("RIGHT TRACKS OOR");
        break;
      case LVLR_CAN_TIMEOUT_ERROR:
        lcd1.print("CAN TIMEOUT");	                
        break;
      default:
        lcd1.print("UNKNOWN ERROR");
        break; 
    }    
   
    lcd1.setCursor(0,2);    
    lcd1.print("L:");
    lcd1.print(leveling_unit.Positions.LeftRamPosition);
    lcd1.print("mm");
    lcd1.setCursor(17,2);
    if(bitRead(leveling_unit.StsMsg.IO,4)){
      lcd1.print("ON");
    } else {
        lcd1.print("OFF");
      }
    
    lcd1.setCursor(0,3);  
    lcd1.print("R:");
    lcd1.print(leveling_unit.Positions.RightRamPosition);
    lcd1.print("mm");
    lcd1.setCursor(17,3);
    if(bitRead(leveling_unit.StsMsg.IO,5)){
      lcd1.print("ON");
    } else {
        lcd1.print("OFF");
      }      
  }
}


  

