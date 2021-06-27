////////////////////////////////////////////////////////////////////////////////
//    ScopeFun Oscilloscope ( http://www.scopefun.com )
//    Copyright (C) 2016 - 2019 David Košenina
//
//    This file is part of ScopeFun Oscilloscope.
//
//    ScopeFun Oscilloscope is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    ScopeFun Oscilloscope is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this ScopeFun Oscilloscope.  If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////
#ifndef SCOPEFUN_API_HEADER
#define SCOPEFUN_API_HEADER

/*----------------------------------------

      ScopeFun API - Types

----------------------------------------*/

typedef unsigned char      byte;
typedef short              ishort;
typedef unsigned short     ushort;
typedef int                iint;
typedef unsigned int       uint;
typedef long long          ilarge;
typedef unsigned long long ularge;

/*----------------------------------------
   version
----------------------------------------*/
#define HARDWARE_VERSION_1 1
#define HARDWARE_VERSION_2 2

/*----------------------------------------

      ScopeFun API - Frame Constants

----------------------------------------*/
#define SCOPEFUN_FRAME_1_HEADER                  (2464)
#define SCOPEFUN_FRAME_1_DATA                   (60000)
#define SCOPEFUN_FRAME_1_PACKET                   (512)
#define SCOPEFUN_FRAME_2_HEADER                  (1024)
#define SCOPEFUN_FRAME_2_DATA           (512*1000*1000)
#define SCOPEFUN_FRAME_2_PACKET             (1024*1024)
#define SCOPEFUN_FRAME_DATA             (512*1000*1000)
#define SCOPEFUN_FRAME_MEMORY           (512*1024*1024)
#define SCOPEFUN_FRAME_PACKET               (1024*1024)

/*----------------------------------------

      ScopeFun API - Constants

----------------------------------------*/
#define SCOPEFUN_INVALID_CLIENT             -1
#define SCOPEFUN_MAX_CLIENT                 8
#define SCOPEFUN_SERVER_ID                  SCOPEFUN_MAX_CLIENT
#define SCOPEFUN_DISPLAY                    10000
#define SCOPEFUN_FIRMWARE_FX2               16384
#define SCOPEFUN_FIRMWARE_FPGA              (4*1024*1024)
#define SCOPEFUN_EEPROM_BYTES               (256*1024)
#define SCOPEFUN_EEPROM_FIRMWARE_NAME_BYTES (16)
#define SCOPEFUN_GENERATOR                  32768

/*----------------------------------------

      ScopeFun API - capture type

----------------------------------------*/
#define SCOPEFUN_CAPTURE_TYPE_NONE   0
#define SCOPEFUN_CAPTURE_TYPE_HEADER 1
#define SCOPEFUN_CAPTURE_TYPE_DATA   2

/*----------------------------------------

      ScopeFun API - Errors

----------------------------------------*/
#define SCOPEFUN_SUCCESS                0
#define SCOPEFUN_FAILURE               -1

/*----------------------------------------

    ScopeFun API - Array

----------------------------------------*/
#define SCOPEFUN_ARRAY_SWIG(type,size)             \
    %extend{                                     \
        int __len__() const { return size; }      \
        type __getitem__(int i)                   \
        {                                         \
            return self->bytes[i];                 \
        }                                         \
        void __setitem__(int i,type v)            \
        {                                         \
            self->bytes[i] = v;                    \
        }                                         \
    }                                            \

#ifdef SWIG
#define SCOPEFUN_ARRAY(name,type,size)                                                                             \
    typedef struct {                                                                                                   \
        type bytes[size];                                                                                               \
        SCOPEFUN_ARRAY_SWIG(type,size)                                                                                  \
    }name;                                                                                                             \
    %typemap(memberin) type bytes[size]                                                                                \
    {                                                                                                                  \
        memcpy($1, $input, size);                                                                                       \
    }                                                                                                                  \
    %typemap(in) name* buffer(name *tmp)                                                                               \
    {                                                                                                                  \
        if ((SWIG_ConvertPtr($input, (void **)tmp, $1_descriptor, SWIG_POINTER_EXCEPTION | SWIG_POINTER_DISOWN)) == -1) \
            return NULL;                                                                                                 \
        $1 = &tmp;                                                                                                      \
    }                                                                                                                  \
    %typemap(argout) name*                                                                                             \
    {                                                                                                                  \
        %append_output(SWIG_NewPointerObj(%as_voidptr($1), $1_descriptor, SWIG_POINTER_OWN));                           \
    }
#else
#define SCOPEFUN_ARRAY(name,type,size)          \
    typedef struct {                                \
        type bytes[size];                            \
    }name;
#endif

/*----------------------------------------

   ScopeFun API - Structures

----------------------------------------*/

/*----------------------------------------
   SHardware1
----------------------------------------*/
typedef struct
{
    ushort control;
    ushort vgaina;
    ushort vgainb;
    ishort offseta;
    ishort offsetb;
    ushort siggen;
    ushort trigger;
    ushort analogswitch;
    ushort triggerMode;
    ushort triggerSource;
    ushort triggerSlope;
    ishort triggerLevel;
    ushort triggerHis;
    ushort triggerPercent;
    ushort xRange;
    ushort sampleSize;
    ushort holdoffH;
    ushort holdoffL;
    ushort generatorType0;
    ushort generatorVoltage0;
    ishort generatorOffset0;
    ushort generatorDeltaH0;
    ushort generatorDeltaL0;
    ushort generatorSqueareDuty0;
    ushort generatorType1;
    ushort generatorVoltage1;
    ishort generatorOffset1;
    ushort generatorDeltaH1;
    ushort generatorDeltaL1;
    ushort generatorSqueareDuty1;
    ushort digitalPattern1a;
    ushort digitalPattern1b;
    ushort digitalMask1;
    ushort digitalPattern2a;
    ushort digitalPattern2b;
    ushort digitalMask2;
    ushort digitalPattern3a;
    ushort digitalPattern3b;
    ushort digitalMask3;
    ushort digitalPattern4a;
    ushort digitalPattern4b;
    ushort digitalMask4;
    ushort dt_delayMaxcnt1;
    ushort dt_delayMaxcnt2;
    ushort dt_delayMaxcnt3;
    ushort dt_delayMaxcnt4;
    ushort dt_control;
    ushort digitalVoltage;
    ushort digitalInputOutput;
    ushort digitalOutputBit;
    ushort digitalOutputMask;
    ushort digitalClkDivideH;
    ushort digitalClkDivideL;
} SHardware1;


/*----------------------------------------
SHardware2
----------------------------------------*/
typedef struct
{
    ushort controlAddr;
    ushort controlData;
    ushort vgaina;
    ushort vgainb;
    ushort offseta;
    ushort offsetb;
    ushort analogswitch;
    ushort triggerMode;
    ushort triggerSource;
    ushort triggerSlope;
    short  triggerLevel;
    ushort triggerHis;
    ushort triggerPercent;
    ushort xRange;
    ushort holdoffH;
    ushort holdoffL;
    ushort sampleSizeH;
    ushort sampleSizeL;
    ushort generatorType0;
    ushort generatorVoltage0;
    short  generatorOffset0;
    ushort generatorDeltaH0;
    ushort generatorDeltaL0;
    ushort generatorSqueareDuty0;
    ushort generatorType1;
    ushort generatorVoltage1;
    short  generatorOffset1;
    ushort generatorDeltaH1;
    ushort generatorDeltaL1;
    ushort generatorSqueareDuty1;
    ushort digitalPattern1a;
    ushort digitalPattern1b;
    ushort digitalMask1;
    ushort digitalPattern2a;
    ushort digitalPattern2b;
    ushort digitalMask2;
    ushort digitalPattern3a;
    ushort digitalPattern3b;
    ushort digitalMask3;
    ushort digitalPattern4a;
    ushort digitalPattern4b;
    ushort digitalMask4;
    ushort dt_delayMaxcnt1;
    ushort dt_delayMaxcnt2;
    ushort dt_delayMaxcnt3;
    ushort dt_delayMaxcnt4;
    ushort dt_control;
    ushort digitalVoltage;
    ushort digitalInputOutput;
    ushort digitalOutputBit;
    ushort digitalOutputMask;
    ushort digitalClkDivideH;
    ushort digitalClkDivideL;
    ushort average;
    uint   reserved2;
    uint   reserved3;
    uint   reserved4;
    uint   reserved5;
    uint   reserved6;
} SHardware2;

/*----------------------------------------
   SFrameHeader
----------------------------------------*/
SCOPEFUN_ARRAY(SArrayMagic1,     byte,   2);
SCOPEFUN_ARRAY(SArrayTrigger1,   byte,   8);
SCOPEFUN_ARRAY(SArrayHardware1,  ushort, 53);
SCOPEFUN_ARRAY(SArrayPadding1,   byte, 2343);
SCOPEFUN_ARRAY(SArrayFrameSize1, byte, 4);

SCOPEFUN_ARRAY(SArrayMagic2,            byte,    4);
SCOPEFUN_ARRAY(SArrayETS2,              byte,    1);
SCOPEFUN_ARRAY(SArrayPaddingBefore2,    byte,   251);
SCOPEFUN_ARRAY(SArrayHardware2,         byte,   128);
SCOPEFUN_ARRAY(SArrayPaddingAfter2,     byte,   639);
SCOPEFUN_ARRAY(SArrayCrc2,              byte,     1);
typedef struct
{
    SArrayMagic1     magic;
    SArrayTrigger1   trigger;
    SArrayHardware1  hardware;
    SArrayPadding1   padding;
    SArrayFrameSize1 frameSize;
    byte             crc;
} SFrameHeader1;

typedef struct
{
    SArrayMagic2         magic;
    SArrayETS2           etsDelay;
    SArrayPaddingBefore2 paddBefore;
    SArrayHardware2      hardware;
    SArrayPaddingAfter2  paddAfter;
    SArrayCrc2           crc;
} SFrameHeader2;

/*----------------------------------------
   SFrameData
----------------------------------------*/
SCOPEFUN_ARRAY(SArrayFrameData, byte, SCOPEFUN_FRAME_MEMORY);
typedef struct
{
    SArrayFrameData data;
} SFrameData;

/*----------------------------------------
   SEEPROM
----------------------------------------*/
SCOPEFUN_ARRAY(SArrayEEPROM, byte, SCOPEFUN_EEPROM_BYTES);
typedef struct
{
    SArrayEEPROM data;
} SEeprom;

/*----------------------------------------
   SGUID
----------------------------------------*/
typedef struct
{
    unsigned int    data1;
    unsigned short  data2;
    unsigned short  data3;
    unsigned char   data4[8];
} SGUID;

/*----------------------------------------
   SFx2
----------------------------------------*/
SCOPEFUN_ARRAY(SArrayFx2, byte, SCOPEFUN_FIRMWARE_FX2);
typedef struct
{
    uint      size;
    SArrayFx2 data;
} SFx2;

/*----------------------------------------
   SFpga
----------------------------------------*/
SCOPEFUN_ARRAY(SArrayFpga, byte, SCOPEFUN_FIRMWARE_FPGA);
typedef struct
{
    uint       size;
    SArrayFpga data;
} SFpga;

/*----------------------------------------
   SGenerator
----------------------------------------*/
SCOPEFUN_ARRAY(SArrayGeneratorAnalog0, ishort, SCOPEFUN_GENERATOR);
SCOPEFUN_ARRAY(SArrayGeneratorAnalog1, ishort, SCOPEFUN_GENERATOR);
SCOPEFUN_ARRAY(SArrayGeneratorDigital, ushort, SCOPEFUN_GENERATOR);
typedef struct
{
    SArrayGeneratorAnalog0 analog0;
    SArrayGeneratorAnalog1 analog1;
    SArrayGeneratorDigital digital;
} SGenerator;

/*----------------------------------------
   ESimulateType
----------------------------------------*/
typedef enum _ESimulateType
{
    stSin,
    stCos,
    stInc,
    stDec,
    stConstant,
    stRandom,
    stSquare,
    stDelta,
} ESimulateType;

/*----------------------------------------
EMode
----------------------------------------*/
typedef enum _EMode
{
    mIdle,
    mCapture,
    mSimulate,
} EMode;

/*----------------------------------------
   SSimulate
----------------------------------------*/
typedef struct
{
    byte                    active0;
    byte                    active1;
    ESimulateType           type0;
    ESimulateType           type1;
    float                   period0;
    float                   period1;
    float                   peakToPeak0;
    float                   peakToPeak1;
    float                   avery0;
    float                   avery1;
    float                   speed0;
    float                   speed1;
    uint                    etsIndex;
    uint                    etsActive;
    uint                    etsMax;
    float                   time;
    float                   voltage0;
    float                   voltage1;
} SSimulate;

/*----------------------------------------
   SUsbGuid
----------------------------------------*/
typedef struct
{
    uint   data1;
    ushort data2;
    ushort data3;
    byte   data4[8];
} SUsbGuid;

/*----------------------------------------
   SUsb
----------------------------------------*/
typedef struct
{
    uint         idVendor;
    uint         idProduct;
    uint         idSerial;
    uint         timeoutEp2;
    uint         timeoutEp4;
    uint         timeoutEp6;
    uint         xferSize;
    SUsbGuid     guid;
} SUsb;

/*----------------------------------------
   SDisplay
----------------------------------------*/
SCOPEFUN_ARRAY(SArrayDisplayAnalog0, float,  SCOPEFUN_DISPLAY);
SCOPEFUN_ARRAY(SArrayDisplayAnalog1, float,  SCOPEFUN_DISPLAY);
SCOPEFUN_ARRAY(SArrayDisplayAnalogF, float,  SCOPEFUN_DISPLAY);
SCOPEFUN_ARRAY(SArrayDisplayDigital, ushort, SCOPEFUN_DISPLAY);
typedef struct
{
    SArrayDisplayAnalog0 analog0;
    SArrayDisplayAnalog1 analog1;
    SArrayDisplayAnalogF analogF;
    SArrayDisplayDigital digital;
    byte                 ch0;
    byte                 ch1;
    byte                 fun;
    byte                 dig;
} SDisplay;

/*----------------------------------------
   EUpload
----------------------------------------*/
typedef enum _EUpload
{
    uChannel0,
    uChannel1,
    uDigital,
    uFunction,
} EUpload;

/*----------------------------------------
   EApiMode
----------------------------------------*/
typedef enum _EHardwareMode
{
    amUSB,
    amNetwork,
} EApiMode;

/*----------------------------------------
   SActiveClients
----------------------------------------*/
SCOPEFUN_ARRAY(SArraySClientId, byte, SCOPEFUN_MAX_CLIENT);
typedef struct
{
    byte            cnt;
    SArraySClientId client;
} SActiveClients;

/*----------------------------------------
   ECaptureMode
----------------------------------------*/
typedef enum _ECaptureMode
{
    cmOff,
    cmAllFrames,
    cmRequest,
} ECaptureMode;

/*----------------------------------------
   SFrameInfo
----------------------------------------*/
typedef struct
{
    uint                    version;
    uint                    header;
    uint                    data;
    uint                    packet;
} SFrameInfo;

/*--------------------------------------

      ScopeFun API - Network

--------------------------------------*/
typedef enum _EMessage
{
    mClientConnect,
    mClientDisconnect,
    mClientDisplay,
    mUpload,
    mDownload,
    mHardwareOpen,
    mHardwareIsOpened,
    mHardwareReset,
    mHardwareConfig1,
    mHardwareConfig2,
    mHardwareCapture,
    mHardwareCaptureFrame,
    mHardwareUploadFx2,
    mHardwareUploadFpga,
    mHardwareUploadGenerator,
    mHardwareEepromRead,
    mHardwareEepromReadFirmwareID,
    mHardwareEepromWrite,
    mHardwareEepromErase,
    mHardwareClose,
    mLast,
} EMessage;

typedef struct
{
    char                    magic[8];
    uint                    size;
    uint                    message;
    uint                    error;
} messageHeader;

/*----------------------------------------

           Client -> Server

----------------------------------------*/

/*----------------------------------------
   client
----------------------------------------*/

typedef struct
{
    messageHeader           header;
    uint                    maxMemory;
} csClientConnect;

typedef struct
{
    messageHeader           header;
    uint                    reserved;
} csClientDisconnect;

typedef struct
{
    messageHeader           header;
    SDisplay                display;
    uint                    clientId;
} csClientDisplay;

/*----------------------------------------
   upload
----------------------------------------*/
typedef struct
{
    messageHeader           header;
    SFrameInfo              frame;
    SSimulate               simulate;
    uint                    simOnOff;
} csUpload;

/*----------------------------------------
   download
----------------------------------------*/
typedef struct
{
    messageHeader           header;
    uint                    reserved;
} csDownload;

/*----------------------------------------
   hardware
----------------------------------------*/
typedef struct
{
    messageHeader           header;
    SUsb                    usb;
    uint                    version;
} csHardwareOpen;

typedef struct
{
    messageHeader           header;
    uint                    reserved;
} csHardwareIsOpened;

typedef struct
{
    messageHeader           header;
    uint                    reserved;
} csHardwareReset;

typedef struct
{
    messageHeader           header;
    SHardware1              config;
} csHardwareConfig1;

typedef struct
{
    messageHeader           header;
    SHardware2              config;
} csHardwareConfig2;

typedef struct
{
    messageHeader           header;
    uint                    len;
    uint                    type;
} csHardwareCapture;

typedef struct
{
    messageHeader           header;
    SFx2                    fx2;
} csHardwareUploadFx2;

typedef struct
{
    messageHeader           header;
    SFpga                   fpga;
} csHardwareUploadFpga;

typedef struct
{
    messageHeader           header;
    SGenerator              data;
} csHardwareUploadGenerator;

typedef struct
{
    messageHeader           header;
    uint                    size;
    uint                    address;
    uint                    reserved;
} csHardwareEepromRead;

typedef struct
{
   messageHeader           header;
   uint                    size;
   uint                    address;
   uint                    reserved;
} csHardwareEepromReadFirmwareID;

typedef struct
{
    messageHeader           header;
    uint                    size;
    uint                    address;
    SEeprom                 eeprom;
} csHardwareEepromWrite;

typedef struct
{
    messageHeader           header;
    uint                    erase;
} csHardwareEepromErase;

typedef struct
{
    messageHeader           header;
    uint                    reserved;
} csHardwareClose;

/*----------------------------------------

         Server -> Client

----------------------------------------*/

/*----------------------------------------
   client
----------------------------------------*/
typedef struct
{
    messageHeader           header;
    uint                    id;
} scClientConnect;

typedef struct
{
    messageHeader           header;
    uint                    reserved;
} scClientDisconnect;

typedef struct
{
    messageHeader           header;
    uint                    reserved;
} scClientDisplay;

/*----------------------------------------
   upload
----------------------------------------*/
typedef struct
{
    messageHeader           header;
    uint                    reserved;
} scUpload;

/*----------------------------------------
   download
----------------------------------------*/
typedef struct
{
    messageHeader           header;
    SFrameInfo              frame;
    SSimulate               simulate;
    uint                    simOnOff;
    SDisplay                display;
    SActiveClients          active;
} scDownload;

/*----------------------------------------
hardware
----------------------------------------*/

typedef struct
{
    messageHeader           header;
    uint                    reserved;
} scHardwareOpen;

typedef struct
{
    messageHeader           header;
    uint                    opened;
} scHardwareIsOpened;

typedef struct
{
    messageHeader           header;
    uint                    reserved;
} scHardwareReset;

typedef struct
{
    messageHeader           header;
    uint                    reserved;
} scHardwareConfig1;

typedef struct
{
    messageHeader           header;
    uint                    reserved;
} scHardwareConfig2;

typedef struct
{
    messageHeader           header;
    uint                    bytes;
    SFrameData              data;
} scHardwareCapture;

typedef struct
{
    messageHeader           header;
    uint                    reserved;
} scHardwareUploadFx2;

typedef struct
{
    messageHeader           header;
    uint                    reserved;
} scHardwareUploadFpga;

typedef struct
{
    messageHeader           header;
    uint                    reserved;
} scHardwareUploadGenerator;

typedef struct
{
    messageHeader           header;
    SEeprom                 eeprom;
} scHardwareEepromRead;

typedef struct
{
   messageHeader           header;
   SEeprom                 eeprom;
} scHardwareEepromReadFirmwareID;

typedef struct
{
    messageHeader           header;
    uint                    reserved;
} scHardwareEepromWrite;

typedef struct
{
    messageHeader           header;
    uint                    reserved;
} scHardwareEepromErase;

typedef struct
{
    messageHeader           header;
    uint                    reserved;
} scHardwareClose;

/*----------------------------------------

ScopeFun API - Context

----------------------------------------*/
typedef struct
{
    int value;
} SAtomic;
typedef int SSpinLock;

typedef struct
{
    SAtomic           on;
    SSimulate         data;
    uint              active;
} SCtxSimulate;

typedef struct
{
    uint              maxMemory;
    SFrameData*       data;
    SFrameInfo        info;
    uint              received;
} SCtxFrame;

typedef struct
{
    SDisplay          data;
} SCtxDisplay;

typedef struct
{
    uint              version;
    uint              major;
    uint              minor;
    uint              active;
    EApiMode          mode;
    SAtomic           thread;
    SSpinLock         lock;
    uint              timeout;
} SCtxApi;

typedef struct
{
    SActiveClients    active;
} SCtxServer;

typedef struct
{
    uint              id;
    uint              connected;
    SDisplay          display;
} SCtxClient;

typedef struct
{
    SCtxApi           api;
    SCtxSimulate      simulate;
    SCtxFrame         frame;
    SCtxServer        server;
    SCtxClient        client;
    byte*             net;
    byte*             usb;
} SFContext;

/*----------------------------------------

   ScopeFun API - Functions

----------------------------------------*/

/*----------------------------------------
   defines
----------------------------------------*/
#define SCOPEFUN_CREATE(name) extern name* sfCreate##name();
#define SCOPEFUN_DELETE(name) extern void  sfDelete##name(name* INPUT);

/*----------------------------------------
   create
----------------------------------------*/
SCOPEFUN_CREATE(SFContext)
SCOPEFUN_CREATE(SDisplay)
SCOPEFUN_CREATE(SSimulate)
SCOPEFUN_CREATE(SFrameInfo)
SCOPEFUN_CREATE(SUsb)
SCOPEFUN_CREATE(SHardware1)
SCOPEFUN_CREATE(SHardware2)
SCOPEFUN_CREATE(SFx2)
SCOPEFUN_CREATE(SFpga)
SCOPEFUN_CREATE(SGenerator)
SCOPEFUN_CREATE(SEeprom)
SCOPEFUN_CREATE(SActiveClients)
extern SFrameData* sfCreateSFrameData(SFContext* ctx, int memory);

/*----------------------------------------
   delete
----------------------------------------*/
SCOPEFUN_DELETE(SFContext)
SCOPEFUN_DELETE(SDisplay)
SCOPEFUN_DELETE(SSimulate)
SCOPEFUN_DELETE(SFrameInfo)
SCOPEFUN_DELETE(SFrameData)
SCOPEFUN_DELETE(SUsb)
SCOPEFUN_DELETE(SHardware1)
SCOPEFUN_DELETE(SHardware2)
SCOPEFUN_DELETE(SFrameData)
SCOPEFUN_DELETE(SFx2)
SCOPEFUN_DELETE(SFpga)
SCOPEFUN_DELETE(SGenerator)
SCOPEFUN_DELETE(SEeprom)
SCOPEFUN_DELETE(SActiveClients)

#ifdef SWIG

    #define SCOPEFUN_API extern

    /*----------------------------------------
    api
    ----------------------------------------*/
    SCOPEFUN_API int sfApiInit();
    SCOPEFUN_API int sfApiCreateContext(SFContext* INOUT, int INPUT);
    SCOPEFUN_API int sfApiDeleteContext(SFContext* INPUT);
    SCOPEFUN_API int sfApiVersion(SFContext* INPUT, int* OUTPUT, int* OUTPUT, int* OUTPUT);
    SCOPEFUN_API int sfApiExit();

    /*----------------------------------------
    is
    ----------------------------------------*/
    SCOPEFUN_API int sfIsUsb(SFContext* INPUT);
    SCOPEFUN_API int sfIsNetwork(SFContext* INPUT);
    SCOPEFUN_API int sfIsSimulate(SFContext* INPUT);
    SCOPEFUN_API int sfIsActive(SFContext* INPUT);
    SCOPEFUN_API int sfIsConnected(SFContext* INPUT);
    SCOPEFUN_API int sfIsThreadSafe(SFContext* INPUT);

    /*----------------------------------------
    set
    ----------------------------------------*/
    SCOPEFUN_API int sfSetThreadSafe(SFContext* INPUT, int INPUT);
    SCOPEFUN_API int sfSetNetwork(SFContext* INPUT);
    SCOPEFUN_API int sfSetUsb(SFContext* INPUT);
    SCOPEFUN_API int sfSetActive(SFContext* INPUT, int INPUT);
    SCOPEFUN_API int sfSetTimeOut(SFContext* INPUT, int INPUT);
    SCOPEFUN_API int sfSetFrameVersion(SFContext* INPUT, int INPUT);
    SCOPEFUN_API int sfSetFrameHeader(SFContext* INPUT, int INPUT);
    SCOPEFUN_API int sfSetFrameData(SFContext* INPUT, int INPUT);
    SCOPEFUN_API int sfSetFramePacket(SFContext* INPUT, int INPUT);
    SCOPEFUN_API int sfSetSimulateData(SFContext* INPUT, SSimulate* INPUT);
    SCOPEFUN_API int sfSetSimulateOnOff(SFContext* INPUT, int INPUT);


    /*----------------------------------------
    get
    ----------------------------------------*/
    SCOPEFUN_API int sfGetTimeOut(SFContext* INPUT, int* OUTPUT);
    SCOPEFUN_API int sfGetFrameVersion(SFContext* INPUT, int* OUTPUT);
    SCOPEFUN_API int sfGetFrameHeader(SFContext* INPUT, int* OUTPUT);
    SCOPEFUN_API int sfGetFrameData(SFContext* INPUT, int* OUTPUT);
    SCOPEFUN_API int sfGetFramePacket(SFContext* INPUT, int* OUTPUT);
    SCOPEFUN_API int sfGetClientDisplay(SFContext* INPUT, SDisplay* INOUT);
    SCOPEFUN_API int sfGetClientId(SFContext* INPUT, int* OUTPUT);
    SCOPEFUN_API int sfGetClientActiveIds(SFContext* INPUT, SActiveClients* INOUT);
    SCOPEFUN_API int sfGetSimulateData(SFContext* INPUT, SSimulate* INOUT);


    /*----------------------------------------
    client
    ----------------------------------------*/
    SCOPEFUN_API int sfClientConnect(SFContext* INPUT, const char* INPUT, int INPUT);
    SCOPEFUN_API int sfClientDisconnect(SFContext* INPUT);
    SCOPEFUN_API int sfClientDisplay(SFContext* INPUT, int INPUT, SDisplay* INPUT);

    /*----------------------------------------
    server
    ----------------------------------------*/
    SCOPEFUN_API int sfServerDownload(SFContext* INPUT);
    SCOPEFUN_API int sfServerUpload(SFContext* INPUT);

    /*----------------------------------------
    hardware
    ----------------------------------------*/
    SCOPEFUN_API int sfHardwareOpen(SFContext* INPUT, SUsb* INPUT, int INPUT);
    SCOPEFUN_API int sfHardwareIsOpened(SFContext* INPUT, int* OUTPUT);
    SCOPEFUN_API int sfHardwareReset(SFContext* INPUT);
    SCOPEFUN_API int sfHardwareConfig1(SFContext* INPUT, SHardware1* INPUT);
    SCOPEFUN_API int sfHardwareConfig2(SFContext* INPUT, SHardware2* INPUT);
    SCOPEFUN_API int sfHardwareCapture(SFContext* INPUT, SFrameData* INOUT, int INPUT, int* OUTPUT, int INPUT);
    SCOPEFUN_API int sfHardwareUploadFx2(SFContext* INPUT, SFx2* INPUT);
    SCOPEFUN_API int sfHardwareUploadFpga(SFContext* INPUT, SFpga* INPUT);
    SCOPEFUN_API int sfHardwareUploadGenerator(SFContext* INPUT, SGenerator* INPUT);
    SCOPEFUN_API int sfHardwareEepromRead(SFContext* INPUT, SEeprom* INOUT, int INPUT, int INPUT);
    SCOPEFUN_API int sfHardwareEepromReadFirmwareID(SFContext* INPUT, SEeprom* INOUT, int INPUT, int INPUT);
    SCOPEFUN_API int sfHardwareEepromWrite(SFContext* INPUT, SEeprom* INPUT, int INPUT, int INPUT);
    SCOPEFUN_API int sfHardwareEepromErase(SFContext* INPUT);
    SCOPEFUN_API int sfHardwareClose(SFContext* INPUT);

    /*----------------------------------------
    Simulate
    ----------------------------------------*/
    SCOPEFUN_API int sfSimulate(SFContext* INPUT, double INPUT);

#else

    #ifdef SCOPEFUN_API_STATIC
        #define SCOPEFUN_API extern
    #else
        #ifdef SCOPEFUN_API_EXPORT
            #if defined(PLATFORM_LINUX ) || defined(PLATFORM_MINGW) || defined(PLATFORM_MAC)
                #define SCOPEFUN_API  __attribute__((visibility("default")))
            #else
                #define SCOPEFUN_API _declspec(dllexport)
            #endif
        #else
            #if defined(PLATFORM_LINUX ) || defined(PLATFORM_MINGW) || defined(PLATFORM_MAC) || defined(PLATFORM_WIN)
                #define SCOPEFUN_API
            #else
                #define SCOPEFUN_API _declspec(dllimport)
            #endif
        #endif
    #endif

    /*----------------------------------------
    api
    ----------------------------------------*/
    SCOPEFUN_API int sfApiInit();
    SCOPEFUN_API int sfApiCreateContext(SFContext* ctx, int maxMemory);
    SCOPEFUN_API int sfApiDeleteContext(SFContext* ctx);
    SCOPEFUN_API int sfApiVersion(SFContext* ctx, int* version, int* major, int* minor);
    SCOPEFUN_API int sfApiExit();

    /*----------------------------------------
    is
    ----------------------------------------*/
    SCOPEFUN_API int sfIsUsb(SFContext* ctx);
    SCOPEFUN_API int sfIsNetwork(SFContext* ctx);
    SCOPEFUN_API int sfIsSimulate(SFContext* ctx);
    SCOPEFUN_API int sfIsActive(SFContext* ctx);
    SCOPEFUN_API int sfIsConnected(SFContext* ctx);
    SCOPEFUN_API int sfIsThreadSafe(SFContext* ctx);

    /*----------------------------------------
    set
    ----------------------------------------*/
    SCOPEFUN_API int sfSetThreadSafe(SFContext* ctx, int threadSafe);
    SCOPEFUN_API int sfSetNetwork(SFContext* ctx);
    SCOPEFUN_API int sfSetUsb(SFContext* ctx);
    SCOPEFUN_API int sfSetActive(SFContext* ctx, int active);
    SCOPEFUN_API int sfSetTimeOut(SFContext* ctx, int timeout);
    SCOPEFUN_API int sfSetFrameVersion(SFContext* ctx, int version);
    SCOPEFUN_API int sfSetFrameHeader(SFContext* ctx, int header);
    SCOPEFUN_API int sfSetFrameData(SFContext* ctx, int data);
    SCOPEFUN_API int sfSetFramePacket(SFContext* ctx, int packet);
    SCOPEFUN_API int sfSetSimulateData(SFContext* ctx, SSimulate* sim);
    SCOPEFUN_API int sfSetSimulateOnOff(SFContext* ctx, int on);

    /*----------------------------------------
    get
    ----------------------------------------*/
    SCOPEFUN_API int sfGetTimeOut(SFContext* ctx, int* timeout);
    SCOPEFUN_API int sfGetFrameVersion(SFContext* ctx, int* version);
    SCOPEFUN_API int sfGetFrameHeader(SFContext* ctx, int* header);
    SCOPEFUN_API int sfGetFrameData(SFContext* ctx, int* data);
    SCOPEFUN_API int sfGetFramePacket(SFContext* ctx, int* packet);
    SCOPEFUN_API int sfGetClientId(SFContext* ctx, int* id);
    SCOPEFUN_API int sfGetClientActiveIds(SFContext* ctx, SActiveClients* id);
    SCOPEFUN_API int sfGetClientDisplay(SFContext* ctx, SDisplay* display);
    SCOPEFUN_API int sfGetSimulateData(SFContext* ctx, SSimulate* sim);

    /*----------------------------------------
    client
    ----------------------------------------*/
    SCOPEFUN_API int sfClientConnect(SFContext* ctx, const char* ip, int port);
    SCOPEFUN_API int sfClientDisconnect(SFContext* ctx);
    SCOPEFUN_API int sfClientDisplay(SFContext* ctx, int clientId, SDisplay* display);

    /*----------------------------------------
    server
    ----------------------------------------*/
    SCOPEFUN_API int sfServerDownload(SFContext* ctx);
    SCOPEFUN_API int sfServerUpload(SFContext* ctx);

    /*----------------------------------------
    hardware
    ----------------------------------------*/
    SCOPEFUN_API int sfHardwareOpen(SFContext* ctx, SUsb* usb, int version);
    SCOPEFUN_API int sfHardwareReset(SFContext* ctx);
    SCOPEFUN_API int sfHardwareIsOpened(SFContext* ctx, int* open);
    SCOPEFUN_API int sfHardwareConfig1(SFContext* ctx, SHardware1* hw);
    SCOPEFUN_API int sfHardwareConfig2(SFContext* ctx, SHardware2* hw);
    SCOPEFUN_API int sfHardwareCapture(SFContext* ctx, SFrameData* buffer, int len, int* received, int type);
    SCOPEFUN_API int sfHardwareCaptureOff(SFContext* ctx);
    SCOPEFUN_API int sfHardwareUploadFx2(SFContext* ctx, SFx2* fx2);
    SCOPEFUN_API int sfHardwareUploadFpga(SFContext* ctx, SFpga* fpgs);
    SCOPEFUN_API int sfHardwareUploadGenerator(SFContext* ctx, SGenerator* gen);
    SCOPEFUN_API int sfHardwareEepromRead(SFContext* ctx, SEeprom* eeprom, int size, int adrees);
    SCOPEFUN_API int sfHardwareEepromReadFirmwareID(SFContext* ctx, SEeprom* eeprom, int size, int adrees);
    SCOPEFUN_API int sfHardwareEepromWrite(SFContext* ctx, SEeprom* eeprom, int size, int adress);
    SCOPEFUN_API int sfHardwareEepromErase(SFContext* ctx);
    SCOPEFUN_API int sfHardwareClose(SFContext* ctx);

    /*----------------------------------------
    simulate
    ----------------------------------------*/
    SCOPEFUN_API int sfSimulate(SFContext* ctx, double time);
#endif

/*----------------------------------------
   helpers
----------------------------------------*/
const char* messageName(EMessage message);
int clientMessageHeader(messageHeader* dest, EMessage message);
int serverMessageHeader(messageHeader* dest, EMessage message);
int isClientHeaderOk(messageHeader* header);
int isServerHeaderOk(messageHeader* header);

#endif
/*----------------------------------------
  http://www.ScopeFun.com
----------------------------------------*/
