
void LcdInit(){
  pinMode(LCD_PIN,INPUT_PULLUP);
  LcdScanMillis = millis(); 
}

void LcdUpdate(){
  if((millis() - LcdScanMillis) >= LCD_SCAN_INTERVAL){
    LcdScanMillis = millis();
    if(!digitalRead(LCD_PIN)){
      if(!LcdConnected){
        LcdConnected = true;
        TWBR = ((16000000L/400000L) - 16) / 2;
        lcd.init();         
        lcd.backlight();
        lcd.print("UR Leveling Unit");     
        LcdUpdateMillis = LcdScanMillis;
      }
    } else {
        LcdConnected = false;
      }
  }
  
  if(((millis() - LcdUpdateMillis) >= LCD_UPDATE_INTERVAL) && LcdConnected){
    LcdUpdateMillis = millis();
    lcd.clear();
    switch(leveller.Mode()){
      case LVLR_STATUS_INIT:
        lcd.print("INIT ");
        break;
      case LVLR_STATUS_READY:
        lcd.print("READY ");
        break;        
      case LVLR_STATUS_AUTO:
        lcd.print("AUTO ");
        break;
      case LVLR_STATUS_MANUAL:
        lcd.print("MANUAL ");
        break;  
      case LVLR_STATUS_FAULT:
        lcd.print("FAULT ");
        break;  
    }
    
    if(leveller.Master()){
      lcd.print("LEFT ");
    } else {
        lcd.print("RIGHT ");
      }    

    lcd.setCursor(14,0);
    switch(leveller.LevelState()){
      case LVLR_LEVEL:
        lcd.print("LVL");
        break;        
      case LVLR_OUT_OF_TOLERANCE_POSITIVE:
        lcd.print("POS");
        break;
      case LVLR_OUT_OF_TOLERANCE_NEGATIVE:
        lcd.print("NEG");
        break; 
    }    
    
    lcd.setCursor(0,1);
    switch(leveller.CurrentError()){
      case NO_ERROR:
        lcd.print("NO ERROR");
        break;
      case LVLR_LEFT_TOO_LOW_ERROR:
        lcd.print("LEFT TOO LOW ERROR");
        break;        
      case LVLR_RIGHT_TOO_LOW_ERROR:
        lcd.print("RIGHT TOO LOW ERROR");
        break;
      case LVLR_LEFT_PROX_ERROR:
        lcd.print("LEFT PROX ERROR");
        break;  
      case LVLR_RIGHT_PROX_ERROR:
        lcd.print("RIGHT PROX ERROR");
        break;  
      case LVLR_LEFT_SENSOR_FAIL:
        lcd.print("LEFT SENSOR FAIL");
        break;
      case LVLR_RIGHT_SENSOR_FAIL:
        lcd.print("RIGHT SENSOR FAIL");
        break;
      default:
        lcd.print("UNKNOWN ERROR");
        break;
    }    
    
    lcd.setCursor(0,2);    
    lcd.print("L:");
    lcd.print(leveller.LeftRamPosition());
    lcd.print("mm ");
    lcd.print(analogRead(LEFT_RAM_CH0_PIN));  
    
    lcd.setCursor(0,3);  
    lcd.print("R:");
    lcd.print(leveller.RightRamPosition());
    lcd.print("mm ");
    lcd.print(analogRead(RIGHT_RAM_CH0_PIN));      
  }
}


