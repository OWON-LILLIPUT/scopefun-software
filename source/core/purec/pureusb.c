////////////////////////////////////////////////////////////////////////////////
//    ScopeFun Oscilloscope ( http://www.scopefun.com )
//    Copyright (C) 2016 David Košenina
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
#include<core/purec/purec.h>
#include<core/purec/pureusb.h>
#include<libusb.h>
#include<ezusb.h>
#include<stdio.h>
#include<stdarg.h>

////////////////////////////////////////////////////////////////////////////////
//
// log
//
////////////////////////////////////////////////////////////////////////////////
void logerror(const char* format, ...)
{
    char str[2049] = { 0 };
    va_list argptr;
    va_start(argptr, format);
    vsprintf(str, format, argptr);
    va_end(argptr);
    printf(str);
}

////////////////////////////////////////////////////////////////////////////////
//
// usbRegisterCallback
//
////////////////////////////////////////////////////////////////////////////////
int usbRegisterCallback(UsbContext* ctx, usbCallback callback)
{
    ctx->callback = callback;
    return PUREUSB_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
//
//  hotplug
//
////////////////////////////////////////////////////////////////////////////////
int LIBUSB_CALL hotplug_callback(libusb_context* ctx, libusb_device* device, libusb_hotplug_event event, void* user_data)
{
    return LIBUSB_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
//
// FX3
//
////////////////////////////////////////////////////////////////////////////////

int usbFx3CheckFirmwareID(UsbContext* ctx, char* data)
{
    if(usbFxxIsConnected(ctx))
    {
        // bReqType: 0xC0, bRequest : 0xB0, wLength : 0x10
        int length = 0x10;
        int ret = libusb_control_transfer((libusb_device_handle*)ctx->device,
                                          0xC0,
                                          0xB0,
                                          0,
                                          0,
                                          (unsigned char*)data,
                                          length,
                                          1000);
        if(ret == length)
        {
            return PUREUSB_SUCCESS;
        }
    }
    return PUREUSB_FAILURE;
}

unsigned short endianSwapLoHi(unsigned short value)
{
    unsigned short low  = (value & 0x00FF);
    unsigned short high = (value & 0xFF00);
    return (low << 8) | (high >> 8);
}

int writeData(UsbContext* ctx, int address, char* buffer, int len)
{
    unsigned short hi = endianSwapLoHi((address >> 16) & 0xffff);
    unsigned short lo = endianSwapLoHi(address & 0xffff);
    int ret = libusb_control_transfer((libusb_device_handle*)ctx->device,
                                      0x40,
                                      0xA0,
                                      lo,
                                      hi,
                                      buffer,
                                      len,
                                      10000);
    char verify[4096] = {0};
    ret = libusb_control_transfer((libusb_device_handle*)ctx->device,
                                  0xC0,
                                  0xA0,
                                  lo,
                                  hi,
                                  verify,
                                  len,
                                  10000);
    ret = memcmp((void*)buffer, (void*)verify, len);
    if(ret != 0)
    {
        int debug = 1;
    }
}

int usbFx3UploadFirmwareToFx3(UsbContext* ctx, const char* path)
{
    if(usbFxxIsConnected(ctx))
    {
        int res = 0;
        res = ezusb_load_ram((libusb_device_handle*)ctx->device, path, FX_TYPE_FX3, IMG_TYPE_IMG, 0);
    }
    return 0;
}
int usbFx3UploadFirmwareToFpga(UsbContext* ctx, unsigned char* buffer, int size, int swapBits)
{
    int data[4] = { 0 };
    if(usbFxxIsConnected(ctx))
    {
        data[0] = 0x7c320500;
        int length = 0x10;
        // bReqType: 0x40, bRequest : 0xB2, wLength : 0x10
        int ret = libusb_control_transfer((libusb_device_handle*)ctx->device,
                                          0x40,
                                          0xB2,
                                          0,
                                          0,
                                          (unsigned char*)data,
                                          length,
                                          1000);
        if(ret != length)
        {
            return PUREUSB_FAILURE;
        }
        // upload
        if(swapBits)
        {
            for(int i = 0; i < size; i++)
            {
                unsigned char temp = buffer[i];
                unsigned char bit0 = temp & 0x1;
                unsigned char bit1 = (temp >> 1) & 0x1;
                unsigned char bit2 = (temp >> 2) & 0x1;
                unsigned char bit3 = (temp >> 3) & 0x1;
                unsigned char bit4 = (temp >> 4) & 0x1;
                unsigned char bit5 = (temp >> 5) & 0x1;
                unsigned char bit6 = (temp >> 6) & 0x1;
                unsigned char bit7 = (temp >> 7) & 0x1;
                buffer[i] = (bit0 << 7) | (bit1 << 6) | (bit2 << 5) | (bit3 << 4) | (bit4 << 3) | (bit5 << 2) | (bit6 << 1) | (bit7 << 0);
            }
        }
        int transfered = 0;
        ret = libusb_bulk_transfer((libusb_device_handle*)ctx->device, 2 | LIBUSB_ENDPOINT_OUT, buffer, size, &transfered, 30000);
        if(ret > 0)
        {
            return PUREUSB_FAILURE;
        }
        // wait for the upload to complete ... todo: check
        cSleep(1000);
        // bReqType: 0xC0, bRequest : 0xB1, wLength : 0x1
        data[0] = 0;
        length  = 1;
        ret = libusb_control_transfer((libusb_device_handle*)ctx->device,
                                      0xC0,
                                      0xB1,
                                      0,
                                      0,
                                      (unsigned char*)data,
                                      length,
                                      10000);
        if(ret != length)
        {
            return PUREUSB_FAILURE;
        }
        return PUREUSB_SUCCESS;
    }
    return PUREUSB_FAILURE;
}

int usbFx3ReadEEPROMFirmwareID(UsbContext* ctx, unsigned char* buffer, int size, int readadress)
{
   int transfered = 0;
   if (usbFxxIsConnected(ctx))
   {
      // bReqType: 0xC0, bRequest : 0xB0, wLength - MAX : 0x1000
      int read = libusb_control_transfer((libusb_device_handle*)ctx->device,
            0xC0,
            0xB0,
            (readadress >> 16) & 0xffff,
            (readadress) & 0xffff,
            buffer,
            size,
            100000);
      if (read == size)
      {
         return PUREUSB_SUCCESS;
      }
   }
   return PUREUSB_FAILURE;
}
int usbFx3ReadEEPROM(UsbContext* ctx, unsigned char* buffer, int size, int readadress)
{
    int transfered = 0;
    if(usbFxxIsConnected(ctx))
    {
        // bReqType: 0xC0, bRequest : 0xBB, wLength - MAX : 0x1000
        int packet = 1024;
        int count  = size / packet;
        int left   = size % packet;
        int read   = 0;
        for (int i = 0; i < count; i++)
        {
           read += libusb_control_transfer((libusb_device_handle*)ctx->device,
              0xC0,
              0xBB,
              (readadress >> 16) & 0xffff,
              (readadress) & 0xffff,
              buffer + i * packet,
              packet,
              100000);
           readadress += packet;
        }
        read += libusb_control_transfer((libusb_device_handle*)ctx->device,
           0xC0,
           0xBB,
           (readadress >> 16) & 0xffff,
           (readadress) & 0xffff,
           buffer + count * packet,
           left,
           100000);
        if(read == size)
        {
            return PUREUSB_SUCCESS;
        }
    }
    return PUREUSB_FAILURE;
}

int usbFx3Reset(UsbContext* ctx)
{
    if(usbFxxIsConnected(ctx))
    {
        int ret = libusb_control_transfer((libusb_device_handle*)ctx->device,
                                          0x40,
                                          0xEE,
                                          0,
                                          0,
                                          0,
                                          0,
                                          10000);
        return PUREUSB_SUCCESS;
    }
    return PUREUSB_FAILURE;
}

int writeDataEeprom(UsbContext* ctx, int address, char* buffer, int len)
{
    unsigned short hi = (address >> 16) & 0xffff;
    unsigned short lo = address & 0xffff;
    int ret = libusb_control_transfer((libusb_device_handle*)ctx->device,
                                      LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT | LIBUSB_RECIPIENT_DEVICE,
                                      0xBA,
                                      hi,
                                      lo,
                                      buffer,
                                      len,
                                      10000);
    return ret == len ? 0 : 1;
}
int usbFx3WriteEEPROM(UsbContext* ctx, unsigned char* buffer, int size, int writeadress)
{
    if(usbFxxIsConnected(ctx))
    {
        // write eeprom
        int chunk = 1024;
        int loop = size / chunk;
        int left = size % chunk;
        int ret  = 0;
        for(int i = 0; i < loop; i++)
        {
            ret += writeDataEeprom(ctx,
                                      writeadress + (chunk * i),
                                      buffer + (chunk * i),
                                      chunk);
        }
        ret += writeDataEeprom(ctx,
                               writeadress + (chunk * loop),
                               buffer + (chunk * loop),
                               left);
        if(ret == 0)
        {
            return PUREUSB_SUCCESS;
        }
    }
    return PUREUSB_FAILURE;
}

int usbFx3WriteLockableEEPROM(UsbContext* ctx, unsigned char* buffer, int size, int writeadress)
{
   if (usbFxxIsConnected(ctx))
   {
      unsigned short hi = 8;
      unsigned short lo = writeadress & 0x00ff;
      int ret = libusb_control_transfer((libusb_device_handle*)ctx->device,
         LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT | LIBUSB_RECIPIENT_DEVICE,
         0xBA,
         hi,
         lo,
         buffer,
         size,
         10000);
      return ret == size ? 0 : 1;
      if (ret == 0)
      {
         return PUREUSB_SUCCESS;
      }
   }
   return PUREUSB_FAILURE;
}

int usbFx3ReadLockableEEPROM(UsbContext* ctx, unsigned char* buffer, int size, int readadress)
{
   int transfered = 0;
   if (usbFxxIsConnected(ctx))
   {
      unsigned short hi = 8;
      unsigned short lo = readadress & 0x00ff;
      int read = libusb_control_transfer((libusb_device_handle*)ctx->device,
         0xC0,
         0xBB,
         hi,
         lo,
         buffer,
         size,
         100000);
      if (read == size)
      {
         return PUREUSB_SUCCESS;
      }
   }
   return PUREUSB_FAILURE;
}

int usbFx3LockLockableEEPROM(UsbContext* ctx)
{
   int transfered = 0;
   if (usbFxxIsConnected(ctx))
   {
      unsigned short hi = 8;
      unsigned short lo = 0x400;
      char byte = 0x2;
      int read = libusb_control_transfer((libusb_device_handle*)ctx->device,
         LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT | LIBUSB_RECIPIENT_DEVICE,
         0xBA,
         hi,
         lo,
         &byte,
         1,
         100000);
      if (read == 1)
      {
         return PUREUSB_SUCCESS;
      }
   }
   return PUREUSB_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////
//
// FX2
//
////////////////////////////////////////////////////////////////////////////////
int usbFx2UploadFirmwareToFx2(UsbContext* ctx, const char* path)
{
    if(usbFxxIsConnected(ctx))
    {
        int res = 0;
        res = ezusb_load_ram((libusb_device_handle*)ctx->device, path, FX_TYPE_FX2LP, IMG_TYPE_HEX, 0);
        return PUREUSB_SUCCESS;
    }
    return PUREUSB_FAILURE;
}

int usbFx2UploadFirmwareToFpga(UsbContext* ctx, unsigned char* buffer, int size, int swapBits)
{
    if(usbFxxIsConnected(ctx))
    {
        unsigned char data   = 0;
        int           length = 0;
        int ret = 0;
        // go to reset mode
        ret = libusb_control_transfer((libusb_device_handle*)ctx->device,
                                      LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT | LIBUSB_RECIPIENT_DEVICE,
                                      0xB2,
                                      0,
                                      0,
                                      &data,
                                      length,
                                      1000);
        if(ret != length)
        {
            return PUREUSB_FAILURE;
        }
        // upload
        if(swapBits)
        {
            for(int i = 0; i < size; i++)
            {
                unsigned char temp = buffer[i];
                unsigned char bit0 = temp & 0x1;
                unsigned char bit1 = (temp >> 1) & 0x1;
                unsigned char bit2 = (temp >> 2) & 0x1;
                unsigned char bit3 = (temp >> 3) & 0x1;
                unsigned char bit4 = (temp >> 4) & 0x1;
                unsigned char bit5 = (temp >> 5) & 0x1;
                unsigned char bit6 = (temp >> 6) & 0x1;
                unsigned char bit7 = (temp >> 7) & 0x1;
                buffer[i] = (bit0 << 7) | (bit1 << 6) | (bit2 << 5) | (bit3 << 4) | (bit4 << 3) | (bit5 << 2) | (bit6 << 1) | (bit7 << 0);
            }
        }
        int transfered = 0;
        ret = libusb_bulk_transfer((libusb_device_handle*)ctx->device, 2 | LIBUSB_ENDPOINT_OUT, buffer, size, &transfered, 1000);
        if(ret > 0)
        {
            return PUREUSB_FAILURE;
        }
        // out of reset mode
        ret = libusb_control_transfer((libusb_device_handle*)ctx->device,
                                      LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT | LIBUSB_RECIPIENT_DEVICE,
                                      0xB3,
                                      0,
                                      0,
                                      &data,
                                      length,
                                      10000);
        if(ret != length)
        {
            return PUREUSB_FAILURE;
        }
        return PUREUSB_SUCCESS;
    }
    return PUREUSB_FAILURE;
}

int usbFx2ReadEEPROM(UsbContext* ctx, unsigned char* buffer, int size, int readaddress)
{
    int transfered = 0;
    if(usbFxxIsConnected(ctx))
    {
        // read eeprom
        int ret = libusb_control_transfer((libusb_device_handle*)ctx->device,
                                          LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN | LIBUSB_RECIPIENT_DEVICE,
                                          0xB4,
                                          (readaddress >> 16) & 0xffff,
                                          (readaddress) & 0xffff,
                                          buffer,
                                          size,
                                          100000);
        if(ret == size)
        {
            return PUREUSB_SUCCESS;
        }
    }
    return PUREUSB_FAILURE;
}

int usbFx2WriteEEPROM(UsbContext* ctx, unsigned char* buffer, int size, int writeaddress)
{
    if(usbFxxIsConnected(ctx))
    {
        // write eeprom
        int ret = libusb_control_transfer((libusb_device_handle*)ctx->device,
                                          LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT | LIBUSB_RECIPIENT_DEVICE,
                                          0xB4,
                                          (writeaddress >> 16) & 0xffff,
                                          (writeaddress) & 0xffff,
                                          buffer,
                                          size,
                                          10000);
        if(ret == size)
        {
            return PUREUSB_SUCCESS;
        }
    }
    return PUREUSB_FAILURE;
}
int usbFx2Reset(UsbContext* ctx)
{
    if(usbFxxIsConnected(ctx))
    {
        int ret = libusb_control_transfer((libusb_device_handle*)ctx->device,
                                          0x40,
                                          0xEE,
                                          0,
                                          0,
                                          0,
                                          0,
                                          10000);
        return PUREUSB_SUCCESS;
    }
    return PUREUSB_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////
//
// Fxx
//
////////////////////////////////////////////////////////////////////////////////

int usbFxxIsConnected(UsbContext* ctx)
{
    return ctx->device > 0 ? 1 : 0;
}

void usbFxxInit(UsbContext* ctx)
{
    libusb_init(0);
}

int usbFxxTransferDataIn(UsbContext* ctx, int endPoint, char* dest, int size, int swapBytes, int timeout, int* transfered)
{
    int ret = 0;
    if(usbFxxIsConnected(ctx))
    {
        *transfered = 0;
        #if defined(MAC)
        int ret = 0;
        int chunkBytes     = 64 * 1024;
        int iterations     = size / chunkBytes;
        int wholePartBytes = chunkBytes * iterations;
        int leftoverBytes  = size % chunkBytes;
        int t = 0;
        for(int i = 0; i < iterations; i++)
        {
            ret += libusb_bulk_transfer((libusb_device_handle*)ctx->device, endPoint | LIBUSB_ENDPOINT_IN, dest, chunkBytes, &t, timeout);
            dest += t;
            *transfered += t;
        }
        if(leftoverBytes > 0 || ((*transfered) < size))
        {
            ret += libusb_bulk_transfer((libusb_device_handle*)ctx->device, endPoint | LIBUSB_ENDPOINT_IN, dest, leftoverBytes + (wholePartBytes - (*transfered)), &t, timeout);
            *transfered += t;
        }
        if(ret > 0)
        {
            return PUREUSB_FAILURE;
        }
        #else
        ret += libusb_bulk_transfer((libusb_device_handle*)ctx->device, endPoint | LIBUSB_ENDPOINT_IN, dest, size, transfered, timeout);
        #endif
        if(ret > 0)
        {
            return PUREUSB_FAILURE;
        }
        if(swapBytes)
        {
            int                count = size / 4;
            unsigned int* swapBuffer = (unsigned int*)dest;
            for(int i = 0; i < count; i++)
            {
                swapBuffer[i] = cSwap32(&swapBuffer[i]);
            }
        }
        return PUREUSB_SUCCESS;
    }
    return PUREUSB_FAILURE;
}

int usbFxxTransferDataOut(UsbContext* ctx, int endPoint, char* src, int size, int swapBytes, int timeOut, int* transfered)
{
    if(usbFxxIsConnected(ctx))
    {
        *transfered = 0;
        if(swapBytes)
        {
            int count = size / 2;
            unsigned short* swapBuffer = (unsigned short*)src;
            for(int i = 0; i < count; i++)
            {
                swapBuffer[i] = cSwap16(&swapBuffer[i]);
            }
        }
        int ret = libusb_bulk_transfer((libusb_device_handle*)ctx->device, endPoint | LIBUSB_ENDPOINT_OUT, src, size, transfered, timeOut);
        if(ret > 0)
        {
            return PUREUSB_FAILURE;
        }
        return PUREUSB_SUCCESS;
    }
    return PUREUSB_FAILURE;
}

void usbFxxGuidVidPid(UsbContext* ctx, struct UsbGuid guid, int vendorId, int productId,char serialId)
{
    ctx->guid      = guid;
    ctx->vendorId  = vendorId;
    ctx->productId = productId;
    ctx->serialId  = serialId;
}

int usbFxxFindList(UsbContext* ctx, usbDevice** foundList, int maxCount)
{
    if(ctx->device)
    {
        return PUREUSB_FAILURE;
    }
    if(ctx->deviceList)
    {
        return PUREUSB_FAILURE;
    }
    ctx->deviceListCnt = (int)libusb_get_device_list(0, (libusb_device***)&ctx->deviceList);
    if(ctx->deviceListCnt < 0)
    {
        return PUREUSB_FAILURE;
    }
    int index = 0;
    for(unsigned int i = 0; i < ctx->deviceListCnt; i++)
    {
        libusb_device* device = (libusb_device*)ctx->deviceList[i];
        if(device)
        {
            struct libusb_device_descriptor desc;
            libusb_get_device_descriptor(device, &desc);
            if(desc.idVendor == ctx->vendorId && desc.idProduct == ctx->productId)
            {
                ctx->serialId = desc.iSerialNumber;
                foundList[index] = (usbDevice*)device;
                index++;
                if(index < 0)
                {
                    index = 0;
                }
                if(index > maxCount - 1)
                {
                    index = maxCount - 1;
                }
            }
        }
    }
    if(index == 0)
    {
        return PUREUSB_FAILURE;
    }
    return PUREUSB_SUCCESS;
}

int usbFxxFreeList(UsbContext* ctx)
{
    libusb_free_device_list((libusb_device**)ctx->deviceList, ctx->deviceListCnt);
    ctx->deviceList    = 0;
    ctx->deviceListCnt = 0;
    return PUREUSB_SUCCESS;
}

int usbFxxOpenSerial(UsbContext* ctx, char* path, char checkSerial, usbDevice** foundList, int maxCount)
{
    for(int i = 0; i < maxCount; i++)
    {
        libusb_device* device = (libusb_device*)foundList[i];
        if(device)
        {
            if(checkSerial > 0)
            {
                usbFxxOpen(ctx, (usbDevice*)device);
                usbFx2UploadFirmwareToFx2(ctx, path);
                usbFxxClose(ctx);
                // 3s, delay
                cSleep(3000);
                usbFxxOpen(ctx, (usbDevice*)device);
                struct UsbEEPROM data;
                usbFx2ReadEEPROM(ctx, data.bytes, EEPROM_BYTE_COUNT, 0);
                if(data.bytes[5] == ctx->serialId)
                {
                    return PUREUSB_SUCCESS;
                }
                else
                {
                    usbFxxClose(ctx);
                }
            }
            else
            {
                usbFxxOpen(ctx, (usbDevice*)device);
                usbFx2UploadFirmwareToFx2(ctx, path);
                usbFxxClose(ctx);
                // 3s, delay
                cSleep(3000);
                usbFxxOpen(ctx, (usbDevice*)device);
                return PUREUSB_SUCCESS;
            }
        }
    }
    return PUREUSB_FAILURE;
}


int usbFxxOpenNormal(UsbContext* ctx, usbDevice** foundList, int maxCount)
{
    for(int i = 0; i < maxCount; i++)
    {
        libusb_device* device = (libusb_device*)foundList[i];
        if(device)
        {
            int ret = usbFxxOpen(ctx, (usbDevice*)device);
            ctx->serialBufferSize = libusb_get_string_descriptor_ascii(ctx->device, ctx->serialId, ctx->serialBuffer, 1024);
            return ret;
        }
    }
    return PUREUSB_FAILURE;
}


int usbFxxOpen(UsbContext* ctx, usbDevice* device)
{
    if(ctx->device)
    {
        return PUREUSB_FAILURE;
    }
    int err = libusb_open((libusb_device*)device, (libusb_device_handle**)&ctx->device);
    if(err)
    {
        return PUREUSB_FAILURE;
    }
    int speed = libusb_get_device_speed((libusb_device*)device);
    // interface
    usbFxxClaimInterface(ctx, 0);

    return PUREUSB_SUCCESS;
}

void usbFxxClose(UsbContext* ctx)
{
    if(!ctx->device)
    {
        return;
    }
    // release
    usbFxxReleaseInterface(ctx, 0);
    // close
    libusb_close((libusb_device_handle*)ctx->device);
    // null
    ctx->device = 0;
}

void usbFxxClaimInterface(UsbContext* ctx, int interface)
{
    if(!ctx->device)
    {
        return;
    }
    libusb_claim_interface((libusb_device_handle*)ctx->device, interface);
    libusb_clear_halt((libusb_device_handle*)ctx->device, interface);
}

void usbFxxReleaseInterface(UsbContext* ctx, int interface)
{
    if(!ctx->device)
    {
        return;
    }
    libusb_release_interface((libusb_device_handle*)ctx->device, interface);
}

void usbFxxReset(UsbContext* ctx)
{
    if(!ctx->device)
    {
        return;
    }
    libusb_reset_device((libusb_device_handle*)ctx->device);
}

void usbFxxUpdate(UsbContext* ctx)
{
}

void usbFxxExit(UsbContext* ctx)
{
    libusb_exit(0);
}


////////////////////////////////////////////////////////////////////////////////
//
// test
//
////////////////////////////////////////////////////////////////////////////////

int testUsbTransferDataIn(UsbContext* ctx, int endPoint, char* dest, int size, int swapBytes, int timeOut, int* transfered)
{
    return PUREUSB_SUCCESS;
}

int testUsbTransferDataOut(UsbContext* ctx, int endPoint, char* src, int size, int swapBytes, int timeOut)
{
    return PUREUSB_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
