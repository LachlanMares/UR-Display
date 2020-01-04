void GPSUpdate(){
  if (Serial2.available()){                                       
    char c = Serial2.read();                                                      
    if (gps.encode(c)){                                           
      gps.f_get_position(&gps_position.Latitude,&gps_position.Longitude,&Gps_Age);                                                                         
      gps.crack_datetime(&datetime.Year,&datetime.Month,&datetime.Day,&datetime.Hour,&datetime.Minute,&datetime.Second,&datetime.Hundredths,&Gps_Age);
      datetime.Minute += GMT_MINUTE;                                                                                                             
      if (datetime.Minute > 59){                                                     
        datetime.Minute -= 60;
        datetime.Hour += 1;  
      }
      datetime.Hour += GMT_HOUR;
      if (datetime.Hour > 23){
        datetime.Hour -= 24;
        datetime.Day += 1;
      }
    }
  }
}

