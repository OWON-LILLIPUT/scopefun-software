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
#include<server/server.h>

////////////////////////////////////////////////////////////////////////////////
// Globals
////////////////////////////////////////////////////////////////////////////////
MANAGER_REGISTER(Server);

////////////////////////////////////////////////////////////////////////////////
// capture
////////////////////////////////////////////////////////////////////////////////
int SDLCALL CaptureThreadFunction(void* data);

////////////////////////////////////////////////////////////////////////////////
// core system
////////////////////////////////////////////////////////////////////////////////

void create()
{
    MANAGER_CREATE(Manager);
    MANAGER_CREATE(Memory);
    MANAGER_CREATE(Format);
    MANAGER_CREATE(Timer);
    MANAGER_CREATE(Server);
}

void setup()
{
    // start
    pManager->addStart("Timer");
    pManager->addStart("Server");
    // update
    pManager->addUpdate("Timer");
    pManager->addUpdate("Server");
    // stop
    pManager->addStop("Timer");
    pManager->addStop("Server");
}

////////////////////////////////////////////////////////////////////////////////
// ScopeFunClient
////////////////////////////////////////////////////////////////////////////////
ScopeFunClient::ScopeFunClient(uint allocateBytes)
{
    maxMemory = allocateBytes;
    buffer = (SFrameData*)pMemory->allocate(allocateBytes);
    SDL_AtomicSet(&bytes, 0);
    SDL_AtomicSet(&active, 0);
    SDL_AtomicSet(&captureType, SCOPEFUN_CAPTURE_TYPE_NONE);
    id = SCOPEFUN_INVALID_CLIENT;
    index = 0;
    thread = 0;
    socket.socket  = 0;
    socket.timeout = 0;
}

ScopeFunClient::~ScopeFunClient()
{
    pMemory->free(buffer);
}

void ScopeFunClient::start(uint clientId, SocketContext s)
{
    SDL_memset(&simulate,  0, sizeof(SSimulate));
    SDL_memset(&display,   0, sizeof(SDisplay));
    SDL_memset(&frameInfo, 0, sizeof(SFrameInfo));
    SDL_AtomicSet(&active, 1);
    SDL_AtomicSet(&captureType, SCOPEFUN_CAPTURE_TYPE_NONE);
    id = clientId;
    socket = s;
    thread = (SDL_Thread*)createClient(this);
}

void ScopeFunClient::stop(bool wait)
{
    int status = 0;
    socketClose(&socket);
    SDL_AtomicSet(&active, 0);
    if(wait)
    {
        SDL_WaitThread(thread, &status);
    }
    else
    {
        SDL_DetachThread(thread);
    }
    thread = 0;
}

////////////////////////////////////////////////////////////////////////////////
// ServerManager
////////////////////////////////////////////////////////////////////////////////
ServerManager::ServerManager()
{
    init();
}

int ServerManager::startServer(const char* ip, int portNumber)
{
    if(SDL_AtomicGet(&serverThreadActive)==0)
    {
        ip   = ip;
        port = portNumber;
        SDL_AtomicSet(&serverThreadActive,1);
        serverThread = (SDL_Thread*)createServer(ip, port);
    }
    return 0;
}

int ServerManager::startCapture()
{
    if(!usbThreadActive)
    {
        usbThreadActive = true;
        usbThread = (SDL_Thread*)SDL_CreateThread(CaptureThreadFunction, "scopefun_capture", this);
    }
    return 0;
}

int ServerManager::stopCapture()
{
    if(usbThreadActive)
    {
        usbThreadActive = false;
        int status = 0;
        SDL_WaitThread(usbThread, &status);
        usbThread = 0;
    }
    return 0;
}

int ServerManager::killClient(int id)
{
    for(int i = 0; i < client.getCount(); i++)
    {
        ScopeFunClient* pClient = client[i];
        if(SDL_AtomicGet(&pClient->active) > 0 && pClient->id == id)
        {
            pClient->stop(false);
        }
    }
    return 0;
}

int ServerManager::stopServer()
{
    if(SDL_AtomicGet(&serverThreadActive)>0)
    {
        for(int i = 0; i < client.getCount(); i++)
        {
            if(SDL_AtomicGet(&client[i]->active) > 0)
            {
                client[i]->stop(false);
            }
        }
        SDL_AtomicSet(&serverThreadActive,0);
        serverThread = 0;
    }
    return 0;
}

int ServerManager::init()
{
    maxMemory = 16 * MEGABYTE;
    maxClient = SCOPEFUN_MAX_CLIENT;
    SDL_AtomicSet(&updateSimulation, 0);
    // server
    serverLockApi = 0;
    serverLockMsg = 0;
    serverThread = 0;
    SDL_AtomicSet(&serverThreadActive,0);
    // usb
    usbThreadActive = false;
    usbThread = 0;
    // settings
    SDL_memset(&settingsUsb, 0, sizeof(settingsUsb));
    SDL_memset(&settingsHw1, 0, sizeof(settingsHw1));
    SDL_memset(&settingsHw2, 0, sizeof(settingsHw2));
    SDL_memset(&settingsSim, 0, sizeof(settingsSim));
    settingsUsb.timeoutEp2 = 33;
    settingsUsb.timeoutEp4 = 33;
    settingsUsb.timeoutEp6 = 33;
    cMemSet((char*)&socket, 0, sizeof(SocketContext));
    cMemSet((char*)&ctx, 0, sizeof(SFContext));
    SDL_AtomicSet(&firmwareUploaded, 0);
    SDL_AtomicSet(&firmwareConfigured, 0);
    return 0;
}

int ServerManager::allocate()
{
    // capture buffer
    captureBuffer = (SFrameData*)pMemory->allocate(maxMemory);
    // clients
    for(uint i = 0; i < maxClient; i++)
    {
        ScopeFunClient* pClient = new ScopeFunClient(maxMemory);
        pClient->index = i;
        client.pushBack(pClient);
    }
    return 0;
}

int ServerManager::free()
{
    pMemory->free(captureBuffer);
    for(uint i = 0; i < maxClient; i++)
    {
        delete client[i];
    }
    client.clear();
    return 0;
}

int ServerManager::start()
{
    allocate();
    sfApiInit();
    sfApiCreateContext(&ctx, maxMemory);
    sfSetThreadSafe(&ctx, 1);
    sfSetActive(&ctx, 1);
    sfSetUsb(&ctx);
    sfSetTimeOut(&ctx, -1);
    startServer(ip.asChar(), port);
    startCapture();
    return 0;
}

int ServerManager::update(float dt)
{
    return 0;
}

int ServerManager::stop()
{
    stopServer();
    free();
    sfApiDeleteContext(&ctx);
    sfApiExit();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
