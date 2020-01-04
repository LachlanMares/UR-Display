  
void GPSStatus(){                                                 // Check GPS Status
  if (Serial2.available()){                                       // Check if there is data in the buffer
    char c = Serial2.read();                                                      
    if (gps.encode(c)){                                           // Call GPSTiny library, if function returns true it means that a new string has been recieved
                                                                  // Some task maybe required later?
    }
  }
}

