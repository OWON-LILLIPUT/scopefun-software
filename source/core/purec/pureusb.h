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
#ifndef __PUREC__USB__
#define __PUREC__USB__

#define PUREUSB_SUCCESS  0
#define PUREUSB_FAILURE -1

////////////////////////////////////////////////////////////////////////////////
// context
////////////////////////////////////////////////////////////////////////////////
struct usbCtx;
typedef struct usbCtx usbCtx;

////////////////////////////////////////////////////////////////////////////////
// device
////////////////////////////////////////////////////////////////////////////////
struct usbDevice;
typedef struct usbDevice       usbDevice;

////////////////////////////////////////////////////////////////////////////////
// handle
////////////////////////////////////////////////////////////////////////////////
struct usbDeviceHandle;
typedef struct usbDeviceHandle usbDeviceHandle;

////////////////////////////////////////////////////////////////////////////////
// UsbEEPROM
////////////////////////////////////////////////////////////////////////////////
#define EEPROM_BYTE_COUNT 16
struct UsbEEPROM
{
    unsigned char  bytes[EEPROM_BYTE_COUNT];
};
typedef struct UsbEEPROM UsbEEPROM;

////////////////////////////////////////////////////////////////////////////////
// UsbGuid
////////////////////////////////////////////////////////////////////////////////
struct UsbGuid
{
    unsigned int    data1;
    unsigned short  data2;
    unsigned short  data3;
    unsigned char   data4[8];
};
typedef struct UsbGuid UsbGuid;

////////////////////////////////////////////////////////////////////////////////
// callback
////////////////////////////////////////////////////////////////////////////////
#define USB_CALLBACK_DEVICE_ARIVED 0x1
#define USB_CALLBACK_DEVICE_LEFT   0x2
typedef int(*usbCallback)(char* ctx, int flag);

////////////////////////////////////////////////////////////////////////////////
// UsbContext
////////////////////////////////////////////////////////////////////////////////
struct UsbContext
{
    usbCtx*               ctx;
    unsigned int          version;
    UsbGuid               guid;
    unsigned int          deviceListCnt;
    usbDevice**           deviceList;
    usbDeviceHandle*      device;
    int                   vendorId;
    int                   productId;
    unsigned int          serialId;
    char                  serialBuffer[1024];
    int                   serialBufferSize;
    usbCallback           callback;
};
typedef struct UsbContext UsbContext;

////////////////////////////////////////////////////////////////////////////////
// FX3
////////////////////////////////////////////////////////////////////////////////
int  usbFx3CheckFirmwareID(UsbContext* ctx, char* data);
int  usbFx3UploadFirmwareToFx3(UsbContext* ctx, const char* path);
int  usbFx3UploadFirmwareToFpga(UsbContext* ctx, unsigned char* buffer, int size, int swapBits);
int  usbFx3ReadEEPROM(UsbContext* ctx, unsigned char* buffer, int size, int adress);
int  usbFx3ReadEEPROMFirmwareID(UsbContext* ctx, unsigned char* buffer, int size, int adress);
int  usbFx3WriteEEPROM(UsbContext* ctx, unsigned char* buffer, int size, int adress);
int  usbFx3WriteLockableEEPROM(UsbContext* ctx, unsigned char* buffer, int size, int adress);
int  usbFx3ReadLockableEEPROM(UsbContext* ctx, unsigned char* buffer, int size, int adress);
int  usbFx3LockLockableEEPROM(UsbContext* ctx);
int  usbFx3Reset(UsbContext* ctx);

////////////////////////////////////////////////////////////////////////////////
// FX2
////////////////////////////////////////////////////////////////////////////////
int  usbFx2UploadFirmwareToFx2(UsbContext* ctx, const char* path);
int  usbFx2UploadFirmwareToFpga(UsbContext* ctx, unsigned char* buffer, int size, int swapBits);
int  usbFx2ReadEEPROM(UsbContext* ctx, unsigned char* buffer, int size, int readaddress);
int  usbFx2WriteEEPROM(UsbContext* ctx, unsigned char* buffer, int size, int writeaddress);
int  usbFx2Reset(UsbContext* ctx);

////////////////////////////////////////////////////////////////////////////////
// Common
////////////////////////////////////////////////////////////////////////////////
void usbFxxInit(UsbContext* ctx);
int  usbFxxIsConnected(UsbContext* ctx);
int  usbFxxRegisterCallback(UsbContext* ctx, usbCallback callback);
int  usbFxxTransferDataIn(UsbContext* ctx, int endPoint, char* dest, int size, int swapBytes, int timeOut, int* transfered);
int  usbFxxTransferDataOut(UsbContext* ctx, int endPoint, char* src, int size, int swapBytes, int timeOut, int* transfered);
int  usbFxxIsConnected(UsbContext* ctx);
void usbFxxGuidVidPid(UsbContext* ctx, UsbGuid guid, int vendorId, int productId, char serialId);
int  usbFxxFindList(UsbContext* ctx, usbDevice** foundList, int maxCount);
int  usbFxxFreeList(UsbContext* ctx);
int  usbFxxOpenSerial(UsbContext* ctx, char* path, char checkSerial, usbDevice** foundList, int maxCount);
int  usbFxxOpenNormal(UsbContext* ctx, usbDevice** foundList, int maxCount);
int  usbFxxOpen(UsbContext* ctx, usbDevice* device);
void usbFxxClose(UsbContext* ctx);
void usbFxxClaimInterface(UsbContext* ctx, int i);
void usbFxxReleaseInterface(UsbContext* ctx, int i);
void usbFxxUpdate(UsbContext* ctx);
void usbFxxExit(UsbContext* ctx);

////////////////////////////////////////////////////////////////////////////////
// test
////////////////////////////////////////////////////////////////////////////////
int  testUsbTransferDataIn(UsbContext* ctx, int endPoint,  char* dest, int size, int swapBytes, int timeOut, int* transfered);
int  testUsbTransferDataOut(UsbContext* ctx, int endPoint, char* src, int size, int swapBytes, int timeOut);

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
