////////////////////////////////////////////////////////////////////////////////
//    ScopeFun Oscilloscope ( http://www.scopefun.com )
//    Copyright (C) 2016 David Koöenina
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
#include<core/purec/puresocket.h>

#include<fcntl.h>
#include<errno.h>

#if defined(PLATFORM_WIN) || defined(PLATFORM_MINGW)
    #define _WINSOCK_DEPRECATED_NO_WARNINGS
    #define _WIN32_WINNT 0x0501
    #include <winsock2.h>
    #include <Ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
#endif

#if defined(PLATFORM_LINUX) || defined(PLATFORM_MAC)
    #include <netinet/tcp.h>
#endif

int tcpIpTextToBin(struct addrinfo* output, const char* ip, int port)
{
    struct addrinfo hints;
    struct addrinfo* result = NULL;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = AF_INET;     /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* stream socket */
    hints.ai_flags    = 0;
    hints.ai_protocol = IPPROTO_TCP; /* TCP protocol */
    char buffer[1024] = {0};
    cItoA(port, buffer, 10);
    int ret = getaddrinfo(ip, buffer, &hints, &result);
    // output first
    if(result)
    {
        *output = *result;
    }
    // free
    freeaddrinfo(result);
    return 0;
}


int socketInit()
{
    #if defined(PLATFORM_WIN) || defined(PLATFORM_MINGW)
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(2, 2), &wsa_data);
    return PURESOCKET_SUCCESS;
    #else
    return PURESOCKET_SUCCESS;
    #endif
}

int socketExit()
{
    #if defined(PLATFORM_WIN) || defined(PLATFORM_MINGW)
    int ret = WSACleanup();
    return PURESOCKET_SUCCESS;
    #else
    return PURESOCKET_SUCCESS;
    #endif
}

int socketCreate(SocketContext* ctx)
{
    SOCKETFUN s = (SOCKETFUN)socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    ctx->socket = s;
    int ret = socketNoDelay(ctx);
    return PURESOCKET_SUCCESS;
}

int socketNonBlocking(SocketContext* ctx, int nonblock)
{
    return PURESOCKET_SUCCESS;
    #if defined(PLATFORM_WIN) || defined(PLATFORM_MINGW)
    int arg = nonblock;
    int ret = ioctlsocket(ctx->socket, FIONBIO, &arg);
    #else
    int flags = fcntl(ctx->socket, F_GETFL, 0);
    if(flags < 0)
    {
        return 1;
    }
    if(nonblock)
    {
        flags = flags |  O_NONBLOCK;
    }
    else
    {
        flags = flags & ~O_NONBLOCK;
    }
    int ret = fcntl(ctx->socket, F_SETFL, flags);
    #endif
    return PURESOCKET_SUCCESS;
}

int socketNoDelay(SocketContext* ctx)
{
    return PURESOCKET_SUCCESS;
    int flag = 1;
    int ret = setsockopt(ctx->socket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));
    if(ret == 0)
    {
        return PURESOCKET_SUCCESS;
    }
    return PURESOCKET_FAILURE;
}

int socketBind(SocketContext* ctx, const char* serverip, int port)
{
    struct sockaddr_in serveraddr;
    cMemSet((char*)&serveraddr, 0, sizeof(struct sockaddr_in));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = inet_addr(serverip);
    int ret = bind(ctx->socket, (const struct sockaddr*)&serveraddr, sizeof(struct sockaddr_in));
    if(ret == 0)
    {
        return PURESOCKET_SUCCESS;
    }
    return PURESOCKET_FAILURE;
}

int socketListen(SocketContext* ctx, int backlog)
{
    int ret = listen(ctx->socket, backlog);
    if(ret == 0)
    {
        return PURESOCKET_SUCCESS;
    }
    return PURESOCKET_FAILURE;
}

int socketAccept(SocketContext* ctx, SocketContext* client)
{
    struct sockaddr_in addr;
    cMemSet((char*)&addr, 0, sizeof(struct sockaddr_in));
    socklen_t len = sizeof(addr);
    int ret = (int)accept(ctx->socket, (struct sockaddr*)&addr, &len);
    if(ret == -1)
    {
        return PURESOCKET_FAILURE;
    }
    client->socket = (SOCKETFUN)ret;
    return PURESOCKET_SUCCESS;
}

int socketConnect(SocketContext* ctx, const char* serverip, int port)
{
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(struct sockaddr_in));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port   = htons(port);
    serveraddr.sin_addr.s_addr = inet_addr(serverip);
    int ret = connect(ctx->socket, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if(ret == 0)
    {
        return PURESOCKET_SUCCESS;
    }
    else
    {
        int       error = errno;
        const char* str = strerror(error);
        return PURESOCKET_FAILURE;
    }
}

int socketRecv(SocketContext* ctx, char* buf, int len, int flags, int* transfered)
{
    struct timeval tv;
    tv.tv_sec  = 0;
    tv.tv_usec = ctx->timeout * 1000;
    setsockopt(ctx->socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval));
    int total = 0;
    *transfered = 0;
    while(total < len)
    {
        int ret = recv(ctx->socket, buf + total, len - total, 0);
        if(ret == -1)
        {
            *transfered = total;
            return PURESOCKET_FAILURE;
        }
        total += ret;
    };
    *transfered = total;
    return PURESOCKET_SUCCESS;
}

int socketSend(SocketContext* ctx, char* buf, int len, int flags, int* transfered)
{
    struct timeval tv;
    tv.tv_sec  = 0;
    tv.tv_usec = ctx->timeout * 1000;
    setsockopt(ctx->socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&tv, sizeof(struct timeval));
    int total = 0;
    int left  = len;
    *transfered = 0;
    while(total < len)
    {
        int ret = (int)send(ctx->socket, buf + total, left, flags);
        if(ret == -1)
        {
            return -1;
        }
        total += ret;
        left  -= ret;
    };
    *transfered = total;
    return PURESOCKET_SUCCESS;
}

int socketClose(SocketContext* ctx)
{
    #if defined(PLATFORM_WIN) || defined(PLATFORM_MINGW)
    int ret = (int)closesocket(ctx->socket);
    #else
    int ret = (int)close(ctx->socket);
    #endif
    return PURESOCKET_SUCCESS;
}

int socketShutDown(SocketContext* ctx)
{
    int status = 0;
    #if defined(PLATFORM_WIN) || defined(PLATFORM_MINGW)
    status = shutdown(ctx->socket, SD_BOTH);
    if(status == 0)
    {
        status = closesocket(ctx->socket);
    }
    #else
    status = shutdown(ctx->socket, SHUT_RDWR);
    if(status == 0)
    {
        status = close(ctx->socket);
    }
    #endif
    return PURESOCKET_SUCCESS;
}
