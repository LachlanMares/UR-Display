
void CheckLevel(){                                   // This function is called every second, it calculates which of the three level counters is the highest
  if (OutOfToleranceNegativeCounter > WithinToleranceCounter && OutOfToleranceNegativeCounter > OutOfTolerancePositiveCounter){
    OutOfTolerancePositive = false;                  // If negative counter has the highest value
    OutOfToleranceNegative = true;
  } else if (OutOfTolerancePositiveCounter > WithinToleranceCounter && OutOfTolerancePositiveCounter > OutOfToleranceNegativeCounter){
      OutOfTolerancePositive = true;                 // If positive counter has the highest value
      OutOfToleranceNegative = false;    
    } else {                                         // If within tolerance counter has the highest value
        OutOfTolerancePositive = false;              
        OutOfToleranceNegative = false;
    }
  WithinToleranceCounter = 0;                        // Reset Counters
  OutOfTolerancePositiveCounter = 0;
  OutOfToleranceNegativeCounter = 0;
}


