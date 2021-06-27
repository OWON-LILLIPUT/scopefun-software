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
#include<core/core.h>
#include<server/servermanager.h>

////////////////////////////////////////////////////////////////////
// capture thread
////////////////////////////////////////////////////////////////////

uint usbFrameSize(byte* header, int version)
{
    SFrameHeader1*    header1 = (SFrameHeader1*)header;
    SFrameHeader2*    header2 = (SFrameHeader2*)header;
    uint frameSize = 0;
    if(version == 2)
    {
        uint sampleSize = (uint(header2->hardware.bytes[32 + 0]) << 24) | (uint(header2->hardware.bytes[32 + 1]) << 16) | (uint(header2->hardware.bytes[32 + 2]) << 8) | (uint(header2->hardware.bytes[32 + 3]) << 0);
        frameSize = sampleSize * 4;
        if(frameSize % 1024 != 0)
        {
            frameSize = ((frameSize / 1024) + 1) * 1024;
        }
        frameSize += SCOPEFUN_FRAME_2_HEADER;
    }
    if(version == 1)
    {
        frameSize = SCOPEFUN_FRAME_1_HEADER + SCOPEFUN_FRAME_1_DATA;
    }
    return frameSize;
}

void errorMessage(const char* msg);

int SDLCALL CaptureThreadFunction(void* data)
{
    SDL_SetThreadPriority(SDL_THREAD_PRIORITY_HIGH);
    pTimer->init(TIMER_CAPTURE);
    double      timer  = 0;
    SDL_MemoryBarrierAcquire();
    while(SDL_AtomicGet(&pServer->serverThreadActive)>0)
    {
        SDL_Delay(1);
        pTimer->deltaTime(TIMER_CAPTURE);
        timer += pTimer->getDelta(TIMER_CAPTURE);
        int opened     = 0;
        int uploaded   = SDL_AtomicGet(&pServer->firmwareUploaded);
        int configured = SDL_AtomicGet(&pServer->firmwareConfigured);
        int simulation = sfIsSimulate(&pServer->ctx);
        int ret = sfHardwareIsOpened(&pServer->ctx, &opened);
        if((opened && uploaded && configured) || simulation)
        {
            Array<ScopeFunClient*,SCOPEFUN_MAX_CLIENT> request;

            // capture frame ?
            bool requestFrame = false;
            while(!requestFrame)
            {
                Array<ScopeFunClient*,SCOPEFUN_MAX_CLIENT> active;
                for(int i = 0; i < pServer->client.getCount(); i++)
                {
                    ScopeFunClient* pClient = pServer->client[i];
                    if(SDL_AtomicGet(&pClient->active) > 0)
                    {
                        active.pushBack(pClient);
                    }
                }
                Array<ScopeFunClient*,SCOPEFUN_MAX_CLIENT> requestHeader;
                for(int i = 0; i < active.getCount(); i++)
                {
                    ScopeFunClient* pClient = active[i];
                    if( SDL_AtomicGet(&pClient->captureType) == SCOPEFUN_CAPTURE_TYPE_HEADER )
                    {
                        requestHeader.pushBack(pClient);
                    }
                }
                Array<ScopeFunClient*,SCOPEFUN_MAX_CLIENT> requestData;
                for(int i = 0; i < active.getCount(); i++)
                {
                    ScopeFunClient* pClient = active[i];
                    if( SDL_AtomicGet(&pClient->captureType) == SCOPEFUN_CAPTURE_TYPE_DATA )
                    {
                        requestData.pushBack(pClient);
                    }
                }

                request.clear();
                if( active.getCount() > 0 )
                {
                    ScopeFunClient* pFirst = active[0];
                    if( SDL_AtomicGet(&pFirst->captureType) == SCOPEFUN_CAPTURE_TYPE_HEADER )
                    {
                        for(int i = 0; i < requestHeader.getCount(); i++)
                        {
                            request.pushBack(requestHeader[i]);
                        }
                    }
                    if( SDL_AtomicGet(&pFirst->captureType) == SCOPEFUN_CAPTURE_TYPE_DATA )
                    {
                        for(int i = 0; i < requestData.getCount(); i++)
                        {
                            request.pushBack(requestData[i]);
                        }
                    }
                }
                if(request.getCount() > 0)
                    requestFrame = true;
                else
                {
                    SDL_Delay(10);
                }
            }

            // max client memory
            ularge maxClientMemory = pServer->maxMemory;
            for(int i = 0; i < request.getCount(); i++)
            {
                ScopeFunClient* pClient = request[i];
                maxClientMemory = min<ularge>(maxClientMemory,pClient->maxMemory);
            }

            // bytesToReceive
            ularge bytesToReceive = maxClientMemory;
            for(int i = 0; i < request.getCount(); i++)
            {
                ScopeFunClient* pClient = request[i];
                bytesToReceive = min<ularge>( bytesToReceive, SDL_AtomicGet(&pClient->bytes) );
            }

            // simulate ?
            if( bytesToReceive > 0 )
            {
                if(SDL_AtomicGet(&pServer->updateSimulation) > 0)
                {
                    sfSimulate(&pServer->ctx, timer);
                }
            }

            // just in case
            if( bytesToReceive == 0 )
            {
                // debug
                FORMAT_BUFFER();
                FORMAT("bytesToReceive was zero",0);
                errorMessage(formatBuffer);
                SDL_Delay(100);
                continue;
            }

            // usb
            int transfered = 0;
            int ret = sfHardwareCapture(&pServer->ctx, (SFrameData*)pServer->captureBuffer, bytesToReceive, &transfered, SCOPEFUN_CAPTURE_TYPE_NONE);

            // debug
            FORMAT_BUFFER();
            FORMAT("sfHardwareCapture: toReceive %d transfered %d",bytesToReceive, transfered);
            errorMessage(formatBuffer);

            // copy buffers to send data over network
            for(int i = 0; i < request.getCount(); i++)
            {
                ScopeFunClient* pClient = request[i];

                // lock
                while(!pClient->sync.producerLock())
                {
                    SDL_Delay(1);
                }

                // copy frame
                if(transfered > 0)
                {
                    SDL_AtomicSet(&pClient->transfered, transfered);
                    SDL_memcpy((void*)&pClient->buffer->data.bytes[0], (void*) & ((SFrameData*)pServer->captureBuffer)->data.bytes[0], transfered);
                }

                // captureType
                SDL_AtomicSet(&pClient->captureType, SCOPEFUN_CAPTURE_TYPE_NONE);

                // unlock
                while(!pClient->sync.producerUnlock())
                {
                    SDL_Delay(1);
                }
            }
        }
        else
        {
            SDL_Delay(1);
        }
    }
    SDL_MemoryBarrierRelease();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
