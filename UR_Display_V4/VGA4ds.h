#ifndef VGA4ds_h
#define VGA4ds_h

#include "Arduino.h"
#include <DigitalOut.h>
#include <UR_Global_Define.h>
#include <UR_Global_Structures.h>

#define VGA4DS_OBJ_BUFFER_SIZE                 12
#define VGA4DS_STR_BUFFER_SIZE                 5
#define VGA4DS_RESET_PERIOD                    500
#define VGA4DS_ENABLE_DELAY_PERIOD             6000

#define VGA4DS_READ_OBJ                        0x00
#define VGA4DS_WRITE_OBJ                       0x01
#define VGA4DS_WRITE_STR                       0x02
#define VGA4DS_WRITE_CONTRAST                  0x04
#define VGA4DS_REPORT_OBJ                      0x05
#define VGA4DS_REPORT_EVENT                    0x07

#define VGA4DS_ACK                             0x06
#define VGA4DS_NAK                             0x15
#define VGA4DS_ACK_TIMEOUT                     1000
#define VGA4DS_REQUEST_TIMEOUT                 1000

#define VGA4DS_BUFFER_LENGTH                   0x0A
#define VGA4DS_READ_OBJ_LENGTH                 0x04
#define VGA4DS_WRITE_OBJ_LENGTH                0x06
#define VGA4DS_WRITE_STR_LENGTH                0x03
#define VGA4DS_WRITE_CONTRAST_LENGTH           0x03
#define VGA4DS_REPORT_OBJ_LENGTH               0x06
#define VGA4DS_REPORT_EVENT_LENGTH             0x06

class VGA4ds
{
public:
	VGA4ds();
        void Init(int,long,int);
        void ResetVGA();
        boolean AddObjToBuffer(VGA4dsObject*);  
        boolean AddStrToBuffer(VGA4dsString);
        void Update();
private:
        void WriteObject(VGA4dsObject);
        void WriteString(VGA4dsString);
        void WriteCmd(byte*,int);
        byte Readbyte();
        boolean ReceiveStatus();
        
        boolean _Enable,_AckReq,_StrBufStatus[VGA4DS_STR_BUFFER_SIZE],_ObjBufStatus[VGA4DS_OBJ_BUFFER_SIZE];
        int _Port,_ResetPin;       
        unsigned long _AckMillis,_RstMillis;
    
        VGA4dsString _StrBuf[VGA4DS_STR_BUFFER_SIZE];
        VGA4dsObject _ObjBuf[VGA4DS_OBJ_BUFFER_SIZE];
        
        DigitalOut _rstpin;
};

#endif
