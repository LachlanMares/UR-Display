#ifndef GlobalStructs_h
#define GlobalStructs_h

#include <WString.h>

// Structures
typedef struct {
float phi;
float theta;
float psi;
} Euler;

typedef struct {
float x;
float y;
float z;
} Vector;

typedef struct {  
float w;
float x;
float y;
float z;
} Quaternion;

typedef struct {
   float _x[3], _y[3];
} ButterworthFilter;

// Message Structures

#pragma pack(1)
typedef struct {  
  byte Status;
  byte Error[3];
  byte Spare[4]; 
} Status_Msg_Struct;
#pragma pack()

#pragma pack(1)
typedef struct {  
  float Latitude;
  float Longitude; 
} GPS_Pos_Msg_Struct;
#pragma pack()

#pragma pack(1)
typedef struct {  
  int Year;
  byte Month;
  byte Day;
  byte Hour;
  byte Minute;
  byte Second;
  byte Hundredths;
} GPS_Time_Msg_Struct;
#pragma pack()

#pragma pack(1)
typedef struct {  
  float Pitch;
  float Roll; 
} IMU_PR_Msg_Struct;
#pragma pack()

#pragma pack(1)
typedef struct {  
  float Speed;
  float Heading; 
} IMU_SH_Msg_Struct;
#pragma pack()

#pragma pack(1)
typedef struct {  
  int HeightLeft;
  int HeightRight;
  boolean Input1;
  boolean Input2;
  boolean Input3;
  boolean Input4;
  boolean Input5;
  boolean Input6;
  boolean Input7;
  boolean Input8;
  boolean Relay1;
  boolean Relay2;
  boolean Relay3;
  boolean Relay4;
  boolean Relay5;
  boolean Relay6;
  boolean Relay7;
  boolean Relay8;
  int Spare;
} IMU_IO_Msg_Struct;
#pragma pack()

#endif
