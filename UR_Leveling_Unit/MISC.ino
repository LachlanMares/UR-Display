
void InitIO(){
  // Initialise Outputs
  pinMode(LeftUpRelay,OUTPUT);  
  pinMode(LeftDownRelay,OUTPUT);
  pinMode(RightUpRelay,OUTPUT);
  pinMode(RightDownRelay,OUTPUT);
  digitalWrite(LeftUpRelay,LOW);
  digitalWrite(LeftDownRelay,LOW);
  digitalWrite(RightUpRelay,LOW);
  digitalWrite(RightDownRelay,LOW);
  
  // Initialise Inputs
  pinMode(LeftProx1,INPUT); 
  pinMode(LeftProx2,INPUT);
  pinMode(RightProx1,INPUT);
  pinMode(RightProx2,INPUT);
  pinMode(AutoModeButton,INPUT_PULLUP);
  pinMode(LeftUpButton,INPUT_PULLUP);
  pinMode(LeftDownButton,INPUT_PULLUP);
  pinMode(RightUpButton,INPUT_PULLUP);
  pinMode(RightDownButton,INPUT_PULLUP);
}

void FastI2C(){                                                                // Function to set I2C speed to 400KHz
  TWBR = ((16000000L/400000L) - 16) / 2;
}
  
  

