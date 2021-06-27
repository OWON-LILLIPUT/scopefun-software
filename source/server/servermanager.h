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

#define CLIENT_RECV_BUFFER (4*MEGABYTE + KILOBYTE)
#define CLIENT_SEND_BUFFER (256*KILOBYTE)

extern "C" {
#include<api/scopefunapi.h>
}

// timer
enum ETimer
{
    TIMER_CAPTURE,
    TIMER_SERVER,
};

// client
class ScopeFunClient
{
public:
    ularge                          maxMemory;
public:
    SocketContext                   socket;
public:
    char                            recv[CLIENT_RECV_BUFFER];
    char                            send[CLIENT_SEND_BUFFER];
public:
    SFrameData*                     buffer;
    SDL_atomic_t                    bytes;
    SDL_atomic_t                    transfered;
    SDL_atomic_t                    captureType;
public:
    ConsumerThreadLock              sync;
    SDL_atomic_t                    active;
    uint                            id;
    uint                            index;
    SDL_Thread*                     thread;
public:
    SDisplay                        display;
    SSimulate                       simulate;
    SFrameInfo                      frameInfo;
    SDL_atomic_t                    requestFlag;
    SDL_atomic_t                    requestState;
public:
    ScopeFunClient(uint maxMemory);
    ~ScopeFunClient();
public:
    void start(uint id, SocketContext socket);
    void stop(bool wait);
    bool waitToStop();
};

////////////////////////////////////////////////////////////////////////////////
//
// ServerManager
//
////////////////////////////////////////////////////////////////////////////////
class ServerManager : public Manager
{
public:
    ularge        maxClient;
    ularge        maxMemory;
public:
    SFContext          ctx;
    SocketContext      socket;
public:
    SFrameData*  captureBuffer;
public:
    SDL_atomic_t firmwareUploaded;
    SDL_atomic_t firmwareConfigured;
    SDL_atomic_t updateSimulation;
public:
    Array<ScopeFunClient*, SCOPEFUN_MAX_CLIENT> client;
public:
    SDL_SpinLock serverLockApi;
    SDL_SpinLock serverLockMsg;
    SDL_Thread*  serverThread;
    SDL_atomic_t serverThreadActive;
    String       ip;
    uint         port;
public:
    SDL_Thread*  usbThread;
    bool         usbThreadActive;
public:
    SUsb         settingsUsb;
    SSimulate    settingsSim;
    SHardware1   settingsHw1;
    SHardware2   settingsHw2;
public:
    ServerManager();
public:
    int startServer(const char* ip, int port);
    int stopServer();
public:
    int startCapture();
    int stopCapture();
public:
    int killClient(int id);
public:
    int init();
public:
    int allocate();
    int free();
public:
    int start();
    int update(float dt);
    int stop();
};

MANAGER_POINTER(Server);

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
