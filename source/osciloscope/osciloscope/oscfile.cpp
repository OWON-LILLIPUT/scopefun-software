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
#include<osciloscope/osciloscope.h>


void largeToJson(cJSON* json, const char* name, ularge large)
{
    FORMAT_BUFFER();
    uint low  = large & 0xffffffff;
    uint high = (large >> 32) & 0xffffffff;
    FORMAT("%sLow", name);
    cJSON_AddNumberToObject(json, formatBuffer, low);
    FORMAT("%sHigh", name);
    cJSON_AddNumberToObject(json, formatBuffer, high);
}

void jsonToLarge(cJSON* json, const char* name, ularge& large)
{
    FORMAT_BUFFER();
    FORMAT("%sLow", name);
    uint low  = cJSON_GetObjectItem(json, formatBuffer)->valueint;
    FORMAT("%sHigh", name);
    uint high = cJSON_GetObjectItem(json, formatBuffer)->valueint;
    large = low | (ularge(high) << 32);
}

////////////////////////////////////////////////////////////////////////////////
//
// Save
//
////////////////////////////////////////////////////////////////////////////////
class OscFileHeader
{
public:
    uint  historyStart;
    uint  historyCount;
    uint  historySize;
};

int _saveToFile(void* param)
{
    OscFileThread* fileThread = (OscFileThread*)param;
    pOsciloscope->window.progress.uiActive = 1;
    pOsciloscope->window.progress.uiRange  = 100;
    pOsciloscope->captureBuffer->history->save(fileThread->file.asChar(), (uint&)pOsciloscope->window.progress.uiValue, (uint&)pOsciloscope->window.progress.uiActive);
    pOsciloscope->window.progress.uiActive = 1;
    pOsciloscope->window.progress.uiRange = 100;
    pOsciloscope->window.progress.uiValue = 0;
    SDL_AtomicSet(&fileThread->atomic, 0);
    return 0;
}

int OsciloscopeManager::saveToFile(const char* file)
{
    fileThread.file = file;
    if(fileThread.file.posReverse(".osc") > 0)
    {
        SDL_AtomicSet(&fileThread.atomic, 1);
        fileThread.thread = SDL_CreateThread(_saveToFile, "OscFileSave", &fileThread);
        // _saveToFile(  &fileThread );
    }
    return 0;
}

int _convertToText(void* param)
{
    OscFileThread* fileThread = (OscFileThread*)param;
    pOsciloscope->window.progress.uiActive = 1;
    pOsciloscope->window.progress.uiRange = 100;
    int& textPercent = pOsciloscope->window.progress.uiText;
    int& progress = pOsciloscope->window.progress.uiValue;
    int& active = pOsciloscope->window.progress.uiActive;
    int frameHeader = 0;
    int frameData   = 0;
    int framePacket = 0;
    // header
    CaptureHeader header;
    SDL_RWops* ctxRead = SDL_RWFromFile(fileThread->file.asChar(), "rb");
    SDL_RWread(ctxRead, &header, sizeof(CaptureHeader), 1);
    // allocate frames
    Ring<CaptureFrame> ringFrame;
    CaptureFrame* framePtr = (CaptureFrame*)pMemory->allocate(header.frameSize * sizeof(CaptureFrame));
    ringFrame.init(framePtr, header.frameSize);
    ringFrame.setStart(header.frameStart);
    ringFrame.setCount(header.frameCount);
    // read frames
    for(int i = 0; i < header.frameSize; i++)
    {
        progress = (i * 100) / header.frameSize;
        active = 1;
        SDL_RWread(ctxRead, (void*)(framePtr + i), sizeof(CaptureFrame), 1);
    }
    // allocate packets
    Ring<PacketData> ringPacket;
    PacketData* packetPtr = (PacketData*)pMemory->allocate(header.packetSize * sizeof(PacketData));
    ringPacket.init(packetPtr, header.packetSize);
    ringPacket.setStart(header.packetStart);
    ringPacket.setCount(header.packetCount);
    // load packets
    for(int i = 0; i < header.packetSize; i++)
    {
        progress = (i * 100) / header.packetSize;
        active = 1;
        SDL_RWread(ctxRead, (void*)(packetPtr + i), sizeof(PacketData), 1);
    }
    // count, start
    ringFrame.setStart(header.frameStart);
    ringFrame.setCount(header.frameCount);
    ringPacket.setStart(header.packetStart);
    ringPacket.setCount(header.packetCount);
    // output
    String outputTextFile = fileThread->file;
    outputTextFile.replace(".osc", ".txt");
    SDL_RWops* ctxWrite = SDL_RWFromFile(outputTextFile.asChar(), "w+b");
    // frame
    byte*   dataPtr = (byte*)pMemory->allocate(SCOPEFUN_FRAME_MEMORY);
    CaptureFrame frame;
    PacketData   packet;
    FORMAT_BUFFER();
    uint frameId = 0;
    int frameCount = ringFrame.getCount();
    while(!ringFrame.isEmpty())
    {
        ringFrame.read(frame);
        textPercent = (frameId * 100) / frameCount;
        active = 1;
        SDL_memset(dataPtr, 0, SCOPEFUN_FRAME_MEMORY);
        uint offset = 0;
        for(uint i = 0; i < frame.packetCount; i++)
        {
            progress = (i * 100) / frame.packetCount;
            active = 1;
            ringPacket.read(packet);
            SDL_RWread(ctxRead, (void*)(dataPtr + offset), packet.size, 1);
            offset += packet.size;
        }
        // output to text file
        // header
        SFrameHeader1* frameHeader1 = (SFrameHeader1*)dataPtr;
        SFrameHeader2* frameHeader2 = (SFrameHeader2*)dataPtr;
        uint  sampleCount = 0;
        byte*   samplePtr = 0;
        if(frame.version == 2)
        {
            OsciloscopeControl2 ctr2;
            ctr2.client2Set(*(SHardware2*)&frameHeader2->hardware.bytes[0]);
            uint sampleSize = ctr2.getSampleSize();
            uint  frameSize = sampleSize * 4 + sizeof(SFrameHeader2);
            FORMAT("frameId=%d\r\nmagic=%x\r\nframeSize=%d\r\n", frameId++, frameHeader2->magic, frameSize);
            SDL_RWwrite(ctxWrite, formatBuffer, SDL_strlen(formatBuffer), 1);
            sampleCount = (frame.frameSize - SCOPEFUN_FRAME_2_HEADER) / 6;
            samplePtr   = (byte*)dataPtr + SCOPEFUN_FRAME_2_HEADER;
        }
        if(frame.version == 1)
        {
            FORMAT("frameId=%d\r\nmagic=%x\r\nframeSize=%d\r\n", frameId++, frameHeader1->magic, frameHeader1->frameSize);
            SDL_RWwrite(ctxWrite, formatBuffer, SDL_strlen(formatBuffer), 1);
            sampleCount = (frame.frameSize - SCOPEFUN_FRAME_1_HEADER) / 6;
            samplePtr = (byte*)dataPtr + SCOPEFUN_FRAME_1_HEADER;
        }
        // todo: configuration
        // ch0
        for(int i = 0; i < 2; i++)
        {
            progress = (i * 100) / 2;
            active = 1;
            FORMAT("ch%d=", i);
            SDL_RWwrite(ctxWrite, formatBuffer, SDL_strlen(formatBuffer), 1);
            for(uint i = 0; i < sampleCount; i++)
            {
                uint offset = i * 6;
                ushort ch0 = *(ushort*)(samplePtr + offset + 0);
                ushort ch1 = *(ushort*)(samplePtr + offset + 2);
                ushort dig = *(ushort*)(samplePtr + offset + 4);
                FORMAT("%d", i == 0 ? ch0 : ch1);
                SDL_RWwrite(ctxWrite, formatBuffer, SDL_strlen(formatBuffer), 1);
                if(i < sampleCount - 1)
                {
                    FORMAT(",", 0);
                    SDL_RWwrite(ctxWrite, formatBuffer, SDL_strlen(formatBuffer), 1);
                }
            }
            FORMAT("\r\n", 0);
            SDL_RWwrite(ctxWrite, formatBuffer, SDL_strlen(formatBuffer), 1);
        }
        // dig
        for(int d = 0; d < 16; d++)
        {
            progress = (d * 100) / 16;
            active = 1;
            FORMAT("dig%d=", d);
            SDL_RWwrite(ctxWrite, formatBuffer, SDL_strlen(formatBuffer), 1);
            for(uint i = 0; i < sampleCount; i++)
            {
                uint offset = i * 6;
                ushort ch0 = *(ushort*)(samplePtr + offset + 0);
                ushort ch1 = *(ushort*)(samplePtr + offset + 2);
                ushort dig = *(ushort*)(samplePtr + offset + 4);
                FORMAT("%d", (dig >> i) & 0x1);
                SDL_RWwrite(ctxWrite, formatBuffer, SDL_strlen(formatBuffer), 1);
                if(i < sampleCount - 1)
                {
                    FORMAT(",", 0);
                    SDL_RWwrite(ctxWrite, formatBuffer, SDL_strlen(formatBuffer), 1);
                }
            }
            FORMAT("\r\n", 0);
            SDL_RWwrite(ctxWrite, formatBuffer, SDL_strlen(formatBuffer), 1);
        }
    }
    // close
    SDL_RWclose(ctxRead);
    SDL_RWclose(ctxWrite);
    pMemory->free(dataPtr);
    pOsciloscope->window.progress.uiActive = 1;
    pOsciloscope->window.progress.uiRange  = 100;
    pOsciloscope->window.progress.uiValue  = 0;
    pOsciloscope->window.progress.uiText   = 0;
    SDL_AtomicSet(&fileThread->atomic, 0);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// Load
//
////////////////////////////////////////////////////////////////////////////////
int _loadFromFile(void* param)
{
    OscFileThread* fileThread = (OscFileThread*)param;
    pOsciloscope->window.progress.uiActive = 1;
    pOsciloscope->window.progress.uiRange = 100;
    pOsciloscope->captureBuffer->history->load(fileThread->file.asChar(), (uint&)pOsciloscope->window.progress.uiValue, (uint&)pOsciloscope->window.progress.uiActive);
    pOsciloscope->window.progress.uiActive = 1;
    pOsciloscope->window.progress.uiRange = 100;
    pOsciloscope->window.progress.uiValue = 0;
    SDL_AtomicSet(&fileThread->atomic, 0);
    return 0;
}

int OsciloscopeManager::loadFromFile(const char* file)
{
    fileThread.file = file;
    if(fileThread.file.posReverse(".osc") > 0)
    {
        SDL_AtomicSet(&fileThread.atomic, 1);
        fileThread.thread = SDL_CreateThread(_loadFromFile, "OscFileLoad", &fileThread);
    }
    return 0;
}

int OsciloscopeManager::convertToText(const char* file)
{
    fileThread.file = file;
    if(fileThread.file.posReverse(".osc") > 0)
    {
        SDL_AtomicSet(&fileThread.atomic, 1);
        fileThread.thread = SDL_CreateThread(_convertToText, "OscConvertToText", &fileThread);
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
