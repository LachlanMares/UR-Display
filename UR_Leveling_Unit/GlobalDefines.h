#ifndef GlobalDefines_h
#define GlobalDefines_h

// Conversions
#define TO_DEG(x)              (x*57.2957795131f)                  // *180/pi
#define TO_RAD(x)              (x*0.01745329252f)                  // *pi/180

// Tunable Parameters
#define MAX_MAG_ANGLE          TO_RAD(4.0f)                        // Magnetometer not so accurate when not level, set tolerance of 8 degrees
#define MAGNETIC_DECLINATION   TO_RAD(8.0f)                        // Magnetic declination in degrees Brisbane 11 degrees
#define MAG_SAMPLES            800                                 // Number of samples to calculate hard iron errors
#define GYRO_SAMPLES           1000                                // Number of samples to calculate gyro bias errors
#define GMT_HOUR               10                                  // Brisbane GMT + 10 hours
#define GMT_MINUTE             0                                   // Brisbane GMT + 0 minutes
#define PRINT_VALUE            2                                   // Select print option from serial port
#define PRINTS_PER_SEC         5                                   // Select frequency of print option
#define GRADIENT_LOOPS         3                                   // Select number of iterations of the gradient descent algorithm
#define GRADIENT_STEP          0.05f                               // Select step size for gradient descent algorithm
#define ERROR_LOG_SIZE         3                                   // Number of errors that can stored in log, must change display unit value to match
#define LEVEL_TOLERANCE        TO_RAD(1.2f)                        // Threshold for leveling of harvester
#define PROXIMITY_ERROR_TIME   1000                                // Time in mS that both proximities have to be off before generating an error
#define SIGMA                  0.25f                               // Mixing ratio of accelerometers and gyros in attitude filter, for dynamic set to 0.0f

// System constants
#define M_PI                   3.141592653589793238f               // Pi
#define TWO_M_PI               6.283185307179586477f               // Pi * 2
#define SQRT2                  1.414213562373095048f               // Square root of 2 

// Timer
#define 10mS                   10
#define 25mS                   25                                 
#define 50mS                   50
#define 100mS                  100
#define 250mS                  250
#define 1000mS                 1000
#define 5000mS                 5000
#define TIMER2                 25mS 

#define PROXIMITY_ERROR        PROXIMITY_ERROR_TIME/RELAY_INTERVAL // 
#define OUT_OF_TOLERANCE       LEVEL_TOLERANCE_TIME/INT_TIME       // Used with a counter to set out of balance time/filter
#define PRINT_TIME            (1000/PRINTS_PER_SEC)/INT_TIME       // Calculates the interval between prints

// Status Message
#define STATUS_INIT            0x01                                // Unit initialising
#define STATUS_READY           0x02                                // Unit ready
#define STATUS_AUTO            0x03                                // Unit is in auto mode
#define STATUS_USER_CMD        0x04                                // Unit is in auto mode but commanded to move manually
#define STATUS_MANUAL          0x05                                // Unit is in manual mode, no active control
#define STATUS_FAULT           0x06                                // Unit is in fault, no active control

// Proximity States
#define BOTH_ON                0x00
#define P1_NOT_P2              0x01
#define P2_NOT_P1              0x02
#define BOTH_OFF               0x03

// Switch States
#define AUTO_OFF               0x00
#define AUTO_ON                0x01
#define NO_CMD                 0x00
#define UP_CMD                 0x01
#define DOWN_CMD               0x02
#define SW_ERROR               0x03

// Error Codes
#define NO_ERROR               0x00                                // No Error
#define MAG_DATA_ERROR         0x01                                // Incorrect information recieved from magnetometer
#define QUARTERNION_ERROR      0x02                                // All 4 elements of Quaternion are zero
#define MAG_CONF_ERROR         0x03                                // Error occurred while trying to configure magnetometer
#define MPU_DATA_ERROR         0x04                                // Incorrect information recieved from MPU-9150
#define LEFT_PROX_ERROR        0x05                                // Both left proximities are off
#define RIGHT_PROX_ERROR       0x06                                // Both right proximities are off
#define LEFT_SWITCH_ERROR      0x07                                // Both left switches are on
#define RIGHT_SWITCH_ERROR     0x08                                // Both right switches are on

//CanSpeed - Assuming 16MHz crystal
#define CAN_5KBPS        1
#define CAN_10KBPS       2
#define CAN_20KBPS       3
#define CAN_40KBPS       4
#define CAN_50KBPS       5
#define CAN_80KBPS       6
#define CAN_100KBPS      7
#define CAN_125KBPS      8
#define CAN_200KBPS      9
#define CAN_250KBPS      10
#define CAN_500KBPS      11

#endif
