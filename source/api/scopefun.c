////////////////////////////////////////////////////////////////////////////////
//    ScopeFun Oscilloscope ( http://www.scopefun.com )
//    Copyright (C) 2016-2019 David Košenina
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
#include<core/purec/puresocket.h>
#include<core/purec/pureusb.h>
#include<core/purec/purec.h>
#include<api/scopefunapi.h>
#include<SDL.h>

#include<stdlib.h>
#include<stdio.h>
#include<stdarg.h>
#include<string.h>
#include<math.h>
#include<errno.h>
#include<locale.h>

/*--------------------------------------------------------------------
   api helper functions
---------------------------------------------------------------------*/
uint apiMin(uint a, uint b)
{
    if(a < b)
    {
        return a;
    }
    else
    {
        return b;
    }
}

void apiLock(SFContext* ctx)
{
    if(SDL_AtomicGet((SDL_atomic_t*)&ctx->api.thread) > 0)
    {
        SDL_AtomicLock(&ctx->api.lock);
    }
    else
    {
        int debug = 1;
    }
}

void apiUnlock(SFContext* ctx)
{
    if(SDL_AtomicGet((SDL_atomic_t*)&ctx->api.thread) > 0)
    {
        SDL_AtomicUnlock(&ctx->api.lock);
    }
    else
    {
        int debug = 1;
    }
}

uint apiFrameHeader(SFContext* ctx)
{
    if(ctx->frame.info.version == 1)
    {
        return SCOPEFUN_FRAME_1_HEADER;
    }
    if(ctx->frame.info.version == 2)
    {
        return SCOPEFUN_FRAME_2_HEADER;
    }
    return 0;
}

uint apiFrameSize(int version, char* header)
{
    if(version == 1)
    {
        return SCOPEFUN_FRAME_1_HEADER + SCOPEFUN_FRAME_1_DATA;
    }
    if(version == 2)
    {
        SFrameHeader2* header2 = (SFrameHeader2*)header;
        uint sampleSize = ((uint)header2->hardware.bytes[32 + 0] << 24) | ((uint)header2->hardware.bytes[32 + 1] << 16) | ((uint)header2->hardware.bytes[32 + 2] << 8) | ((uint)header2->hardware.bytes[32 + 3] << 0);
        uint frameSize = sampleSize * 4;
        if(frameSize % 1024 != 0)
        {
            frameSize = ((frameSize / 1024) + 1) * 1024;
        }
        frameSize += SCOPEFUN_FRAME_2_HEADER;
        return frameSize;
    }
    return 0;
}

int apiResult(int ret)
{
    if(ret == 0)
    {
        return SCOPEFUN_SUCCESS;
    }
    return SCOPEFUN_FAILURE;
}

/*--------------------------------------------------------------------
   macros
---------------------------------------------------------------------*/
#define SCOPEFUN_CREATE_DELETE(name) \
    name* sfCreate##name() { name* ptr = malloc(sizeof(name)); SDL_zerop(ptr); return ptr; } \
    void  sfDelete##name(name* ptr) { free(ptr); }

/*--------------------------------------------------------------------
   API structures
---------------------------------------------------------------------*/
SCOPEFUN_CREATE_DELETE(SFContext)
SCOPEFUN_CREATE_DELETE(SDisplay)
SCOPEFUN_CREATE_DELETE(SSimulate)
SCOPEFUN_CREATE_DELETE(SFrameInfo)
SCOPEFUN_CREATE_DELETE(SUsb)
SCOPEFUN_CREATE_DELETE(SHardware1)
SCOPEFUN_CREATE_DELETE(SHardware2)
SCOPEFUN_CREATE_DELETE(SFx2)
SCOPEFUN_CREATE_DELETE(SFpga)
SCOPEFUN_CREATE_DELETE(SGenerator)
SCOPEFUN_CREATE_DELETE(SEeprom)
SCOPEFUN_CREATE_DELETE(SActiveClients)
SFrameData* sfCreateSFrameData(SFContext* ctx, int memory)
{
    apiLock(ctx);
    memory  = apiMin(memory, SCOPEFUN_FRAME_MEMORY);
    SFrameData* ptr = (SFrameData*)malloc(memory);
    SDL_memset(ptr, 0, memory);
    apiUnlock(ctx);
    return ptr;
}
void sfDeleteSFrameData(SFrameData* ptr)
{
    free(ptr);
}

/*--------------------------------------------------------------------
   helper
---------------------------------------------------------------------*/
const char* msgName[(mLast + 1)] =
{
    "mClientConnect",
    "mClientDisconnect",
    "mClientDisplay",
    "mUpload",
    "mDownload",
    "mHardwareOpen",
    "mHardwareIsOpened",
    "mHardwareReset",
    "mHardwareConfig1",
    "mHardwareConfig2",
    "mHardwareCapture",
    "mHardwareCaptureFrame",
    "mHardwareUploadFx2",
    "mHardwareUploadFpga",
    "mHardwareUploadGenerator",
    "mHardwareEepromRead",
    "mHardwareEepromWrite",
    "mHardwareEepromErase",
    "mHardwareClose",
    "mLast",
};
const char* messageName(EMessage message)
{
    return msgName[message];
}
int clientMessageHeader(messageHeader* dest, EMessage message)
{
    dest->magic[0] = 'S';
    dest->magic[1] = 'c';
    dest->magic[2] = 'o';
    dest->magic[3] = 'p';
    dest->magic[4] = 'e';
    dest->magic[5] = 'F';
    dest->magic[6] = 'u';
    dest->magic[7] = 'n';
    dest->message = (uint)message;
    dest->error   = SCOPEFUN_SUCCESS;
    switch(message)
    {
        case mClientConnect:
            dest->size = sizeof(csClientConnect);
            break;
        case mClientDisconnect:
            dest->size = sizeof(csClientDisconnect);
            break;
        case mClientDisplay:
            dest->size = sizeof(csClientDisplay);
            break;
        case mUpload:
            dest->size = sizeof(csUpload);
            break;
        case mDownload:
            dest->size = sizeof(csDownload);
            break;
        case mHardwareOpen:
            dest->size = sizeof(csHardwareOpen);
            break;
        case mHardwareIsOpened:
            dest->size = sizeof(csHardwareIsOpened);
            break;
        case mHardwareReset:
            dest->size = sizeof(csHardwareReset);
            break;
        case mHardwareConfig1:
            dest->size = sizeof(csHardwareConfig1);
            break;
        case mHardwareConfig2:
            dest->size = sizeof(csHardwareConfig2);
            break;
        case mHardwareCapture:
            dest->size = sizeof(csHardwareCapture);
            break;
        case mHardwareUploadFx2:
            dest->size = sizeof(csHardwareUploadFx2);
            break;
        case mHardwareUploadFpga:
            dest->size = sizeof(csHardwareUploadFpga);
            break;
        case mHardwareUploadGenerator:
            dest->size = sizeof(csHardwareUploadGenerator);
            break;
        case mHardwareEepromRead:
            dest->size = sizeof(csHardwareEepromRead);
            break;
        case mHardwareEepromReadFirmwareID:
           dest->size = sizeof(csHardwareEepromReadFirmwareID);
           break;
        case mHardwareEepromWrite:
            dest->size = sizeof(csHardwareEepromWrite);
            break;
        case mHardwareEepromErase:
            dest->size = sizeof(csHardwareEepromErase);
            break;
        case mHardwareClose:
            dest->size = sizeof(csHardwareClose);
            break;
        default:
            return SCOPEFUN_FAILURE;
    };
    return SCOPEFUN_SUCCESS;
}
int serverMessageHeader(messageHeader* dest, EMessage message)
{
    dest->magic[0] = 'S';
    dest->magic[1] = 'c';
    dest->magic[2] = 'o';
    dest->magic[3] = 'p';
    dest->magic[4] = 'e';
    dest->magic[5] = 'F';
    dest->magic[6] = 'u';
    dest->magic[7] = 'n';
    dest->message = (uint)message;
    dest->error   = SCOPEFUN_SUCCESS;
    switch(message)
    {
        case mClientConnect:
            dest->size = sizeof(scClientConnect);
            break;
        case mClientDisconnect:
            dest->size = sizeof(scClientDisconnect);
            break;
        case mClientDisplay:
            dest->size = sizeof(scClientDisplay);
            break;
        case mUpload:
            dest->size = sizeof(scUpload);
            break;
        case mDownload:
            dest->size = sizeof(scDownload);
            break;
        case mHardwareOpen:
            dest->size = sizeof(scHardwareOpen);
            break;
        case mHardwareIsOpened:
            dest->size = sizeof(scHardwareIsOpened);
            break;
        case mHardwareReset:
            dest->size = sizeof(scHardwareReset);
            break;
        case mHardwareConfig1:
            dest->size = sizeof(scHardwareConfig1);
            break;
        case mHardwareConfig2:
            dest->size = sizeof(scHardwareConfig2);
            break;
        case mHardwareCapture:
            dest->size = sizeof(scHardwareCapture);
            break;
        case mHardwareUploadFx2:
            dest->size = sizeof(scHardwareUploadFx2);
            break;
        case mHardwareUploadFpga:
            dest->size = sizeof(scHardwareUploadFpga);
            break;
        case mHardwareUploadGenerator:
            dest->size = sizeof(scHardwareUploadGenerator);
            break;
        case mHardwareEepromRead:
            dest->size = sizeof(scHardwareEepromRead);
            break;
        case mHardwareEepromReadFirmwareID:
           dest->size = sizeof(scHardwareEepromReadFirmwareID);
           break;
        case mHardwareEepromWrite:
            dest->size = sizeof(scHardwareEepromWrite);
            break;
        case mHardwareEepromErase:
            dest->size = sizeof(scHardwareEepromErase);
            break;
        case mHardwareClose:
            dest->size = sizeof(scHardwareClose);
            break;
        default:
            return SCOPEFUN_FAILURE;
    };
    return SCOPEFUN_SUCCESS;
}
int isClientHeaderOk(messageHeader* header)
{
    if(header->magic[0] != 'S' ||
       header->magic[1] != 'c' ||
       header->magic[2] != 'o' ||
       header->magic[3] != 'p' ||
       header->magic[4] != 'e' ||
       header->magic[5] != 'F' ||
       header->magic[6] != 'u' ||
       header->magic[7] != 'n'
      )
    {
        return SCOPEFUN_FAILURE;
    }
    switch(header->message)
    {
        case mClientConnect:
            if(header->size == sizeof(csClientConnect))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mClientDisconnect:
            if(header->size == sizeof(csClientDisconnect))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mClientDisplay:
            if(header->size == sizeof(csClientDisplay))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mUpload:
            if(header->size == sizeof(csUpload))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mDownload:
            if(header->size == sizeof(csDownload))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mHardwareOpen:
            if(header->size == sizeof(csHardwareOpen))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mHardwareIsOpened:
            if(header->size == sizeof(csHardwareIsOpened))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mHardwareReset:
            if(header->size == sizeof(csHardwareReset))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mHardwareConfig1:
            if(header->size == sizeof(csHardwareConfig1))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mHardwareConfig2:
            if(header->size == sizeof(csHardwareConfig2))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mHardwareCapture:
            if(header->size == sizeof(csHardwareCapture))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mHardwareUploadFx2:
            if(header->size == sizeof(csHardwareUploadFx2))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mHardwareUploadFpga:
            if(header->size == sizeof(csHardwareUploadFpga))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mHardwareUploadGenerator:
            if(header->size == sizeof(csHardwareUploadGenerator))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mHardwareEepromRead:
            if(header->size == sizeof(csHardwareEepromRead))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mHardwareEepromReadFirmwareID:
           if (header->size == sizeof(csHardwareEepromReadFirmwareID))
           {
              return SCOPEFUN_SUCCESS;
           }
        case mHardwareEepromWrite:
            if(header->size == sizeof(csHardwareEepromWrite))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mHardwareEepromErase:
            if(header->size == sizeof(csHardwareEepromErase))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mHardwareClose:
            if(header->size == sizeof(csHardwareClose))
            {
                return SCOPEFUN_SUCCESS;
            }
    };
    return SCOPEFUN_FAILURE;
}

int isServerHeaderOk(messageHeader* header)
{
    if(header->magic[0] != 'S' ||
       header->magic[1] != 'c' ||
       header->magic[2] != 'o' ||
       header->magic[3] != 'p' ||
       header->magic[4] != 'e' ||
       header->magic[5] != 'F' ||
       header->magic[6] != 'u' ||
       header->magic[7] != 'n'
      )
    {
        return SCOPEFUN_FAILURE;
    }
    switch(header->message)
    {
        case mClientConnect:
            if(header->size == sizeof(scClientConnect))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mClientDisconnect:
            if(header->size == sizeof(scClientDisconnect))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mClientDisplay:
            if(header->size == sizeof(scClientDisplay))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mUpload:
            if(header->size == sizeof(scUpload))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mDownload:
            if(header->size == sizeof(scDownload))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mHardwareOpen:
            if(header->size == sizeof(scHardwareOpen))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mHardwareIsOpened:
            if(header->size == sizeof(scHardwareIsOpened))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mHardwareReset:
            if(header->size == sizeof(scHardwareReset))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mHardwareConfig1:
            if(header->size == sizeof(scHardwareConfig1))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mHardwareConfig2:
            if(header->size == sizeof(scHardwareConfig2))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mHardwareCapture:
            if(header->size == sizeof(scHardwareCapture))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mHardwareUploadFx2:
            if(header->size == sizeof(scHardwareUploadFx2))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mHardwareUploadFpga:
            if(header->size == sizeof(scHardwareUploadFpga))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mHardwareUploadGenerator:
            if(header->size == sizeof(scHardwareUploadGenerator))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mHardwareEepromRead:
            if(header->size == sizeof(scHardwareEepromRead))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mHardwareEepromReadFirmwareID:
           if (header->size == sizeof(scHardwareEepromReadFirmwareID))
           {
              return SCOPEFUN_SUCCESS;
           }
        case mHardwareEepromWrite:
            if(header->size == sizeof(scHardwareEepromWrite))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mHardwareEepromErase:
            if(header->size == sizeof(scHardwareEepromErase))
            {
                return SCOPEFUN_SUCCESS;
            }
        case mHardwareClose:
            if(header->size == sizeof(scHardwareClose))
            {
                return SCOPEFUN_SUCCESS;
            }
    };
    return SCOPEFUN_FAILURE;
}

/*--------------------------------------------------------------------

   api

---------------------------------------------------------------------*/
SCOPEFUN_API int sfApiInit()
{
    socketInit();
    usbFxxInit(0);
    return SCOPEFUN_SUCCESS;
}
SCOPEFUN_API int sfApiCreateContext(SFContext* ctx, int memory)
{
    cMemSet((char*)ctx, 0, sizeof(SFContext));
    apiLock(ctx);
    // frame
    ctx->frame.maxMemory = memory;
    ctx->frame.data = cMalloc(ctx->frame.maxMemory);
    cMemSet((char*)ctx->frame.data, 0, ctx->frame.maxMemory);
    // socket
    ctx->net = cMalloc(sizeof(struct SocketContext));
    struct SocketContext* pExt = (SocketContext*)ctx->net;
    cMemSet((char*)pExt, 0, sizeof(SocketContext));
    // usb
    ctx->usb = cMalloc(sizeof(struct UsbContext));
    struct UsbContext* pCtx = (UsbContext*)ctx->usb;
    cMemSet((char*)pCtx, 0, sizeof(struct UsbContext));
    // version
    ctx->api.version = 1;
    ctx->api.major   = 0;
    ctx->api.minor   = 0;
    // mode
    ctx->api.mode = amUSB;
    // info
    ctx->frame.info.version = HARDWARE_VERSION_2;
    ctx->frame.info.header  = SCOPEFUN_FRAME_2_HEADER;
    ctx->frame.info.data    = memory;
    ctx->frame.info.packet  = SCOPEFUN_FRAME_2_PACKET;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfApiDeleteContext(SFContext* ctx)
{
    // frame
    cFree((char*)ctx->frame.data);
    // socket
    cFree((char*)ctx->net);
    // usb
    cFree((char*)ctx->usb);
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfApiVersion(SFContext* ctx, int* version, int* major, int* minor)
{
    apiLock(ctx);
    *version = ctx->api.version;
    *major   = ctx->api.major;
    *minor   = ctx->api.minor;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}
SCOPEFUN_API int sfApiExit()
{
    socketExit();
    usbFxxExit(0);
    return SCOPEFUN_SUCCESS;
}

/*----------------------------------------

   is

----------------------------------------*/
SCOPEFUN_API int sfIsUsb(SFContext* ctx)
{
    int usb = 0;
    apiLock(ctx);
    usb = ctx->api.mode == amUSB;
    apiUnlock(ctx);
    return usb;
}
SCOPEFUN_API int sfIsNetwork(SFContext* ctx)
{
    int net = 0;
    apiLock(ctx);
    net = ctx->api.mode == amNetwork;
    apiUnlock(ctx);
    return net;
}
SCOPEFUN_API int sfIsActive(SFContext* ctx)
{
    int active = 0;
    apiLock(ctx);
    active = ctx->api.active;
    apiUnlock(ctx);
    return active;
}
SCOPEFUN_API int sfIsSimulate(SFContext* ctx)
{
    int simulate = 0;
    apiLock(ctx);
    simulate = SDL_AtomicGet((SDL_atomic_t*)&ctx->simulate.on);
    apiUnlock(ctx);
    return simulate;
}
SCOPEFUN_API int sfIsConnected(SFContext* ctx)
{
    int connected = 0;
    apiLock(ctx);
    connected = ctx->client.connected;
    apiUnlock(ctx);
    return connected;
}
SCOPEFUN_API int sfIsThreadSafe(SFContext* ctx)
{
    int ts = 0;
    apiLock(ctx);
    ts = SDL_AtomicGet((SDL_atomic_t*)&ctx->api.thread);
    apiUnlock(ctx);
    return ts;
}

/*--------------------------------------------------------------------

   set

---------------------------------------------------------------------*/
SCOPEFUN_API int sfSetThreadSafe(SFContext* ctx, int threadSafe)
{
    while(SDL_AtomicCAS((SDL_atomic_t*)&ctx->api.thread, 0, threadSafe) == SDL_FALSE)
    {
        cSleep(1);
    }
    return SCOPEFUN_SUCCESS;
}
SCOPEFUN_API int sfSetNetwork(SFContext* ctx)
{
    apiLock(ctx);
    ctx->api.mode = amNetwork;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}
SCOPEFUN_API int sfSetUsb(SFContext* ctx)
{
    apiLock(ctx);
    ctx->api.mode = amUSB;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}
SCOPEFUN_API int sfSetActive(SFContext* ctx, int active)
{
    apiLock(ctx);
    ctx->api.active = active;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}
SCOPEFUN_API int sfSetTimeOut(SFContext* ctx, int timeout)
{
    apiLock(ctx);
    ctx->api.timeout = timeout;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}
SCOPEFUN_API int sfSetFrameVersion(SFContext* ctx, int version)
{
    apiLock(ctx);
    ctx->frame.info.version = version;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}
SCOPEFUN_API int sfSetFrameHeader(SFContext* ctx, int header)
{
    apiLock(ctx);
    ctx->frame.info.header = header;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}
SCOPEFUN_API int sfSetFrameData(SFContext* ctx, int data)
{
    apiLock(ctx);
    ctx->frame.info.data = data;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}
SCOPEFUN_API int sfSetFramePacket(SFContext* ctx, int packet)
{
    apiLock(ctx);
    ctx->frame.info.packet = packet;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}
SCOPEFUN_API int sfSetDisplay(SFContext* ctx, SDisplay* display)
{
    apiLock(ctx);
    ctx->client.display = *display;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}
SCOPEFUN_API int sfSetSimulateData(SFContext* ctx, SSimulate* sim)
{
    apiLock(ctx);
    ctx->simulate.data = *sim;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}
SCOPEFUN_API int sfSetSimulateOnOff(SFContext* ctx, int on)
{
    apiLock(ctx);
    SDL_AtomicSet((SDL_atomic_t*)&ctx->simulate.on, on);
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfSetActiveClients(SFContext* ctx, SActiveClients* clients)
{
    apiLock(ctx);
    ctx->server.active = *clients;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}

/*--------------------------------------------------------------------

   get

---------------------------------------------------------------------*/

SCOPEFUN_API int sfGetTimeOut(SFContext* ctx, int* timeout)
{
    apiLock(ctx);
    *timeout = ctx->api.timeout;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}
SCOPEFUN_API int sfGetFrameVersion(SFContext* ctx, int* version)
{
    apiLock(ctx);
    *version = ctx->frame.info.version;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}
SCOPEFUN_API int sfGetFrameHeader(SFContext* ctx, int* header)
{
    apiLock(ctx);
    *header = ctx->frame.info.header;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}
SCOPEFUN_API int sfGetFrameData(SFContext* ctx, int* data)
{
    apiLock(ctx);
    *data = ctx->frame.info.data;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}
SCOPEFUN_API int sfGetFramePacket(SFContext* ctx, int* packet)
{
    apiLock(ctx);
    *packet = ctx->frame.info.packet;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}
SCOPEFUN_API int sfGetClientId(SFContext* ctx, int* id)
{
    apiLock(ctx);
    *id = ctx->client.id;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}
SCOPEFUN_API int sfGetClientActiveIds(SFContext* ctx, SActiveClients* id)
{
    apiLock(ctx);
    *id = ctx->server.active;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}
SCOPEFUN_API int sfGetClientDisplay(SFContext* ctx, SDisplay* display)
{
    apiLock(ctx);
    *display = ctx->client.display;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}
SCOPEFUN_API int sfGetSimulateData(SFContext* ctx, SSimulate* data)
{
    apiLock(ctx);
    *data = ctx->simulate.data;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}

/*--------------------------------------------------------------------

   client

---------------------------------------------------------------------*/

SCOPEFUN_API int sfClientConnect(SFContext* ctx, const char* ip, int port)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    struct SocketContext* pSocketCtx = (SocketContext*)ctx->net;
    if(pSocketCtx->socket == 0 && ctx->api.active > 0)
    {
        int ret = socketCreate(pSocketCtx);
        if(ret == PURESOCKET_SUCCESS)
        {
            ret = socketConnect(pSocketCtx, ip, port);
            if(ret == PURESOCKET_SUCCESS)
            {
                int sent = 0;
                csClientConnect message = { 0 };
                clientMessageHeader(&message.header, mClientConnect);
                message.maxMemory = ctx->frame.maxMemory;
                ret = socketSend(pSocketCtx, (char*)&message, sizeof(csClientConnect), 0, &sent);
                if(ret == PURESOCKET_SUCCESS && sent == sizeof(csClientConnect))
                {
                    int received = 0;
                    scClientConnect response = { 0 };
                    ret = socketRecv(pSocketCtx, (char*)&response, sizeof(scClientConnect), 0, &received);
                    if(ret == PURESOCKET_SUCCESS && received == sizeof(scClientConnect) && isServerHeaderOk((messageHeader*)&response) == SCOPEFUN_SUCCESS)
                    {
                        ctx->client.connected = 1;
                        ctx->client.id        = response.id;
                        result = apiResult(ret);
                    }
                }
            }
        }
    }
    apiUnlock(ctx);
    return result;
}

SCOPEFUN_API int sfClientDisconnect(SFContext* ctx)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    struct SocketContext* pSocketCtx = (SocketContext*)ctx->net;
    if(pSocketCtx->socket > 0 && ctx->api.active > 0)
    {
        int ret = 0;
        int sent = 0;
        int received = 0;
        csClientDisconnect message = { 0 };
        clientMessageHeader(&message.header, mClientDisconnect);
        ret = socketSend(pSocketCtx, (char*)&message, sizeof(csClientDisconnect), 0, &sent);
        if(ret == PURESOCKET_SUCCESS && sent == sizeof(csClientDisconnect))
        {
            scClientDisconnect response = { 0 };
            ret = socketRecv(pSocketCtx, (char*)&response, sizeof(scClientDisconnect), 0, &received);
            if(ret == PURESOCKET_SUCCESS && received == sizeof(scClientDisconnect) && isServerHeaderOk((messageHeader*)&response) == SCOPEFUN_SUCCESS)
            {
                ret = socketClose(pSocketCtx);
                if(ret == PURESOCKET_SUCCESS)
                {
                    ctx->client.connected = 0;
                    pSocketCtx->socket    = 0;
                    result = apiResult(ret);
                }
            }
        }
    }
    apiUnlock(ctx);
    return result;
}

SCOPEFUN_API int sfClientDisplay(SFContext* ctx, int clientId, SDisplay* display)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    struct SocketContext* pSocketCtx = (SocketContext*)ctx->net;
    if(pSocketCtx->socket > 0 && ctx->api.active > 0)
    {
        int ret = 0;
        int sent = 0;
        int received = 0;
        csClientDisplay message = { 0 };
        clientMessageHeader(&message.header, mClientDisplay);
        message.clientId = clientId;
        message.display  = *display;
        ret = socketSend(pSocketCtx, (char*)&message, sizeof(csClientDisplay), 0, &sent);
        if(ret == PURESOCKET_SUCCESS && sent == sizeof(csClientDisplay))
        {
            scClientDisplay response = { 0 };
            ret = socketRecv(pSocketCtx, (char*)&response, sizeof(scClientDisplay), 0, &received);
            if(ret == PURESOCKET_SUCCESS && received == sizeof(scClientDisplay) && isServerHeaderOk((messageHeader*)&response) == SCOPEFUN_SUCCESS)
            {
                result = apiResult(ret);
            }
        }
    }
    apiUnlock(ctx);
    return result;
}


/*----------------------------------------

   server

----------------------------------------*/

SCOPEFUN_API int sfServerDownload(SFContext* ctx)
{
    int network = sfIsNetwork(ctx);
    int    ret = 0;
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    if(network)
    {
        struct SocketContext* pSocketCtx = (SocketContext*)ctx->net;
        if(pSocketCtx->socket > 0 && ctx->api.active > 0)
        {
            int sent = 0;
            int received = 0;
            csDownload message = { 0 };
            clientMessageHeader(&message.header, mDownload);
            ret = socketSend(pSocketCtx, (char*)&message, sizeof(message), 0, &sent);
            if(ret == PURESOCKET_SUCCESS && sent == sizeof(csDownload))
            {
                scDownload downloaded = { 0 };
                ret = socketRecv(pSocketCtx, (char*)&downloaded, sizeof(scDownload), 0, &received);
                if(ret == PURESOCKET_SUCCESS && received == sizeof(scDownload) && isServerHeaderOk((messageHeader*)&downloaded) == SCOPEFUN_SUCCESS)
                {
                    SDL_AtomicSet((SDL_atomic_t*)&ctx->simulate.on, downloaded.simOnOff);
                    ctx->simulate.data  = downloaded.simulate;
                    ctx->client.display = downloaded.display;
                    ctx->frame.info     = downloaded.frame;
                    ctx->server.active  = downloaded.active;
                    result = apiResult(ret);
                }
            }
        }
    }
    apiUnlock(ctx);
    return result;
}

SCOPEFUN_API int sfServerUpload(SFContext* ctx)
{
    int network = sfIsNetwork(ctx);
    int    ret = 0;
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    if(network)
    {
        struct SocketContext* pSocketCtx = (SocketContext*)ctx->net;
        if(pSocketCtx->socket > 0 && ctx->api.active > 0)
        {
            int sent = 0;
            int received = 0;
            csUpload message = { 0 };
            clientMessageHeader(&message.header, mUpload);
            message.frame     = ctx->frame.info;
            message.simulate  = ctx->simulate.data;
            message.simOnOff  = SDL_AtomicGet((SDL_atomic_t*)&ctx->simulate.on);
            ret = socketSend(pSocketCtx, (char*)&message, sizeof(message), 0, &sent);
            if(ret == PURESOCKET_SUCCESS && sent == sizeof(csUpload))
            {
                scUpload response = { 0 };
                ret = socketRecv(pSocketCtx, (char*)&response, sizeof(response), 0, &received);
                if(ret == PURESOCKET_SUCCESS && received == sizeof(scUpload) && isServerHeaderOk((messageHeader*)&response) == SCOPEFUN_SUCCESS)
                {
                    result = apiResult(ret);
                }
            }
        }
    }
    apiUnlock(ctx);
    return result;
}

/*--------------------------------------------------------------------

  net

---------------------------------------------------------------------*/
SCOPEFUN_API int netHardwareOpen(SFContext* ctx, SUsb* config, int version)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    struct SocketContext* pSocketCtx = (SocketContext*)ctx->net;
    if(pSocketCtx->socket > 0 && ctx->api.active > 0)
    {
        int ret = 0;
        int sent = 0;
        int received = 0;
        csHardwareOpen message = { 0 };
        clientMessageHeader(&message.header, mHardwareOpen);
        message.usb     = *config;
        message.version = version;
        ret = socketSend(pSocketCtx, (char*)&message, sizeof(message), 0, &sent);
        if(ret == PURESOCKET_SUCCESS && sent == sizeof(csHardwareOpen))
        {
            scHardwareOpen response = { 0 };
            ret = socketRecv(pSocketCtx, (char*)&response, sizeof(scHardwareOpen), 0, &received);
            if(ret == PURESOCKET_SUCCESS && received == sizeof(scHardwareOpen) && isServerHeaderOk((messageHeader*)&response) == SCOPEFUN_SUCCESS)
            {
                result = apiResult(ret);
            }
        }
    }
    apiUnlock(ctx);
    return result;
}
SCOPEFUN_API int netHardwareIsOpened(SFContext* ctx, int* open)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    struct SocketContext* pSocketCtx = (SocketContext*)ctx->net;
    if(pSocketCtx->socket > 0 && ctx->api.active > 0)
    {
        int ret = 0;
        int sent = 0;
        int received = 0;
        csHardwareIsOpened message = { 0 };
        clientMessageHeader(&message.header, mHardwareIsOpened);
        ret = socketSend(pSocketCtx, (char*)&message, sizeof(message), 0, &sent);
        if(ret == PURESOCKET_SUCCESS && sent == sizeof(csHardwareIsOpened))
        {
            scHardwareIsOpened response = { 0 };
            ret = socketRecv(pSocketCtx, (char*)&response, sizeof(scHardwareIsOpened), 0, &received);
            if(ret == PURESOCKET_SUCCESS && received == sizeof(scHardwareIsOpened) && isServerHeaderOk((messageHeader*)&response) == SCOPEFUN_SUCCESS)
            {
                *open = response.opened;
                result = apiResult(ret);
            }
        }
    }
    apiUnlock(ctx);
    return result;
}
SCOPEFUN_API int netHardwareReset(SFContext* ctx)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    struct SocketContext* pSocketCtx = (SocketContext*)ctx->net;
    if(pSocketCtx->socket > 0 && ctx->api.active > 0)
    {
        int ret = 0;
        int sent = 0;
        int received = 0;
        csHardwareReset message = { 0 };
        clientMessageHeader(&message.header, mHardwareReset);
        ret = socketSend(pSocketCtx, (char*)&message, sizeof(message), 0, &sent);
        if(ret == PURESOCKET_SUCCESS && sent == sizeof(csHardwareReset))
        {
            scHardwareReset response = { 0 };
            ret = socketRecv(pSocketCtx, (char*)&response, sizeof(scHardwareReset), 0, &received);
            if(ret == PURESOCKET_SUCCESS && received == sizeof(scHardwareReset) && isServerHeaderOk((messageHeader*)&response) == SCOPEFUN_SUCCESS)
            {
                result = apiResult(ret);
            }
        }
    }
    apiUnlock(ctx);
    return result;
}
SCOPEFUN_API int netHardwareConfig1(SFContext* ctx, SHardware1* config)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    struct SocketContext* pSocketCtx = (SocketContext*)ctx->net;
    if(pSocketCtx->socket > 0 && ctx->api.active > 0)
    {
        int ret = 0;
        int sent = 0;
        int received = 0;
        csHardwareConfig1 message = { 0 };
        clientMessageHeader(&message.header, mHardwareConfig1);
        message.config = *config;
        ret = socketSend(pSocketCtx, (char*)&message, sizeof(message), 0, &sent);
        if(ret == PURESOCKET_SUCCESS && sent == sizeof(csHardwareConfig1))
        {
            scHardwareConfig1 response = { 0 };
            ret = socketRecv(pSocketCtx, (char*)&response, sizeof(scHardwareConfig1), 0, &received);
            if(ret == PURESOCKET_SUCCESS && received == sizeof(scHardwareConfig1) && isServerHeaderOk((messageHeader*)&response) == SCOPEFUN_SUCCESS)
            {
                result = apiResult(ret);
            }
        }
    }
    apiUnlock(ctx);
    return result;
}
SCOPEFUN_API int netHardwareConfig2(SFContext* ctx, SHardware2* config)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    struct SocketContext* pSocketCtx = (SocketContext*)ctx->net;
    if(pSocketCtx->socket > 0 && ctx->api.active > 0)
    {
        int ret = 0;
        int sent = 0;
        int received = 0;
        csHardwareConfig2 message = { 0 };
        clientMessageHeader(&message.header, mHardwareConfig2);
        message.config = *config;
        ret = socketSend(pSocketCtx, (char*)&message, sizeof(message), 0, &sent);
        if(ret == PURESOCKET_SUCCESS && sent == sizeof(csHardwareConfig2))
        {
            scHardwareConfig2 response = { 0 };
            ret = socketRecv(pSocketCtx, (char*)&response, sizeof(scHardwareConfig2), 0, &received);
            if(ret == PURESOCKET_SUCCESS && received == sizeof(scHardwareConfig2) && isServerHeaderOk((messageHeader*)&response) == SCOPEFUN_SUCCESS)
            {
                result = apiResult(ret);
            }
        }
    }
    apiUnlock(ctx);
    return result;
}
SCOPEFUN_API int netHardwareCapture(SFContext* ctx, SFrameData* data, int len, int* transfered,int type)
{
    int result = SCOPEFUN_FAILURE;
    *transfered = 0;
    struct SocketContext* pSocketCtx = (SocketContext*)ctx->net;
    if(pSocketCtx->socket > 0 && ctx->api.active > 0)
    {
        len = apiMin(len, SCOPEFUN_FRAME_MEMORY);
        int ret = 0;
        int sent = 0;
        int received = 0;
        csHardwareCapture message = { 0 };
        clientMessageHeader(&message.header, mHardwareCapture);
        message.len  = apiMin(len,SCOPEFUN_FRAME_MEMORY);
        message.type = type;
        ret = socketSend(pSocketCtx, (char*)&message, sizeof(message), 0, &sent);
        if(ret == PURESOCKET_SUCCESS && sent == sizeof(csHardwareCapture))
        {
            // header
            messageHeader header = { 0 };
            ret = socketRecv(pSocketCtx, (char*)&header, sizeof(messageHeader), 0, &received);
            if(ret == PURESOCKET_SUCCESS && received == sizeof(messageHeader) && isServerHeaderOk((messageHeader*)&header) == SCOPEFUN_SUCCESS)
            {
                // size
                uint bytes = 0;
                ret = socketRecv(pSocketCtx, (char*)&bytes, sizeof(uint), 0, &received);
                if(ret == PURESOCKET_SUCCESS && received == sizeof(uint))
                {
                    // data
                    bytes = apiMin(bytes, len);
                    ret = socketRecv(pSocketCtx, (char*)&data->data.bytes[0], bytes, 0, &received);
                    if(ret == PURESOCKET_SUCCESS && bytes == received)
                    {
                        *transfered = received;
                        result = apiResult(ret);
                    }
                    else
                    {
                        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "ScopeFun API: communication error 4" );
                        int debug = 1;
                    }
                }
                else
                {
                    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "ScopeFun API: communication error 3" );
                    int debug = 1;
                }
            }
            else
            {
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "ScopeFun API: communication error 2" );
                int debug = 1;
            }
        }
        else
        {
            int debug = 1;
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "ScopeFun API: communication error 1" );
        }
    }
    return result;
}

SCOPEFUN_API int netHardwareUploadFx2(SFContext* ctx, SFx2* fw)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    struct SocketContext* pSocketCtx = (SocketContext*)ctx->net;
    if(pSocketCtx->socket > 0 && ctx->api.active > 0)
    {
        int ret = 0;
        int sent = 0;
        int received = 0;
        messageHeader header = { 0 };
        clientMessageHeader(&header, mHardwareUploadFx2);
        ret = socketSend(pSocketCtx, (char*)&header, sizeof(messageHeader), 0, &sent);
        if(ret == PURESOCKET_SUCCESS && sent == sizeof(messageHeader))
        {
            ret = socketSend(pSocketCtx, (char*)fw, sizeof(SFx2), 0, &sent);
            if(ret == PURESOCKET_SUCCESS && sent == sizeof(SFx2))
            {
                scHardwareUploadFx2 response = { 0 };
                ret = socketRecv(pSocketCtx, (char*)&response, sizeof(scHardwareUploadFx2), 0, &received);
                if(ret == PURESOCKET_SUCCESS && received == sizeof(scHardwareUploadFx2) && isServerHeaderOk((messageHeader*)&response) == SCOPEFUN_SUCCESS)
                {
                    result = apiResult(ret);
                }
            }
        }
    }
    apiUnlock(ctx);
    return result;
}
SCOPEFUN_API int netHardwareUploadFpga(SFContext* ctx, SFpga* fpga)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    struct SocketContext* pSocketCtx = (SocketContext*)ctx->net;
    if(pSocketCtx->socket > 0 && ctx->api.active > 0)
    {
        int ret = 0;
        int sent = 0;
        int received = 0;
        messageHeader header = { 0 };
        clientMessageHeader(&header, mHardwareUploadFpga);
        ret = socketSend(pSocketCtx, (char*)&header, sizeof(messageHeader), 0, &sent);
        if(ret == PURESOCKET_SUCCESS && sent == sizeof(messageHeader))
        {
            ret = socketSend(pSocketCtx, (char*)fpga, sizeof(SFpga), 0, &sent);
            if(ret == PURESOCKET_SUCCESS && sent == sizeof(SFpga))
            {
                scHardwareUploadFpga response = { 0 };
                ret = socketRecv(pSocketCtx, (char*)&response, sizeof(scHardwareUploadFpga), 0, &received);
                if(ret == PURESOCKET_SUCCESS && received == sizeof(scHardwareUploadFpga) && isServerHeaderOk((messageHeader*)&response) == SCOPEFUN_SUCCESS)
                {
                    result = apiResult(ret);
                }
            }
        }
    }
    apiUnlock(ctx);
    return result;
}
SCOPEFUN_API int netHardwareUploadGenerator(SFContext* ctx, SGenerator* data)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    struct SocketContext* pSocketCtx = (SocketContext*)ctx->net;
    if(pSocketCtx->socket > 0 && ctx->api.active > 0)
    {
        int ret = 0;
        int sent = 0;
        int received = 0;
        messageHeader header = { 0 };
        clientMessageHeader(&header, mHardwareUploadGenerator);
        ret = socketSend(pSocketCtx, (char*)&header, sizeof(messageHeader), 0, &sent);
        if(ret == PURESOCKET_SUCCESS && sent == sizeof(messageHeader))
        {
            ret = socketSend(pSocketCtx, (char*)data, sizeof(SGenerator), 0, &sent);
            if(ret == PURESOCKET_SUCCESS && sent == sizeof(SGenerator))
            {
                scHardwareUploadGenerator response = { 0 };
                ret = socketRecv(pSocketCtx, (char*)&response, sizeof(scHardwareUploadGenerator), 0, &received);
                if(ret == PURESOCKET_SUCCESS && received == sizeof(scHardwareUploadGenerator) && isServerHeaderOk((messageHeader*)&response) == SCOPEFUN_SUCCESS)
                {
                    result = apiResult(ret);
                }
            }
        }
    }
    apiUnlock(ctx);
    return result;
}
SCOPEFUN_API int netHardwareEepromRead(SFContext* ctx, SEeprom* data, int size, int adress)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    struct SocketContext* pSocketCtx = (SocketContext*)ctx->net;
    if(pSocketCtx->socket > 0 && ctx->api.active > 0)
    {
        int ret = 0;
        int sent = 0;
        int received = 0;
        // message
        csHardwareEepromRead message = { 0 };
        clientMessageHeader(&message.header, mHardwareEepromRead);
        message.size = size;
        message.address = adress;
        ret = socketSend(pSocketCtx, (char*)&message, sizeof(message), 0, &sent);
        if(ret == PURESOCKET_SUCCESS && sent == sizeof(csHardwareEepromRead))
        {
            // response
            messageHeader header = { 0 };
            ret = socketRecv(pSocketCtx, (char*)&header, sizeof(messageHeader), 0, &received);
            if(ret == PURESOCKET_SUCCESS && received == sizeof(messageHeader) && isServerHeaderOk((messageHeader*)&header) == SCOPEFUN_SUCCESS)
            {
                ret = socketRecv(pSocketCtx, (char*)data, sizeof(SEeprom), 0, &received);
                if(ret == PURESOCKET_SUCCESS && received == sizeof(SEeprom))
                {
                    result = apiResult(ret);
                }
            }
        }
    }
    apiUnlock(ctx);
    return result;
}

SCOPEFUN_API int netHardwareEepromReadFirmwareID(SFContext* ctx, SEeprom* data, int size, int adress)
{
   int result = SCOPEFUN_FAILURE;
   apiLock(ctx);
   struct SocketContext* pSocketCtx = (SocketContext*)ctx->net;
   if (pSocketCtx->socket > 0 && ctx->api.active > 0)
   {
      int ret = 0;
      int sent = 0;
      int received = 0;
      // message
      csHardwareEepromReadFirmwareID message = { 0 };
      clientMessageHeader(&message.header, mHardwareEepromReadFirmwareID);
      message.size = size;
      message.address = adress;
      ret = socketSend(pSocketCtx, (char*)&message, sizeof(message), 0, &sent);
      if (ret == PURESOCKET_SUCCESS && sent == sizeof(csHardwareEepromReadFirmwareID))
      {
         // response
         messageHeader header = { 0 };
         ret = socketRecv(pSocketCtx, (char*)&header, sizeof(messageHeader), 0, &received);
         if (ret == PURESOCKET_SUCCESS && received == sizeof(messageHeader) && isServerHeaderOk((messageHeader*)&header) == SCOPEFUN_SUCCESS)
         {
            ret = socketRecv(pSocketCtx, (char*)data, sizeof(SEeprom), 0, &received);
            if (ret == PURESOCKET_SUCCESS && received == sizeof(SEeprom))
            {
               result = apiResult(ret);
            }
         }
      }
   }
   apiUnlock(ctx);
   return result;
}

SCOPEFUN_API int netHardwareEepromWrite(SFContext* ctx, SEeprom* data, int size, int adress)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    struct SocketContext* pSocketCtx = (SocketContext*)ctx->net;
    if(pSocketCtx->socket > 0 && ctx->api.active > 0)
    {
        int ret = 0;
        int sent = 0;
        int received = 0;
        // message
        messageHeader header = { 0 };
        clientMessageHeader(&header, mHardwareEepromWrite);
        ret = socketSend(pSocketCtx, (char*)&header, sizeof(messageHeader), 0, &sent);
        if(ret == PURESOCKET_SUCCESS && sent == sizeof(messageHeader))
        {
            ret = socketSend(pSocketCtx, (char*)&size, 4, 0, &sent);
            if(ret == PURESOCKET_SUCCESS && sent == 4)
            {
                ret = socketSend(pSocketCtx, (char*)&adress, 4, 0, &sent);
                if(ret == PURESOCKET_SUCCESS && sent == 4)
                {
                    ret = socketSend(pSocketCtx, (char*)data, sizeof(SEeprom), 0, &sent);
                    if(ret == PURESOCKET_SUCCESS && sent == sizeof(SEeprom))
                    {
                        // response
                        scHardwareEepromWrite response = { 0 };
                        ret = socketRecv(pSocketCtx, (char*)&response, sizeof(response), 0, &received);
                        if(ret == PURESOCKET_SUCCESS && received == sizeof(response) && isServerHeaderOk((messageHeader*)&response) == SCOPEFUN_SUCCESS)
                        {
                            result = apiResult(ret);
                        }
                    }
                }
            }
        }
    }
    apiUnlock(ctx);
    return result;
}
SCOPEFUN_API int netHardwareEepromErase(SFContext* ctx)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    struct SocketContext* pSocketCtx = (SocketContext*)ctx->net;
    if(pSocketCtx->socket > 0 && ctx->api.active > 0)
    {
        int ret = 0;
        int sent = 0;
        int received = 0;
        // message
        csHardwareEepromErase message = { 0 };
        clientMessageHeader(&message.header, mHardwareEepromErase);
        ret = socketSend(pSocketCtx, (char*)&message, sizeof(csHardwareEepromErase), 0, &sent);
        if(ret == SCOPEFUN_SUCCESS && sent == sizeof(csHardwareEepromErase))
        {
            // response
            scHardwareEepromErase response = { 0 };
            ret = socketRecv(pSocketCtx, (char*)&response, sizeof(scHardwareEepromErase), 0, &received);
            if(ret == PURESOCKET_SUCCESS && received == sizeof(scHardwareEepromErase) && isServerHeaderOk((messageHeader*)&response) == SCOPEFUN_SUCCESS)
            {
                result = apiResult(ret);
            }
        }
    }
    apiUnlock(ctx);
    return result;
}
SCOPEFUN_API int netHardwareClose(SFContext* ctx)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    struct SocketContext* pSocketCtx = (SocketContext*)ctx->net;
    if(pSocketCtx->socket > 0 && ctx->api.active > 0)
    {
        int ret      = 0;
        int sent     = 0;
        int received = 0;
        // message
        csHardwareClose message = { 0 };
        clientMessageHeader(&message.header, mHardwareClose);
        ret = socketSend(pSocketCtx, (char*)&message, sizeof(csHardwareClose), 0, &sent);
        if(ret == SCOPEFUN_SUCCESS && sent == sizeof(csHardwareClose))
        {
            // response
            scHardwareClose response = { 0 };
            ret = socketRecv(pSocketCtx, (char*)&response, sizeof(scHardwareClose), 0, &received);
            if(ret == PURESOCKET_SUCCESS && received == sizeof(scHardwareClose) && isServerHeaderOk((messageHeader*)&response) == SCOPEFUN_SUCCESS)
            {
                result = apiResult(ret);
            }
        }
    }
    apiUnlock(ctx);
    return result;
}

/*--------------------------------------------------------------------

   usb

---------------------------------------------------------------------*/
SCOPEFUN_API int usbHardwareOpen(SFContext* ctx, SUsb* cfg, int version)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    if(ctx->api.active > 0)
    {
        struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
        UsbGuid id;
        cMemCpy((char*)&id, (char*)&cfg->guid, sizeof(struct UsbGuid));
        usbDevice* deviceList[4] = { 0 };
        usbFxxGuidVidPid(pUsbCtx, id, cfg->idVendor, cfg->idProduct, cfg->idSerial);
        int ret = PUREUSB_SUCCESS;
        ret += usbFxxFreeList(pUsbCtx);
        ret += usbFxxFindList(pUsbCtx, deviceList, 4);
        ret += usbFxxOpenNormal(pUsbCtx, deviceList, 4);
        result = apiResult(ret);
        pUsbCtx->version = version;
    }
    apiUnlock(ctx);
    return result;
}
SCOPEFUN_API int usbHardwareIsOpened(SFContext* ctx, int* open)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    if(ctx->api.active > 0)
    {
        struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
        *open = usbFxxIsConnected(pUsbCtx);
        result = SCOPEFUN_SUCCESS;
    }
    if(SDL_AtomicGet((SDL_atomic_t*)&ctx->simulate.on) > 0)
    {
        *open  = 1;
        result = SCOPEFUN_SUCCESS;
    }
    apiUnlock(ctx);
    return result;
}
SCOPEFUN_API int usbHardwareReset(SFContext* ctx)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    if(ctx->api.active > 0)
    {
        struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
        int ret = PUREUSB_FAILURE;
        if(pUsbCtx->version == HARDWARE_VERSION_1)
        {
            ret = usbFx2Reset(pUsbCtx);
        }
        if(pUsbCtx->version == HARDWARE_VERSION_2)
        {
            ret = usbFx3Reset(pUsbCtx);
        }
        result = apiResult(ret);
    }
    apiUnlock(ctx);
    return result;
}
SCOPEFUN_API int usbHardwareConfig1(SFContext* ctx, SHardware1* hw)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    if(ctx->api.active > 0)
    {
        struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
        int swap = 0;
        int transfered = 0;
        int ret = usbFxxTransferDataOut(pUsbCtx, 2, (char*)hw, sizeof(SHardware1), swap, ctx->api.timeout, &transfered);
        result = apiResult(ret);
    }
    apiUnlock(ctx);
    return result;
}
SCOPEFUN_API int usbHardwareConfig2(SFContext* ctx, SHardware2* hw)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    if(ctx->api.active > 0)
    {
        struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
        int swap = 1;
        int transfered = 0;
        int ret = usbFxxTransferDataOut(pUsbCtx, 2, (char*)hw, sizeof(SHardware2), swap, ctx->api.timeout, &transfered);
        result = apiResult(ret);
    }
    apiUnlock(ctx);
    return result;
}
SCOPEFUN_API int usbHardwareClose(SFContext* ctx)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    if(ctx->api.active > 0)
    {
        struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
        usbFxxClose(pUsbCtx);
        result = SCOPEFUN_SUCCESS;
    }
    apiUnlock(ctx);
    return result;
}

SCOPEFUN_API int usbHardwareCapture(SFContext* ctx, SFrameData* dest, int size, int* transfer)
{
    int result = SCOPEFUN_FAILURE;
    size = apiMin(size, SCOPEFUN_FRAME_MEMORY);
    if(ctx->api.active > 0)
    {
        struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
        int swap = 0;
        int ret = usbFxxTransferDataIn(pUsbCtx, 6, (byte*)dest->data.bytes, size, swap, ctx->api.timeout, transfer);
        result = apiResult(ret);
    }
    return result;
}

SCOPEFUN_API int usbHardwareUploadFx2(SFContext* ctx, SFx2* fx2)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    if(ctx->api.active > 0)
    {
        struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
        if(pUsbCtx->version == HARDWARE_VERSION_1)
        {
            int ret = usbFx2UploadFirmwareToFx2(pUsbCtx, fx2->data.bytes);
            result = apiResult(ret);
            cSleep(3000);
        }
        if(pUsbCtx->version == HARDWARE_VERSION_2)
        {
            int ret = usbFx3UploadFirmwareToFx3(pUsbCtx, fx2->data.bytes);
            result = apiResult(ret);
            cSleep(3000);
        }
    }
    apiUnlock(ctx);
    return result;
}
SCOPEFUN_API int usbHardwareUploadFpga(SFContext* ctx, SFpga* buffer)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    if(ctx->api.active > 0)
    {
        struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
        if(pUsbCtx->version == HARDWARE_VERSION_1)
        {
            int ret = usbFx2UploadFirmwareToFpga(pUsbCtx, (byte*)buffer->data.bytes, buffer->size, 1);
            result = apiResult(ret);
        }
        if(pUsbCtx->version == HARDWARE_VERSION_2)
        {
            int ret = 0;
            char id[EEPROM_BYTE_COUNT] = { 0 };
            ret += usbFx3CheckFirmwareID(pUsbCtx, id);
            ret += usbFx3UploadFirmwareToFpga(pUsbCtx, (byte*)buffer->data.bytes, buffer->size, 0);
            result = apiResult(ret);
        }
    }
    apiUnlock(ctx);
    return result;
}
SCOPEFUN_API int usbHardwareUploadGenerator(SFContext* ctx, SGenerator* buffer)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    if(ctx->api.active > 0)
    {
        struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
        int transfered = 0;
        int ret = usbFxxTransferDataOut(pUsbCtx, 4, (byte*)buffer, sizeof(SGenerator), 0, ctx->api.timeout, &transfered);
        result = apiResult(ret);
    }
    apiUnlock(ctx);
    return result;
}
SCOPEFUN_API int usbHardwareEepromRead(SFContext* ctx, SEeprom* buffer, int size, int adress)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    if(ctx->api.active > 0)
    {
        struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
        if(pUsbCtx->version == HARDWARE_VERSION_1)
        {
            int ret = usbFx2ReadEEPROM(pUsbCtx, (byte*)&buffer->data.bytes[0], size, 0);
            result = apiResult(ret);
        }
        if(pUsbCtx->version == HARDWARE_VERSION_2)
        {
            int ret = usbFx3ReadEEPROM(pUsbCtx, (byte*)&buffer->data.bytes[0], size, adress);
            result  = apiResult(ret);
        }
    }
    apiUnlock(ctx);
    return result;
}

SCOPEFUN_API int usbHardwareEepromReadFirmwareID(SFContext* ctx, SEeprom* buffer, int size, int adress)
{
   int result = SCOPEFUN_FAILURE;
   apiLock(ctx);
   if (ctx->api.active > 0)
   {
      struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
      if (pUsbCtx->version == HARDWARE_VERSION_1)
      {
         int ret = usbFx3ReadEEPROMFirmwareID(pUsbCtx, (byte*)&buffer->data.bytes[0], SCOPEFUN_EEPROM_FIRMWARE_NAME_BYTES, adress);
         result  = apiResult(ret);
      }
      if (pUsbCtx->version == HARDWARE_VERSION_2)
      {
         int ret = usbFx3ReadEEPROMFirmwareID(pUsbCtx, (byte*)&buffer->data.bytes[0], SCOPEFUN_EEPROM_FIRMWARE_NAME_BYTES, adress);
         result = apiResult(ret);
      }
   }
   apiUnlock(ctx);
   return result;
}


SCOPEFUN_API int usbHardwareEepromWrite(SFContext* ctx, SEeprom* buffer, int size, int address)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    if(ctx->api.active > 0)
    {
        struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
        if(pUsbCtx->version == HARDWARE_VERSION_1)
        {
            int ret = usbFx2WriteEEPROM(pUsbCtx, (byte*)&buffer->data.bytes[0], size, 0);
            result = apiResult(ret);
        }
        if(pUsbCtx->version == HARDWARE_VERSION_2)
        {
            int ret = usbFx3WriteEEPROM(pUsbCtx, (byte*)&buffer->data.bytes[0], size, address);
            result = apiResult(ret);
        }
    }
    apiUnlock(ctx);
    return result;
}
SCOPEFUN_API int usbHardwareEepromErase(SFContext* ctx)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    if(ctx->api.active > 0)
    {
        struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
        UsbEEPROM eeprom;
        cMemSet((char*)&eeprom, 0, sizeof(struct UsbEEPROM));
        if(pUsbCtx->version == HARDWARE_VERSION_1)
        {
            int ret = usbFx2WriteEEPROM(pUsbCtx, (byte*)&eeprom, sizeof(UsbEEPROM), 0);
            result = apiResult(ret);
        }
        if(pUsbCtx->version == HARDWARE_VERSION_2)
        {
            int ret = usbFx3WriteEEPROM(pUsbCtx, (byte*)&eeprom, sizeof(UsbEEPROM), 0);
            result = apiResult(ret);
        }
    }
    apiUnlock(ctx);
    return result;
}

/*--------------------------------------------------------------------

    hardware

---------------------------------------------------------------------*/
SCOPEFUN_API int sfHardwareOpen(SFContext* ctx, SUsb* usb, int version)
{
    int ret = 0;
    if(sfIsUsb(ctx))
    {
        ret = usbHardwareOpen(ctx, usb, version);
    }
    if(sfIsNetwork(ctx))
    {
        ret = netHardwareOpen(ctx, usb, version);
    }
    return ret;
}

SCOPEFUN_API int sfHardwareReset(SFContext* ctx)
{
    int ret = 0;
    if(sfIsUsb(ctx))
    {
        ret = usbHardwareReset(ctx);
    }
    if(sfIsNetwork(ctx))
    {
        ret = netHardwareReset(ctx);
    }
    return ret;
}

SCOPEFUN_API int sfHardwareIsOpened(SFContext* ctx, int* open)
{
    int ret = 0;
    if(sfIsUsb(ctx))
    {
        ret = usbHardwareIsOpened(ctx, open);
    }
    if(sfIsNetwork(ctx))
    {
        ret = netHardwareIsOpened(ctx, open);
    }
    return ret;
}
SCOPEFUN_API int sfHardwareConfig1(SFContext* ctx, SHardware1* hw)
{
    int ret = 0;
    if(sfIsUsb(ctx))
    {
        ret = usbHardwareConfig1(ctx, hw);
    }
    if(sfIsNetwork(ctx))
    {
        ret = netHardwareConfig1(ctx, hw);
    }
    return ret;
}
SCOPEFUN_API int sfHardwareConfig2(SFContext* ctx, SHardware2* hw)
{
    int ret = 0;
    if(sfIsUsb(ctx))
    {
        ret = usbHardwareConfig2(ctx, hw);
    }
    if(sfIsNetwork(ctx))
    {
        ret = netHardwareConfig2(ctx, hw);
    }
    return ret;
}
SCOPEFUN_API int sfHardwareCapture(SFContext* ctx, SFrameData* buffer, int len, int* received,int type)
{
    int      ret = 0;
    if(sfIsSimulate(ctx))
    {
        apiLock(ctx);
        int simLen = apiMin(len, ctx->frame.maxMemory);
        if(ctx->frame.info.version == 1 && (ctx->frame.received + simLen) > ctx->frame.maxMemory)
        {
            simLen = ctx->frame.maxMemory - ctx->frame.received;
        }
        if(ctx->frame.info.version == 2 && (ctx->frame.received + simLen) > ctx->frame.maxMemory)
        {
            simLen = ctx->frame.maxMemory - ctx->frame.received;
        }
        SDL_memcpy(&buffer->data.bytes[0], (byte*)&ctx->frame.data->data.bytes[0] + ctx->frame.received, simLen);
        ctx->frame.received += simLen;
        *received = simLen;
        apiUnlock(ctx);
    }
    if(sfIsUsb(ctx))
    {
        apiLock(ctx);
        ret = usbHardwareCapture(ctx, buffer, len, received);
        apiUnlock(ctx);
    }
    if(sfIsNetwork(ctx))
    {
        apiLock(ctx);
        ret = netHardwareCapture(ctx, buffer, len, received, type);
        apiUnlock(ctx);
    }
    return ret;
}

SCOPEFUN_API int sfHardwareUploadFx2(SFContext* ctx, SFx2* fx2)
{
    int ret = 0;
    if(sfIsUsb(ctx))
    {
        ret = usbHardwareUploadFx2(ctx, fx2);
    }
    if(sfIsNetwork(ctx))
    {
        ret = netHardwareUploadFx2(ctx, fx2);
    }
    return ret;
}
SCOPEFUN_API int sfHardwareUploadFpga(SFContext* ctx, SFpga* fpga)
{
    int ret = 0;
    if(sfIsUsb(ctx))
    {
        ret = usbHardwareUploadFpga(ctx, fpga);
    }
    if(sfIsNetwork(ctx))
    {
        ret = netHardwareUploadFpga(ctx, fpga);
    }
    return ret;
}
SCOPEFUN_API int sfHardwareUploadGenerator(SFContext* ctx, SGenerator* gen)
{
    int ret = 0;
    if(sfIsUsb(ctx))
    {
        ret = usbHardwareUploadGenerator(ctx, gen);
    }
    if(sfIsNetwork(ctx))
    {
        ret = netHardwareUploadGenerator(ctx, gen);
    }
    return ret;
}
SCOPEFUN_API int sfHardwareEepromRead(SFContext* ctx, SEeprom* eeprom, int size, int adress)
{
    int ret = 0;
    if(sfIsUsb(ctx))
    {
        ret = usbHardwareEepromRead(ctx, eeprom, size, adress);
    }
    if(sfIsNetwork(ctx))
    {
        ret = netHardwareEepromRead(ctx, eeprom, size, adress);
    }
    return ret;
}
SCOPEFUN_API int sfHardwareEepromReadFirmwareID(SFContext* ctx, SEeprom* eeprom, int size, int adress)
{
   int ret = 0;
   if (sfIsUsb(ctx))
   {
      ret = usbHardwareEepromReadFirmwareID(ctx, eeprom, size, adress);
   }
   if (sfIsNetwork(ctx))
   {
      ret = netHardwareEepromReadFirmwareID(ctx, eeprom, size, adress);
   }
   return ret;
}
SCOPEFUN_API int sfHardwareEepromWrite(SFContext* ctx, SEeprom* eeprom, int size, int adress)
{
    int ret = 0;
    if(sfIsUsb(ctx))
    {
        ret = usbHardwareEepromWrite(ctx, eeprom, size, adress);
    }
    if(sfIsNetwork(ctx))
    {
        ret = netHardwareEepromWrite(ctx, eeprom, size, adress);
    }
    return ret;
}
SCOPEFUN_API int sfHardwareEepromErase(SFContext* ctx)
{
    int ret = 0;
    if(sfIsUsb(ctx))
    {
        ret = usbHardwareEepromErase(ctx);
    }
    if(sfIsNetwork(ctx))
    {
        ret = netHardwareEepromErase(ctx);
    }
    return ret;
}
SCOPEFUN_API int sfHardwareClose(SFContext* ctx)
{
    int ret = 0;
    if(sfIsUsb(ctx))
    {
        ret = usbHardwareClose(ctx);
    }
    if(sfIsNetwork(ctx))
    {
        ret = netHardwareClose(ctx);
    }
    return ret;
}

/*--------------------------------------------------------------------

   simulate

---------------------------------------------------------------------*/

#define PI           3.14159265358979323846f
#define NUM_SAMPLES  10000
#define NUM_SAMPLESF 10000.f
#define MAXOSCVALUE  511.f

float clamp(float a, float min, float max)
{
    if(a < min)
    {
        return min;
    }
    if(a > max)
    {
        return max;
    }
    return a;
}

float rand_FloatRange(float a, float b)
{
    return (float)((b - a) * ((float)rand() / RAND_MAX)) + a;
}

int softwareGenerator1(int frameVersion, int frameHeader, int frameData, int framePacket, SFContext* ctx, SSimulate* sim, double timer)
{
    ctx->frame.received = 0;
    // header, channel0, channel1 and digital bits
    byte* packet = &ctx->frame.data->data.bytes[0];
    uint  numSamples  = (uint)frameData / 6;
    float numSamplesF = (float)numSamples;
    // header, channel0, channel1 and digital bits
    byte*       header = packet;
    ushort*   channel0 = (ushort*)(packet + frameHeader + 0);
    ushort*   channel1 = (ushort*)(packet + frameHeader + 2);
    ushort*   digital = (ushort*)(packet + frameHeader + 4);
    // header
    SDL_memset(header, 0, frameHeader);
    // magic
    header[0] = 0xDD;
    header[1] = 0xDD;
    // frameSize
    uint frameSize = numSamples * 6 + frameHeader;
    header[frameHeader - 5] = (frameSize >> 0) & 0xff;
    header[frameHeader - 4] = (frameSize >> 8) & 0xff;
    header[frameHeader - 3] = (frameSize >> 16) & 0xff;
    header[frameHeader - 2] = (frameSize >> 24) & 0xff;
    // ets
    uint ets = rand() % 32;
    header[sim->etsIndex] = ets;
    if(sim->etsActive > 0)
    {
        timer = 0;
    }
    // crc
    ularge crc = 0;
    for(int i = 0; i < frameHeader - 1; i++)
    {
        crc += header[i];
    }
    byte byteCRC = (crc % frameHeader) & 0xFF;
    header[frameHeader - 1] = byteCRC;
    // analog
    for(int i = 0; i < 2; i++)
    {
        // capture
        float captureTime = sim->time * numSamplesF;
        float captureVolt = 0;
        if(i == 0)
        {
            captureVolt = sim->voltage0;
        }
        if(i == 1)
        {
            captureVolt = sim->voltage1;
        }
        // generateTime
        float generateTime = 0;
        if(i == 0)
        {
            generateTime = (float)timer * sim->speed0;
        }
        if(i == 1)
        {
            generateTime = (float)timer * sim->speed1;
        }
        // peakToPeak
        float peakToPeak = 0;
        if(i == 0)
        {
            peakToPeak = sim->peakToPeak0;
        }
        if(i == 1)
        {
            peakToPeak = sim->peakToPeak1;
        }
        // peakToPeak
        ESimulateType type = stSin;
        if(i == 0)
        {
            type = sim->type0;
        }
        if(i == 1)
        {
            type = sim->type1;
        }
        // period
        float period = 0;
        if(i == 0)
        {
            period = sim->period0;
        }
        if(i == 1)
        {
            period = sim->period1;
        }
        // min/max peak
        float minpeak = -peakToPeak / 2.f;
        float maxpeak = peakToPeak / 2.f;
        // frame size
        float displayFrameSize = numSamplesF;
        // period
        period = period * numSamplesF / NUM_SAMPLESF;
        // generate
        srand((uint)SDL_GetPerformanceCounter());
        float value = 0;
        for(uint j = 0; j < numSamples; j++)
        {
            float sample = (float)j;
            switch(type)
            {
                case stSin:
                case stCos:
                    {
                        float  normalizedTime = sample / displayFrameSize + generateTime;
                        float  signaltime = (normalizedTime * captureTime);
                        float            t = signaltime / period;
                        float    angle2Pi = 2.f * PI * t;
                        switch(type)
                        {
                            case stSin:
                                value = maxpeak * sinf(angle2Pi);
                                break;
                            case stCos:
                                value = maxpeak * cosf(angle2Pi);
                                break;
                            default:
                                break;
                        };
                    }
                    break;
                case stDec:
                case stInc:
                    {
                        float  normalizedTime = sample / displayFrameSize + generateTime;
                        float  normalizedPeriod = period / captureTime;
                        float mod = fmodf(normalizedTime, normalizedPeriod);
                        float div = mod / normalizedPeriod;
                        switch(type)
                        {
                            case stInc:
                                value = div * peakToPeak + minpeak;
                                break;
                            case stDec:
                                value = (1.f - div) * peakToPeak + minpeak;
                                break;
                            default:
                                break;
                        };
                    }
                    break;
                case stConstant:
                    value = peakToPeak;
                    break;
                case stRandom:
                    value = rand_FloatRange(-1.f, 1.f) * (peakToPeak / 2.f);
                    break;
                case stSquare:
                    {
                        float  normalizedTime = sample / displayFrameSize + generateTime;
                        float  normalizedPeriod = period / captureTime;
                        float  time = fmodf(normalizedTime, 1.0f);
                        if(time < normalizedPeriod)
                        {
                            value = peakToPeak;
                        }
                        else
                        {
                            value = 0.f;
                        }
                    }
                    break;
                case stDelta:
                    {
                        float  normalizedTime = sample / displayFrameSize + generateTime;
                        float  normalizedPeriod = period / captureTime;
                        float  time = fmodf(normalizedTime, 1.0f);
                        float      t = time / normalizedPeriod / 2.f;
                        if(time < normalizedPeriod / 2.f)
                        {
                            value = 4.0f * t * peakToPeak;
                        }
                        else if(time < normalizedPeriod)
                        {
                            value = 4.0f * (0.5f - t) * peakToPeak;
                        }
                        else
                        {
                            value = 0.f;
                        }
                    }
                    break;
            };
            float normalized = value / (5.f * captureVolt);
            normalized = clamp(normalized, -1.0f, 1.0f);
            ishort val = (ishort)(normalized * MAXOSCVALUE);
            if(i == 0)
            {
                channel0[j * 3] = val;
            }
            if(i == 1)
            {
                channel1[j * 3] = val;
            }
        }
    }
    // digital
    srand((uint)SDL_GetPerformanceCounter());
    for(uint i = 0; i < numSamples; i++)
    {
        byte digital0 = rand() % 2;
        byte digital1 = rand() % 2;
        byte digital2 = rand() % 2;
        byte digital3 = rand() % 2;
        byte digital4 = rand() % 2;
        byte digital5 = rand() % 2;
        byte digital6 = rand() % 2;
        byte digital7 = rand() % 2;
        byte digital8 = rand() % 2;
        byte digital9 = rand() % 2;
        byte digital10 = rand() % 2;
        byte digital11 = rand() % 2;
        byte digital12 = rand() % 2;
        byte digital13 = rand() % 2;
        byte digital14 = rand() % 2;
        byte digital15 = rand() % 2;
        ushort bits = digital0 | (digital1 << 1) | (digital2 << 2) | (digital3 << 3) | (digital4 << 4) | (digital5 << 5) | (digital6 << 6) | (digital7 << 7);
        bits |= digital8 << 8 | (digital9 << 9) | (digital10 << 10) | (digital11 << 11) | (digital12 << 12) | (digital13 << 13) | (digital14 << 14) | (digital15 << 15);
        digital[i * 3] = bits;
    }
    return 0;
}

int softwareGenerator2(int frameVersion, int frameHeader, int frameData, int framePacket, SFContext* ctx, SSimulate* sim, double timer)
{
    ctx->frame.received = 0;
    // header, channel0, channel1 and digital bits
    byte* packet = &ctx->frame.data->data.bytes[0];
    SFrameHeader2* header = (SFrameHeader2*)packet;
    uint  numSamples  = (uint)frameData / 4;
    float numSamplesF = (float)numSamples;
    // header
    SDL_memset(header, 0, frameHeader);
    // magic
    header->magic.bytes[0] = 0xDD;
    header->magic.bytes[1] = 0xDD;
    header->magic.bytes[2] = 0xDD;
    header->magic.bytes[3] = 0xDD;
    // frameSize
    header->hardware.bytes[32 + 0] = (numSamples >> 24) & 0xff;
    header->hardware.bytes[32 + 1] = (numSamples >> 16) & 0xff;
    header->hardware.bytes[32 + 2] = (numSamples >> 8) & 0xff;
    header->hardware.bytes[32 + 3] = (numSamples >> 0) & 0xff;
    // ets
    header->etsDelay.bytes[0] = (byte)(rand() % 32);
    if(sim->etsActive > 0)
    {
        timer = 0;
    }
    // crc
    header->crc.bytes[0] = 0;
    // analog
    for(int i = 0; i < 2; i++)
    {
        // capture
        float captureTime = sim->time * numSamplesF;
        float captureVolt = 0;
        if(i == 0)
        {
            captureVolt = sim->voltage0;
        }
        if(i == 1)
        {
            captureVolt = sim->voltage1;
        }
        // generateTime
        float generateTime = 0;
        if(i == 0)
        {
            generateTime = (float)timer * sim->speed0;
        }
        if(i == 1)
        {
            generateTime = (float)timer * sim->speed1;
        }
        // peakToPeak
        float peakToPeak = 0;
        if(i == 0)
        {
            peakToPeak = sim->peakToPeak0;
        }
        if(i == 1)
        {
            peakToPeak = sim->peakToPeak1;
        }
        // peakToPeak
        ESimulateType type = stSin;
        if(i == 0)
        {
            type = sim->type0;
        }
        if(i == 1)
        {
            type = sim->type1;
        }
        // period
        float period = 0;
        if(i == 0)
        {
            period = sim->period0;
        }
        if(i == 1)
        {
            period = sim->period1;
        }
        // min/max peak
        float minpeak = -peakToPeak / 2.f;
        float maxpeak = peakToPeak / 2.f;
        // frame size
        float displayFrameSize = numSamplesF;
        // period
        period = period * numSamplesF / NUM_SAMPLESF;
        // generate
        srand((uint)SDL_GetPerformanceCounter());
        float value = 0;
        for(uint j = 0; j < numSamples; j++)
        {
            float sample = (float)j;
            switch(type)
            {
                case stSin:
                case stCos:
                    {
                        float  normalizedTime = sample / displayFrameSize + generateTime;
                        float  signaltime = (normalizedTime * captureTime);
                        float            t = (signaltime / period);
                        float    angle2Pi = 2.f * PI * t;
                        switch(type)
                        {
                            case stSin:
                                value = maxpeak * sinf(angle2Pi);
                                break;
                            case stCos:
                                value = maxpeak * cosf(angle2Pi);
                                break;
                            default:
                                break;
                        };
                    }
                    break;
                case stDec:
                case stInc:
                    {
                        float  normalizedTime = sample / displayFrameSize + generateTime;
                        float  normalizedPeriod = period / captureTime;
                        float mod = fmodf(normalizedTime, normalizedPeriod);
                        float div = mod / (normalizedPeriod);
                        switch(type)
                        {
                            case stInc:
                                value = div * peakToPeak + minpeak;
                                break;
                            case stDec:
                                value = (1.f - div) * peakToPeak + minpeak;
                                break;
                            default:
                                break;
                        };
                    }
                    break;
                case stConstant:
                    value = peakToPeak;
                    break;
                case stRandom:
                    value = rand_FloatRange(-1.f, 1.f) * (peakToPeak / 2.f);
                    break;
                case stSquare:
                    {
                        float  normalizedTime = sample / displayFrameSize + generateTime;
                        float  normalizedPeriod = period / captureTime;
                        float  time = fmodf(normalizedTime, 1.0f);
                        if(time < normalizedPeriod)
                        {
                            value = peakToPeak;
                        }
                        else
                        {
                            value = 0.f;
                        }
                    }
                    break;
                case stDelta:
                    {
                        float  normalizedTime = sample / displayFrameSize + generateTime;
                        float  normalizedPeriod = period / captureTime;
                        float  time = fmodf(normalizedTime, 1.0f);
                        float      t = time / normalizedPeriod / 2.f;
                        if(time < normalizedPeriod / 2.f)
                        {
                            value = 4.0f * t * peakToPeak;
                        }
                        else if(time < normalizedPeriod)
                        {
                            value = 4.0f * (0.5f - t) * peakToPeak;
                        }
                        else
                        {
                            value = 0.f;
                        }
                    }
                    break;
            };
            float normalized = value / (5.f * captureVolt);
            normalized = clamp(normalized, -1.0f, 1.0f);
            ishort ival = (ishort)(normalized * MAXOSCVALUE);
            byte*  data = packet + frameHeader + j * 4;
            byte* byte0 = (byte*)(data + 0);
            byte* byte1 = (byte*)(data + 1);
            byte* byte2 = (byte*)(data + 2);
            byte* byte3 = (byte*)(data + 3);
            uint val = ival & 0x1FF;
            if(ival < 0)
            {
                val |= 0x200;
            }
            if(i == 0)
            {
                ishort shifted0 = (val >> 2);
                *byte0 = *byte0 | (byte)(shifted0);
                *byte1 = *byte1 | (byte)(val & 0x3);
                *byte1 = (*byte1 << 6);
            }
            if(i == 1)
            {
                ishort shifted1 = (val >> 4);
                *byte1 = *byte1 | (byte)(shifted1);
                *byte2 = *byte2 | (byte)(val & 0xF);
                *byte2 = (*byte2 << 4);
            }
        }
    }
    // digital
    srand((uint)SDL_GetPerformanceCounter());
    for(uint i = 0; i < numSamples; i++)
    {
        byte digital0 = rand() % 2;
        byte digital1 = rand() % 2;
        byte digital2 = rand() % 2;
        byte digital3 = rand() % 2;
        byte digital4 = rand() % 2;
        byte digital5 = rand() % 2;
        byte digital6 = rand() % 2;
        byte digital7 = rand() % 2;
        byte digital8 = rand() % 2;
        byte digital9 = rand()  % 2;
        byte digital10 = rand() % 2;
        byte digital11 = rand() % 2;
        byte digital12 = rand() % 2;
        byte digital13 = rand() % 2;
        byte digital14 = rand() % 2;
        byte digital15 = rand() % 2;
        ushort bits = digital0 | (digital1 << 1) | (digital2 << 2) | (digital3 << 3) | (digital4 << 4) | (digital5 << 5) | (digital6 << 6) | (digital7 << 7);
        bits |= digital8 << 8 | (digital9 << 9) | (digital10 << 10) | (digital11 << 11) | (digital12 << 12) | (digital13 << 13) | (digital14 << 14) | (digital15 << 15);
        byte* byte0 = (byte*)(packet + frameHeader + i * 4 + 0);
        byte* byte1 = (byte*)(packet + frameHeader + i * 4 + 1);
        byte* byte2 = (byte*)(packet + frameHeader + i * 4 + 2);
        byte* byte3 = (byte*)(packet + frameHeader + i * 4 + 3);
        *byte2 = *byte2 | (byte)(bits >> 12);
        *byte3 = *byte3 | (byte)(bits >> 4);
    }
    return 0;
}

int softwareGenerator(int frameVersion, int frameHeader, int frameData, int framePacket, SFContext* ctx, SSimulate* sim, double time)
{
    SDL_memset(&ctx->frame.data->data.bytes[0], 0, apiMin(sizeof(SFrameData), frameHeader + frameData));
    if(frameVersion == HARDWARE_VERSION_1)
    {
        return softwareGenerator1(frameVersion, frameHeader, frameData, framePacket, ctx, sim, time);
    }
    if(frameVersion == HARDWARE_VERSION_2)
    {
        return softwareGenerator2(frameVersion, frameHeader, frameData, framePacket, ctx, sim, time);
    }
    return 0;
}

SCOPEFUN_API int sfSimulate(SFContext* ctx, double time)
{
    apiLock(ctx);
    int simulate = SDL_AtomicGet((SDL_atomic_t*)&ctx->simulate.on);
    if(simulate > 0)
    {
        int version = ctx->frame.info.version;
        int header = ctx->frame.info.header;
        int data = ctx->frame.info.data;
        int packet = ctx->frame.info.packet;
        // generate
        softwareGenerator(version, header, data, packet, ctx, &ctx->simulate.data, time);
    }
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}
