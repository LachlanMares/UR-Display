#ifndef AHRS_h
#define AHRS_h
#include <Wire.h>
#include <EEPROM.h>
#include "Arduino.h"
#include "GlobalStructs.h"

// Conversions
#define TO_DEG(x)              (x*57.2957795131f)                  // *180/pi
#define TO_RAD(x)              (x*0.01745329252f)                  // *pi/180

// Configure
#define MAGNETIC_DECLINATION        TO_RAD(8.0f)                        // Magnetic declination in degrees Brisbane 11 degrees
#define GYRO_SAMPLES                1000                                // Number of samples to calculate gyro bias errors
#define SIGMA                       0.25f                               // Mixing ratio of accelerometers and gyros in attitude filter, for dynamic set to 0.0f
#define ACC_SCALE_FACTOR            8192.0f
#define GYRO_SCALE_FACTOR           16.384f                             // (32768/2000)
#define GYRO_DRIFT                  TO_RAD(0.2f)                        // Gyroscope drift error 0.2 deg/s/s
#define ACCL_EE_ADDR                20
#define MAG_EE_ADDR                 50                                  // Start address of where calibration data is stored
#define MAX_MAG_ANGLE               TO_RAD(10.0f)

// I2C Address
#define MPU6050_ADDR                0x68
#define AK8975C_ADDR                0x0C

// MPU 9150 registers
#define MPUREG_WHOAMI               0x75 
#define	MPUREG_SMPLRT_DIV           0x19 
#define MPUREG_CONFIG               0x1A 
#define MPUREG_GYRO_CONFIG          0x1B
#define MPUREG_ACCEL_CONFIG         0x1C
#define MPUREG_I2C_MST_STATUS       0x36
#define MPUREG_INT_PIN_CFG          0x37
#define	MPUREG_INT_ENABLE           0x38 
#define MPUREG_ACCEL_XOUT_H         0x3B 
#define MPUREG_ACCEL_XOUT_L         0x3C 
#define MPUREG_ACCEL_YOUT_H         0x3D 
#define MPUREG_ACCEL_YOUT_L         0x3E 
#define MPUREG_ACCEL_ZOUT_H         0x3F 
#define MPUREG_ACCEL_ZOUT_L         0x40 
#define MPUREG_TEMP_OUT_H           0x41
#define MPUREG_TEMP_OUT_L           0x42
#define MPUREG_GYRO_XOUT_H          0x43  
#define	MPUREG_GYRO_XOUT_L          0x44 
#define MPUREG_GYRO_YOUT_H          0x45 
#define	MPUREG_GYRO_YOUT_L          0x46 
#define MPUREG_GYRO_ZOUT_H          0x47 
#define	MPUREG_GYRO_ZOUT_L          0x48 
#define MPUREG_USER_CTRL            0x6A 
#define	MPUREG_PWR_MGMT_1           0x6B 
#define	MPUREG_PWR_MGMT_2           0x6C 

// Configuration bits MPU 9150
#define BIT_SLEEP                   0x40
#define BIT_H_RESET                 0x80
#define BITS_CLKSEL                 0x07
#define MPU_CLK_SEL_PLLGYROX        0x01
#define MPU_CLK_SEL_PLLGYROZ        0x03
#define MPU_EXT_SYNC_GYROX          0x02
#define MPU_SAMPLE_200HZ            0x04
#define MPU_SAMPLE_166HZ            0x05
#define MPU_SAMPLE_142HZ            0x06
#define MPU_SAMPLE_125HZ            0x07
#define MPU_SAMPLE_100HZ            0x09
#define MPU_SAMPLE_50HZ             0x13
#define BITS_FS_250DPS              0x00
#define BITS_FS_500DPS              0x08
#define BITS_FS_1000DPS             0x10
#define BITS_FS_2000DPS             0x18
#define BITS_ACC_2G                 0x00
#define BITS_ACC_4G                 0x08
#define BITS_ACC_8G                 0x10
#define BITS_ACC_16G                0x18
#define BITS_FS_MASK                0x18
#define BITS_DLPF_CFG_256HZ_NOLPF2  0x00
#define BITS_DLPF_CFG_188HZ         0x01
#define BITS_DLPF_CFG_98HZ          0x02
#define BITS_DLPF_CFG_42HZ          0x03
#define BITS_DLPF_CFG_20HZ          0x04
#define BITS_DLPF_CFG_10HZ          0x05
#define BITS_DLPF_CFG_5HZ           0x06
#define BITS_DLPF_CFG_2100HZ_NOLPF  0x07
#define BITS_DLPF_CFG_MASK          0x07
#define	BIT_INT_ANYRD_2CLEAR	    0x12
#define	BIT_RAW_RDY_EN		    0x01
#define	BIT_I2C_IF_DIS              0x10

// AK8975C registers
#define AK8975C_ST1                 0x02 
#define AK8975C_ST2                 0x09 
#define AK8975C_CNTL                0x0A 
#define AK8975C_HXL                 0x03
#define AK8975C_ASAX                0x10

// Configuration bits AK8975C
#define SINGLE_READING              0x01  

// Misc
#define I2C_LENGTH                  14
#define MPU6050_LENGTH              14
#define ACCL_DATA_LENGTH            6
#define AK8975C_LENGTH              6
#define AK8975C_CONFIG_LEN          3
#define M_PI                       3.141592653589793238f               // Pi
#define TWO_M_PI                   6.283185307179586477f               // Pi * 2
#define SQRT2                      1.414213562373095048f               // Square root of 2 
#define VALID_LIMIT                0.0001f

//Gyro Mode
#define	SAMPLE                      0x00
#define	NO_SAMPLES                  0x01

typedef struct {
  float Xbias;
  float Xscale;
  float Ybias;
  float Yscale;
  float Zbias;
  float Zscale;
} CalibrationMatrix;

class AHRS
{
public:
	AHRS();
        void Init(byte,byte,byte,int,float);
        boolean Status();
        void Start();
        void Stop();
        void AHRS_ISR();
        void Update();
        void GetEulerAngles(Euler*);
        void ResetQuaternion(Quaternion*);
        void NormaliseQuaternion(Quaternion*);
        void ScaledCopyQ(Quaternion*,float,Quaternion*);
        void QFromAccl(Vector At,Quaternion*);
        float SetPhiFromQuaternion(Quaternion*);
        float SetThetaFromQuaternion(Quaternion*);
        float SetPsiFromQuaternion(Quaternion*);  
        void ResetVector(Vector*);
        void NormaliseVector(Vector*);
        void SetVectorTo(Vector*,float);
        boolean CheckInputValid(Vector*);
        float UpdateFilter(ButterworthFilter*,float);
        float ScaleTwoFloats(float,float,float);
        void WriteI2C(byte,byte,byte);
        void ReadI2C(byte,byte,byte);
        void StoreCalibrationMatrix(CalibrationMatrix*,int);
        void GetCalibrationMatrix(CalibrationMatrix*,int);
        
private:
        boolean _Enable,_GyroReady,_InitMode,_GetNewSample;
        int _ReqSamples,_SampleCounter;
        float _DT,_GyroDriftDt,_HalfDT,_Sigma,_Xc0,_Xc1,_Xc2,_Yc0,_Yc1;
        Vector _Accl,_AcclScale;
        Vector _Gyro,_GyroBiasDt,_GyroScale,_GyroZeroBias;
        Vector _Mag;
        Euler _EulerAngles;
        Quaternion _Q;
        ButterworthFilter _Phi,_Theta,_Psi;
        CalibrationMatrix _AcclC,_MagC;
};

#endif

