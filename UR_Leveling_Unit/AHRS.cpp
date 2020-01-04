#include "AHRS.h"

AHRS::AHRS(){
  _GetNewSample = false;
  _Enable = false;
  _GyroReady = false;
}

void AHRS::Init(byte Sample_Rate,byte Accl_Config,byte Gyro_Config,int GyroSamples,float CutoffFreq){
  
  ResetQuaternion(&_Q); 
  ResetVector(&_Accl);
  ResetVector(&_Gyro);
  ResetVector(&_Mag); 
  GetCalibrationMatrix(&_AcclC,ACCL_EE_ADDR);
  GetCalibrationMatrix(&_MagC,MAG_EE_ADDR);
  
  switch(Sample_Rate){
    case MPU_SAMPLE_200HZ:
      _DT = 0.005f;
      break;
    case MPU_SAMPLE_166HZ:
      _DT = 0.00602f;
      break;   
    case MPU_SAMPLE_142HZ:
      _DT = 0.00704f;
      break;   
    case MPU_SAMPLE_125HZ:
      _DT = 0.008f;
      break;
    case MPU_SAMPLE_100HZ:
      _DT = 0.01f;
      break;
    case MPU_SAMPLE_50HZ:
      _DT = 0.02f;
      break;
  } 
  
  _GyroDriftDt = _DT * GYRO_DRIFT;
  _HalfDT = 0.5f*_DT;
  _Sigma = SIGMA;
  
  switch(Accl_Config){
    case BITS_ACC_2G:
      SetVectorTo(&_AcclScale,32768.0f/2.0f);
      break;
    case BITS_ACC_4G:
      SetVectorTo(&_AcclScale,32768.0f/4.0f);
      break;   
    case BITS_ACC_8G:
      SetVectorTo(&_AcclScale,32768.0f/8.0f);
      break;   
    case BITS_ACC_16G:
      SetVectorTo(&_AcclScale,32768.0f/16.0f);
      break;
    default:
      SetVectorTo(&_AcclScale,ACC_SCALE_FACTOR);
      break;
  }
  
  switch(Gyro_Config){
    case BITS_FS_250DPS:
      SetVectorTo(&_GyroScale,32768.0f/250.0f);
      break;
    case BITS_FS_500DPS:
      SetVectorTo(&_GyroScale,32768.0f/500.0f);
      break;
    case BITS_FS_1000DPS:
      SetVectorTo(&_GyroScale,32768.0f/1000.0f);
      break;
    case BITS_FS_2000DPS:
      SetVectorTo(&_GyroScale,32768.0f/2500.0f);
      break;
    default: 
      SetVectorTo(&_GyroScale,GYRO_SCALE_FACTOR);
      break;
  }

  ResetVector(&_GyroZeroBias);
  ResetVector(&_GyroBiasDt); 
    
  WriteI2C(MPU6050_ADDR,MPUREG_PWR_MGMT_1,BIT_H_RESET);    
  delay(50);  
  WriteI2C(MPU6050_ADDR,MPUREG_PWR_MGMT_1,MPU_CLK_SEL_PLLGYROZ); 
  WriteI2C(MPU6050_ADDR,MPUREG_SMPLRT_DIV,Sample_Rate);
  WriteI2C(MPU6050_ADDR,MPUREG_CONFIG,BITS_DLPF_CFG_42HZ);
  WriteI2C(MPU6050_ADDR,MPUREG_GYRO_CONFIG,Gyro_Config);
  WriteI2C(MPU6050_ADDR,MPUREG_ACCEL_CONFIG,Accl_Config);
  WriteI2C(MPU6050_ADDR,MPUREG_INT_ENABLE,BIT_RAW_RDY_EN);
  WriteI2C(MPU6050_ADDR,MPUREG_INT_PIN_CFG,BIT_INT_ANYRD_2CLEAR);  

  float _Wc = TWO_M_PI*CutoffFreq;
  float _c = 1.0f/(tan((_Wc*_DT)/2.0f));
  float _G = _c*_c + SQRT2*_c + 1.0f;
  _Yc0 = (2.0f - 2.0f*_c*_c)/_G;
  _Yc1 = (_c*_c - SQRT2*_c + 1.0f)/_G;
  _Xc0 = 1.0f/_G;
  _Xc1 = 2.0f/_G;
  _Xc2 = 1.0f/_G;
  
  _SampleCounter = 0;
  _ReqSamples = GyroSamples;
  
  if(_ReqSamples > 0){
    _InitMode = true;
  } else {
      _InitMode = false;
      _GyroReady = true;
    }

  WriteI2C(AK8975C_ADDR,AK8975C_CNTL,SINGLE_READING);
  _GetNewSample = true;   
}

boolean AHRS::Status(){
  return _GyroReady;
}

void AHRS::Start(){
  _Enable = true;
}

void AHRS::Stop(){
  _Enable = false;
}

void AHRS::AHRS_ISR(){
  if(_Enable || _InitMode){
    _GetNewSample = true;
  }
}

void AHRS::Update(){
  if(_GetNewSample){
    int _i = 0;
    int _i2cdata[I2C_LENGTH];
  
    ReadI2C(MPU6050_ADDR,MPUREG_ACCEL_XOUT_H,MPU6050_LENGTH); 
    while(Wire.available()){ 
      _i2cdata[_i] = Wire.read();                                  
      _i++;
    }
    Wire.endTransmission();                                         
  
    if (_i==MPU6050_LENGTH){ 
      if(!_InitMode){  
        _Accl.x = (((float)((_i2cdata[2] << 8) | _i2cdata[3])) / _AcclScale.x) * -1.0f;                                                      
        _Accl.y = (((float)((_i2cdata[0] << 8) | _i2cdata[1])) / _AcclScale.y);      
        _Accl.z = (((float)((_i2cdata[4] << 8) | _i2cdata[5])) / _AcclScale.z);  
        _Gyro.x = TO_RAD((((float)((_i2cdata[8] << 8)  | _i2cdata[9]))  / _GyroScale.x) * -1.0f) - _GyroZeroBias.x - _GyroBiasDt.x;           
        _Gyro.y = TO_RAD(((float)((_i2cdata[10] << 8) | _i2cdata[11])) / _GyroScale.y) - _GyroZeroBias.y - _GyroBiasDt.y;            
        _Gyro.z = TO_RAD(((float)((_i2cdata[12] << 8) | _i2cdata[13])) / _GyroScale.z) - _GyroZeroBias.z - _GyroBiasDt.z;  
        NormaliseVector(&_Accl);
      } else {
          _GyroZeroBias.x += TO_RAD(((float)((_i2cdata[8] << 8)  | _i2cdata[9]))  / _GyroScale.x) * -1.0f;            
          _GyroZeroBias.y += TO_RAD(((float)((_i2cdata[10] << 8) | _i2cdata[11])) / _GyroScale.y);            
          _GyroZeroBias.z += TO_RAD(((float)((_i2cdata[12] << 8) | _i2cdata[13])) / _GyroScale.z);  
          _SampleCounter++;   
          if (_SampleCounter == _ReqSamples){
            _InitMode = false;
            _GyroReady = true;
            _GyroZeroBias.x /= _ReqSamples;            
            _GyroZeroBias.y /= _ReqSamples;            
            _GyroZeroBias.z /= _ReqSamples; 
          }
        }     
    } else {
        Serial.println("!ERR: MPU data");                                                                                                        
      } 

    ReadI2C(AK8975C_ADDR,AK8975C_HXL,AK8975C_LENGTH);
    while(Wire.available()){ 
      _i2cdata[_i] = Wire.read();                                  
      _i++;
    }
    Wire.endTransmission();  
  
    Vector _TempMag;
    if (_i==AK8975C_LENGTH){                                                                                                                                                  
      _TempMag.x = ((float)((_i2cdata[3] << 8) | _i2cdata[2])) * -1.0f;                           
      _TempMag.y = (float)((_i2cdata[1] << 8) | _i2cdata[0]);                           
      _TempMag.z = (float)((_i2cdata[5] << 8) | _i2cdata[4]);                           
      _Mag.x = _MagC.Xscale*_TempMag.x + _MagC.Xbias;
      _Mag.y = _MagC.Yscale*_TempMag.y + _MagC.Ybias;
      _Mag.z = _MagC.Zscale*_TempMag.z + _MagC.Zbias;
      NormaliseVector(&_Mag);
    } else {
        Serial.println("!ERR: Mag data");                                                          
      }
    WriteI2C(AK8975C_ADDR,AK8975C_CNTL,SINGLE_READING); 
    
    if (CheckInputValid(&_Accl) && CheckInputValid(&_Gyro)){ 
      Quaternion _TwoQdot,_Qdot,_Qhat;
    
      QFromAccl(_Accl,&_Qdot);
      ScaledCopyQ(&_Qdot,2.0f,&_TwoQdot);

      _GyroBiasDt.x -= _GyroDriftDt*(_Q.w*_TwoQdot.x - _Q.x*_TwoQdot.w - _Q.y*_TwoQdot.z + _Q.z*_TwoQdot.y);  // Estimate gyro drift direction by looking at current and previous orientation estimates
      _GyroBiasDt.y -= _GyroDriftDt*(_Q.w*_TwoQdot.y + _Q.x*_TwoQdot.z - _Q.y*_TwoQdot.w - _Q.z*_TwoQdot.x);
      _GyroBiasDt.z -= _GyroDriftDt*(_Q.w*_TwoQdot.z - _Q.x*_TwoQdot.y + _Q.y*_TwoQdot.x - _Q.z*_TwoQdot.w);
            
      ScaledCopyQ(&_Q,1.0f,&_Qhat);

      _Qhat.w +=  -1.0f*_HalfDT*(_Gyro.x*_Qhat.x + _Gyro.y*_Qhat.y + _Gyro.z*_Qhat.z);                           // Use rotation matrix to estimate change in orientation using gyro readings
      _Qhat.x +=  _HalfDT*(_Gyro.x*_Qhat.w + _Gyro.z*_Qhat.y - _Gyro.y*_Qhat.z);
      _Qhat.y +=  _HalfDT*(_Gyro.y*_Qhat.w - _Gyro.z*_Qhat.x + _Gyro.x*_Qhat.z);
      _Qhat.z +=  _HalfDT*(_Gyro.z*_Qhat.w + _Gyro.y*_Qhat.x - _Gyro.x*_Qhat.y);
    
      NormaliseQuaternion(&_Qhat);                                                                         // Normalise Qhat

      _Q.w = ScaleTwoFloats(_Qhat.w,_Qdot.w,_Sigma);                                                      // Scale the two orientation estimation based on sigma value
      _Q.x = ScaleTwoFloats(_Qhat.x,_Qdot.x,_Sigma);
      _Q.y = ScaleTwoFloats(_Qhat.y,_Qdot.y,_Sigma);
      _Q.z = ScaleTwoFloats(_Qhat.z,_Qdot.z,_Sigma);
    
      NormaliseQuaternion(&_Q);                                                                             // Normalise Q 
      
      if(isnan(_Q.w) == 1 || isnan(_Q.x) == 1 || isnan(_Q.y) == 1 || isnan(_Q.z) == 1){
        ResetQuaternion(&_Q);
      }
      _EulerAngles.phi = UpdateFilter(&_Phi,SetPhiFromQuaternion(&_Q));
      _EulerAngles.theta = UpdateFilter(&_Theta,SetThetaFromQuaternion(&_Q));
    }

    if(CheckInputValid(&_Mag) && abs(_EulerAngles.phi) < MAX_MAG_ANGLE && abs(_EulerAngles.theta) < MAX_MAG_ANGLE){
      float _TempPsi = atan2(-_Mag.y,_Mag.x) + MAGNETIC_DECLINATION;
      while (_TempPsi > TWO_M_PI){                                                                      // Want to see a heading of 0 to 359 degrees
        _TempPsi -= TWO_M_PI;
      } 
      while (_TempPsi < 0.0f){                                                                          // Want to see a heading of 0 to 359 degrees
        _TempPsi += TWO_M_PI;
      } 
      _EulerAngles.psi = UpdateFilter(&_Psi,_TempPsi);
    }
  }
}

void AHRS::GetEulerAngles(Euler* EAng){
  EAng->phi = _EulerAngles.phi;
  EAng->theta = _EulerAngles.theta;
  EAng->psi = _EulerAngles.psi;
}

void AHRS::ResetQuaternion(Quaternion* Qt){
  Qt->w = 1.0f;
  Qt->x = 0.0f;
  Qt->y = 0.0f;  
  Qt->z = 0.0f;  
}  

void AHRS::NormaliseQuaternion(Quaternion* Qt){
  float _Norm = sqrt(Qt->w*Qt->w + Qt->x*Qt->x + Qt->y*Qt->y + Qt->z*Qt->z); 
  Qt->w /= _Norm;
  Qt->x /= _Norm;
  Qt->y /= _Norm;
  Qt->z /= _Norm;
}

void AHRS::ScaledCopyQ(Quaternion* Qt,float St,Quaternion* Qt2){
  Qt2->w = Qt->w * St;
  Qt2->x = Qt->x * St;
  Qt2->y = Qt->y * St;
  Qt2->z = Qt->z * St;
} 

void AHRS::QFromAccl(Vector At,Quaternion* Qr){
  Vector _N;
  _N.x = At.x;                                                                                     
  _N.y = At.y;                                                                                     
  _N.z = At.z + 1.0f;
  NormaliseVector(&_N);
  Qr->w = (At.x*_N.x + At.y*_N.y + At.z*_N.z);                                                           
  Qr->x = (At.y*_N.z) - (At.z*_N.y);                                                                   
  Qr->y = (At.z*_N.x) - (At.x*_N.z);
  Qr->z = (At.x*_N.y) - (At.y*_N.x);
  NormaliseQuaternion(Qr); 
}

float AHRS::SetPhiFromQuaternion(Quaternion* Qt){
  return (atan2(2.0f*Qt->w*Qt->x - 2.0f*Qt->y*Qt->z , 1.0f - 2.0f*Qt->x*Qt->x - 2.0f*Qt->y*Qt->y));
}

float AHRS::SetThetaFromQuaternion(Quaternion* Qt){
  return (asin(2.0f*Qt->w*Qt->y + 2.0f*Qt->z*Qt->x));
}
 
float AHRS::SetPsiFromQuaternion(Quaternion* Qt){
 return (atan2(2.0f*Qt->w*Qt->z - 2.0f*Qt->x*Qt->y , 1.0f - 2.0f*Qt->y*Qt->y - 2.0f*Qt->z*Qt->z));
} 

void AHRS::ResetVector(Vector* Vt){
  Vt->x = 0.0f;
  Vt->y = 0.0f;  
  Vt->z = 0.0f;  
}  

void AHRS::NormaliseVector(Vector* Vt){
  float _Norm = sqrt(Vt->x*Vt->x + Vt->y*Vt->y + Vt->z*Vt->z); 
  Vt->x /= _Norm;
  Vt->y /= _Norm;
  Vt->z /= _Norm;
}

void AHRS::SetVectorTo(Vector* Vt,float Ft){
  Vt->x = Ft;
  Vt->y = Ft;
  Vt->z = Ft;
}

boolean AHRS::CheckInputValid(Vector* Vt){
  boolean RetVal = true;
  if(abs(Vt->x) < VALID_LIMIT && abs(Vt->y) < VALID_LIMIT && abs(Vt->z) < VALID_LIMIT){
    RetVal = false;
  } 
  return RetVal;
}

float AHRS::UpdateFilter(ButterworthFilter* Filt,float NewReading){
  Filt->_x[2] = Filt->_x[1];
  Filt->_x[1] = Filt->_x[0];
  Filt->_x[0] = NewReading;
  Filt->_y[2] = Filt->_y[1];
  Filt->_y[1] = Filt->_y[0]; 
  Filt->_y[0] = _Xc0*Filt->_x[0] + _Xc1*Filt->_x[1] + _Xc2*Filt->_x[2] - _Yc0*Filt->_y[1] - _Yc1*Filt->_y[2];
  return Filt->_y[0];
}

float AHRS::ScaleTwoFloats(float A,float B,float Scale){
  return (A*Scale + (1.0f-Scale)*B);
}

void AHRS::WriteI2C(byte I2CAddr,byte I2CReg, byte I2CData){                          // Function for writing a byte to a I2C slave
  Wire.beginTransmission(I2CAddr);
  Wire.write(I2CReg);                                
  Wire.write(I2CData);                                             
  Wire.endTransmission();
}

void AHRS::ReadI2C(byte I2CAddr,byte I2CReg, byte I2CLen){                            // Function for reading bytes from a I2C slave
  Wire.beginTransmission(I2CAddr); 
  Wire.write(I2CReg);                                      
  Wire.endTransmission();                                        
  Wire.beginTransmission(I2CAddr);                           
  Wire.requestFrom(I2CAddr,I2CLen);                               
}

void AHRS::StoreCalibrationMatrix(CalibrationMatrix* StartPtr,int Addr){                              
  int _Len = sizeof(CalibrationMatrix);
  byte _Buf[_Len];
  memcpy(StartPtr,&_Buf, _Len);
  for(int _i=0;_i<_Len;_i++){                                          
    EEPROM.write(_i + Addr,_Buf[_i]);
  } 
}

void AHRS::GetCalibrationMatrix(CalibrationMatrix* StartPtr,int Addr){                                                     
  int _Len = sizeof(CalibrationMatrix);
  byte _Buf[_Len];
  for(int _i=0;_i<_Len;_i++){
    _Buf[_i] = EEPROM.read(_i + Addr); 
  }
  memcpy(&_Buf,StartPtr,_Len);
}
