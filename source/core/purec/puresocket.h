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
#ifndef __PUREC_SOCKET__
#define __PUREC_SOCKET__

#define PURESOCKET_SUCCESS  0
#define PURESOCKET_FAILURE -1

////////////////////////////////////////////////////////////////////////////////
// socket
////////////////////////////////////////////////////////////////////////////////
#if defined(PLATFORM_WIN) || defined(PLATFORM_MINGW)
    typedef unsigned int SOCKETFUN;
#else
    typedef int SOCKETFUN;
#endif

////////////////////////////////////////////////////////////////////////////////
// SocketContext
////////////////////////////////////////////////////////////////////////////////
struct SocketContext
{
    SOCKETFUN  socket;
    int        timeout;
};
typedef struct SocketContext SocketContext;

////////////////////////////////////////////////////////////////////////////////
// api
////////////////////////////////////////////////////////////////////////////////
int        socketInit();
int        socketCreate(SocketContext* ctx);
int        socketNonBlocking(SocketContext* ctx, int nonblock);
int        socketNoDelay(SocketContext* ctx);
int        socketBind(SocketContext* ctx, const char* serverip, int port);
int        socketListen(SocketContext* ctx, int backlog);
int        socketAccept(SocketContext* ctx, SocketContext* client);
int        socketConnect(SocketContext* ctx, const char* serverip, int port);
int        socketRecv(SocketContext* ctx, char* buf, int len, int flags, int* transfered);
int        socketSend(SocketContext* ctx, char* buf, int len, int flags, int* transfered);
int        socketClose(SocketContext* ctx);
int        socketShutDown(SocketContext* ctx);
int        socketExit();

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
////////////////////////////////////////////////////////////////////////////////
