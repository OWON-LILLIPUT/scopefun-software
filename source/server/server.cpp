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
#include <server/servermanager.h>

extern "C" {
#include <api/scopefunapi.h>
#include <core/purec/puresocket.h>
}

void* getListBox1();
void* getListBox2();
void  msgListBoxAdd(void* list, const char* msg);
void  msgListBoxRemove(void* list, const char* msg);
void  msgListBoxClear(void* list);

SDL_SpinLock lock = 0;

void* createRunner();

int SDLCALL ScopeFunServer(void* data)
{
    SDL_SetThreadPriority(SDL_THREAD_PRIORITY_LOW);
    SDL_AtomicUnlock(&lock);
    SocketContext& serverSocket = pServer->socket;
    socketCreate(&serverSocket);
    socketNoDelay(&serverSocket);
    socketNonBlocking(&serverSocket, 1);
    socketBind(&serverSocket, pServer->ip.asChar(), pServer->port);
    int id = 0;
    while(SDL_AtomicGet(&pServer->serverThreadActive)>0)
    {
        int listen = socketListen(&serverSocket, SCOPEFUN_MAX_CLIENT);
        SocketContext clientSocket = { 0 };
        int ret = socketAccept(&serverSocket, &clientSocket);
        if(ret == PURESOCKET_SUCCESS && clientSocket.socket != 0)
        {
            for(int i = 0; i < pServer->client.getCount(); i++)
            {
                if(SDL_AtomicGet(&pServer->client[i]->active) == 0)
                {
                    // start client
                    socketNoDelay(&clientSocket);
                    socketNonBlocking(&clientSocket, 0);
                    pServer->client[i]->start(id, clientSocket);
                    id++;
                    break;
                }
            }
        }
        SDL_Delay(1000);
    }
    socketClose(&serverSocket);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// static assert
////////////////////////////////////////////////////////////////////////////////
template <bool b>
struct StaticCheck {};

template <>
struct StaticCheck<true>
{
    static void check() {};
};

template <>
struct StaticCheck<false>
{
    static void check()
    {
        SDL_assert_always(true);
    }
};

#define SERVER_RECV_HEADER \
    messageHeader* recvHeader = (messageHeader*)pClient->recv; \
    int server_recv_header = (int)socketRecv(&s, (char*)recvHeader, sizeof(messageHeader), 0, &transfered); \
    if (transfered == sizeof(messageHeader) ) transfered_size_ok = SCOPEFUN_SUCCESS; else transfered_size_ok = SCOPEFUN_FAILURE;

#define SERVER_RECV_MSG(structure) \
    structure* recvMessage = (structure*)pClient->recv; \
    int server_recv_msg = socketRecv(&s, (char*)recvMessage + sizeof(messageHeader), sizeof(structure) - sizeof(messageHeader), 0, &transfered); \
    if (transfered == (sizeof(structure) - sizeof(messageHeader)) ) transfered_size_ok = SCOPEFUN_SUCCESS; else transfered_size_ok = SCOPEFUN_FAILURE; \
    StaticCheck< sizeof(structure) < CLIENT_RECV_BUFFER > ::check();

#define SERVER_HEADER(structure,messageType) \
    structure* sendMessage = (structure*)pClient->send; \
    int server_header = serverMessageHeader((messageHeader*)sendMessage, messageType);

#define SERVER_SEND_MSG(structure) \
    int server_send_msg = socketSend(&s, (char*)sendMessage, sizeof(structure), 0, &transfered); \
    if (transfered == sizeof(structure) ) transfered_size_ok = SCOPEFUN_SUCCESS; else transfered_size_ok = SCOPEFUN_FAILURE;  \
    StaticCheck< sizeof(structure) < CLIENT_SEND_BUFFER > ::check();

#define SERVER_SEND_SIZE(structure,size) \
    int server_send_size = socketSend(&s, (char*)sendMessage, size, 0, &transfered); \
    if (transfered == size ) transfered_size_ok = SCOPEFUN_SUCCESS; else transfered_size_ok = SCOPEFUN_FAILURE;

#define SERVER_SEND_BUFFER(buffer,size) \
    int server_send_buffer = socketSend(&s, (char*)buffer, size, 0, &transfered); \
    if (transfered == size ) transfered_size_ok = SCOPEFUN_SUCCESS; else transfered_size_ok = SCOPEFUN_FAILURE;

void errorMessage(const char* msg)
{
    FORMAT_BUFFER();
    FORMAT(msg,0);
    msgListBoxAdd(getListBox1(), formatBuffer);
}


int SDLCALL ClientFunServer(void* data)
{
    try {

        // client
        ScopeFunClient* pClient = (ScopeFunClient*)data;
        // list box
        FORMAT_BUFFER();
        FORMAT("Client %d", pClient->id);
        msgListBoxAdd(getListBox2(), (const char*)formatBuffer);
        // loop
        double timer = 0.f;
        while(SDL_AtomicGet(&pClient->active) > 0)
        {
            int  transfered = 0;
            int  transfered_size_ok = 0;
            SocketContext& s = pClient->socket;
            // receive
            SERVER_RECV_HEADER;
            if(transfered == 0)
            {
                pTimer->deltaTime(pClient->index);
                timer += pTimer->getDelta(pClient->index);
                if(timer > 60.f)
                {
                    SDL_Delay(1000);
                }
                SDL_Delay(10);
                continue;
            }
            else
            {
                pTimer->deltaTime(pClient->index);
                timer = 0.f;
            }
            // check header
            int headerOk = isClientHeaderOk((messageHeader*)recvHeader);
            if(headerOk != SCOPEFUN_SUCCESS)
            {
                continue;
            }
            // header ok
            FORMAT("Client %d | %s ", pClient->id, (const char*)messageName((EMessage)recvHeader->message));
            msgListBoxAdd(getListBox1(), formatBuffer);
            /*------------------------------------------------------------------
                client

                   - mClientConnect
                   - mClientDisconnect

            ------------------------------------------------------------------*/
            if(recvHeader->message == mClientConnect)
            {
                SERVER_RECV_MSG(csClientConnect);
                if(server_recv_msg == SCOPEFUN_SUCCESS)
                {
                    SERVER_HEADER(scClientConnect, mClientConnect);
                    if(server_header == SCOPEFUN_SUCCESS)
                    {
                        sendMessage->id    = pClient->id;
                        pClient->maxMemory = min<ularge>(pClient->maxMemory, recvMessage->maxMemory);
                        SERVER_SEND_MSG(scClientConnect);
                    }
                }
                continue;
            }
            if(recvHeader->message == mClientDisconnect)
            {
                SERVER_RECV_MSG(csClientDisconnect);
                if(server_recv_msg == SCOPEFUN_SUCCESS)
                {
                    SERVER_HEADER(scClientDisconnect, mClientDisconnect);
                    if(server_header == SCOPEFUN_SUCCESS)
                    {
                        SERVER_SEND_MSG(scClientDisconnect);
                        if(server_send_msg == SCOPEFUN_SUCCESS)
                        {
                            pClient->stop(false);
                        }
                    }
                }
                continue;
            }
            if(recvHeader->message == mClientDisplay)
            {
                SERVER_RECV_MSG(csClientDisplay);
                if(server_recv_msg == SCOPEFUN_SUCCESS)
                {
                    SERVER_HEADER(scClientDisplay, mClientDisplay);
                    if(server_header == SCOPEFUN_SUCCESS)
                    {
                        ScopeFunClient* pDisplayClient = 0;
                        for(int i = 0; i < pServer->client.getCount(); i++)
                        {
                            if(recvMessage->clientId == pServer->client[i]->id)
                            {
                                pDisplayClient = pServer->client[i];
                                break;
                            }
                        }
                        if(pDisplayClient)
                        {
                            pDisplayClient->display = recvMessage->display;
                        }
                        SERVER_SEND_MSG(scUpload);
                    }
                }
                continue;
            }
            /*------------------------------------------------------------------
                server

                   - mUpload
                   - mDownload

            ------------------------------------------------------------------*/
            if(recvHeader->message == mUpload)
            {
                SERVER_RECV_MSG(csUpload);
                if(server_recv_msg == SCOPEFUN_SUCCESS)
                {
                    SERVER_HEADER(scUpload, mUpload);
                    if(server_header == SCOPEFUN_SUCCESS)
                    {
                        sfSetSimulateData(&pServer->ctx,   &recvMessage->simulate);
                        sfSetSimulateOnOff(&pServer->ctx,   recvMessage->simOnOff);
                        sfSetFrameVersion(&pServer->ctx,    recvMessage->frame.version);
                        sfSetFrameHeader(&pServer->ctx,     recvMessage->frame.header);
                        sfSetFrameData(&pServer->ctx,       recvMessage->frame.data);
                        sfSetFramePacket(&pServer->ctx,     recvMessage->frame.packet);
                        SERVER_SEND_MSG(scUpload);
                    }
                }
                continue;
            }
            if(recvHeader->message == mDownload)
            {
                SERVER_RECV_MSG(csDownload);
                if(server_recv_msg == SCOPEFUN_SUCCESS)
                {
                    SERVER_HEADER(scDownload, mDownload);
                    if(server_header == SCOPEFUN_SUCCESS)
                    {
                        sendMessage->active.cnt = pServer->client.getCount();
                        for(int i = 0; i < sendMessage->active.cnt; i++)
                        {
                            sendMessage->active.client.bytes[i] = pServer->client[i]->id;
                        }
                        sendMessage->display = pClient->display;
                        sendMessage->simOnOff = sfIsSimulate(&pServer->ctx);
                        sfGetSimulateData(&pServer->ctx,  &sendMessage->simulate);
                        sfGetFrameVersion(&pServer->ctx, (int*)&sendMessage->frame.version);
                        sfGetFrameData(&pServer->ctx, (int*)&sendMessage->frame.data);
                        sfGetFrameHeader(&pServer->ctx, (int*)&sendMessage->frame.header);
                        sfGetFramePacket(&pServer->ctx, (int*)&sendMessage->frame.packet);
                        SERVER_SEND_MSG(scDownload);
                    }
                }
                continue;
            }
            /*------------------------------------------------------------------
                hardware

                      - mHardwareOpen
                      - mHardwareIsOpened
                      - mHardwareReset
                      - mHardwareConfig1
                      - mHardwareConfig2
                      - mHardwareCapture
                      - mHardwareUploadFx2
                      - mHardwareUploadFpga
                      - mHardwareUploadGenerator
                      - mHardwareEepromRead
                      - mHardwareEepromWrite
                      - mHardwareEepromErase
                      - mHardwareClose

            ------------------------------------------------------------------*/
            if(recvHeader->message == mHardwareOpen)
            {
                SERVER_RECV_MSG(csHardwareOpen);
                if(server_recv_msg == SCOPEFUN_SUCCESS)
                {
                    SERVER_HEADER(scHardwareOpen, mHardwareOpen);
                    if(server_header == SCOPEFUN_SUCCESS)
                    {
                        sendMessage->header.error = sfHardwareOpen(&pServer->ctx, &recvMessage->usb, recvMessage->version);
                        SERVER_SEND_MSG(scHardwareOpen);
                    }
                }
                continue;
            }
            if(recvHeader->message == mHardwareIsOpened)
            {
                SERVER_RECV_MSG(csHardwareIsOpened);
                if(server_recv_msg == SCOPEFUN_SUCCESS)
                {
                    SERVER_HEADER(scHardwareIsOpened, mHardwareIsOpened);
                    if(server_header == SCOPEFUN_SUCCESS)
                    {
                        sendMessage->header.error = sfHardwareIsOpened(&pServer->ctx, (int*)&sendMessage->opened);
                        SERVER_SEND_MSG(scHardwareIsOpened);
                    }
                }
                continue;
            }
            if(recvHeader->message == mHardwareReset)
            {
                SERVER_RECV_MSG(csHardwareReset);
                if(server_recv_msg == SCOPEFUN_SUCCESS)
                {
                    SERVER_HEADER(scHardwareReset, mHardwareReset);
                    if(server_header == SCOPEFUN_SUCCESS)
                    {
                        sendMessage->header.error = sfHardwareReset(&pServer->ctx);
                        SERVER_SEND_MSG(scHardwareReset);
                    }
                }
                continue;
            }
            if(recvHeader->message == mHardwareConfig1)
            {
                SERVER_RECV_MSG(csHardwareConfig1);
                if(server_recv_msg == SCOPEFUN_SUCCESS)
                {
                    SERVER_HEADER(scHardwareConfig1, mHardwareConfig1);
                    if(server_header == SCOPEFUN_SUCCESS)
                    {
                        sendMessage->header.error = sfHardwareConfig1(&pServer->ctx, &recvMessage->config);
                        if(sendMessage->header.error == SCOPEFUN_SUCCESS)
                        {
                            SDL_AtomicSet(&pServer->firmwareConfigured, 1);
                        }
                        SERVER_SEND_MSG(scHardwareConfig1);
                    }
                }
                continue;
            }
            if(recvHeader->message == mHardwareConfig2)
            {
                SERVER_RECV_MSG(csHardwareConfig2);
                if(server_recv_msg == SCOPEFUN_SUCCESS)
                {
                    SERVER_HEADER(scHardwareConfig2, mHardwareConfig2);
                    if(server_header == SCOPEFUN_SUCCESS)
                    {
                        sendMessage->header.error = sfHardwareConfig2(&pServer->ctx, &recvMessage->config);
                        if(sendMessage->header.error == SCOPEFUN_SUCCESS)
                        {
                            SDL_AtomicSet(&pServer->firmwareConfigured, 1);
                        }
                        SERVER_SEND_MSG(scHardwareConfig2);
                    }
                }
                continue;
            }
            if(recvHeader->message == mHardwareCapture)
            {
                SERVER_RECV_MSG(csHardwareCapture);
                if(server_recv_msg == SCOPEFUN_SUCCESS)
                {
                    // acquire
                    SDL_MemoryBarrierAcquire();

                    // amount of data to capture
                    SDL_AtomicSet(&pClient->bytes,recvMessage->len);

                     // captureType
                    SDL_AtomicSet(&pClient->captureType, recvMessage->type);

                    // lock
                    while(!pClient->sync.consumerLock())
                    {
                        SDL_Delay(1);
                    }

                    // send data to client
                    SERVER_HEADER(scHardwareCapture, mHardwareCapture);
                    if(server_header == SCOPEFUN_SUCCESS)
                    {
                        // header
                        SERVER_SEND_MSG(messageHeader);
                        if(server_send_msg == SCOPEFUN_SUCCESS)
                        {
                            // amount of data to send
                            sendMessage->bytes = SDL_AtomicGet(&pClient->transfered);
                            SERVER_SEND_BUFFER(&sendMessage->bytes, sizeof(int));
                            if(server_send_buffer == SCOPEFUN_SUCCESS)
                            {
                                SERVER_SEND_BUFFER((byte*)&pClient->buffer->data.bytes[0], (sendMessage->bytes));
                            }
                            else
                            {
                                int debug = 1;
                                errorMessage( "error - 5" );
                            }
                        }
                        else
                        {
                            int debug = 1;
                            errorMessage( "error - 6" );
                        }
                    }
                    else
                    {
                        int debug = 1;
                        errorMessage( "error - 7" );
                    }

                    // amount of data to capture
                    SDL_AtomicSet(&pClient->bytes,0);

                    // unlock
                    while(!pClient->sync.consumerUnlock())
                    {
                        SDL_Delay(1);
                    }

                    // release
                    SDL_MemoryBarrierRelease();
                }
                continue;
            }
            if(recvHeader->message == mHardwareUploadFx2)
            {
                SERVER_RECV_MSG(csHardwareUploadFx2);
                if(server_recv_msg == SCOPEFUN_SUCCESS)
                {
                    SERVER_HEADER(scHardwareUploadFx2, mHardwareUploadFx2);
                    if(server_header == SCOPEFUN_SUCCESS)
                    {
                        sendMessage->header.error = sfHardwareUploadFx2(&pServer->ctx, &recvMessage->fx2);
                        SERVER_SEND_MSG(scHardwareUploadFx2);
                    }
                }
                continue;
            }
            if(recvHeader->message == mHardwareUploadFpga)
            {
                SERVER_RECV_MSG(csHardwareUploadFpga);
                if(server_recv_msg == SCOPEFUN_SUCCESS)
                {
                    SERVER_HEADER(scHardwareUploadFpga, mHardwareUploadFpga);
                    if(server_header == SCOPEFUN_SUCCESS)
                    {
                        sendMessage->header.error = sfHardwareUploadFpga(&pServer->ctx, &recvMessage->fpga);
                        if(sendMessage->header.error == SCOPEFUN_SUCCESS)
                        {
                            SDL_AtomicSet(&pServer->firmwareUploaded, 1);
                        }
                        SERVER_SEND_MSG(scHardwareUploadFpga);
                    }
                }
                continue;
            }
            if(recvHeader->message == mHardwareUploadGenerator)
            {
                SERVER_RECV_MSG(csHardwareUploadGenerator);
                if(server_recv_msg == SCOPEFUN_SUCCESS)
                {
                    SERVER_HEADER(scHardwareUploadGenerator, mHardwareUploadGenerator);
                    if(server_header == SCOPEFUN_SUCCESS)
                    {
                        sendMessage->header.error = sfHardwareUploadGenerator(&pServer->ctx, &recvMessage->data);
                        SERVER_SEND_MSG(scHardwareUploadGenerator);
                    }
                }
                continue;
            }
            if(recvHeader->message == mHardwareEepromRead)
            {
                SERVER_RECV_MSG(csHardwareEepromRead);
                if(server_recv_msg == SCOPEFUN_SUCCESS)
                {
                    SERVER_HEADER(scHardwareEepromRead, mHardwareEepromRead);
                    if(server_header == SCOPEFUN_SUCCESS)
                    {
                        sendMessage->header.error = sfHardwareEepromRead(&pServer->ctx, &sendMessage->eeprom, recvMessage->size, recvMessage->address);
                        SERVER_SEND_MSG(scHardwareEepromRead);
                    }
                }
                continue;
            }
            if (recvHeader->message == mHardwareEepromReadFirmwareID)
            {
               SERVER_RECV_MSG(csHardwareEepromReadFirmwareID);
               if (server_recv_msg == SCOPEFUN_SUCCESS)
               {
                  SERVER_HEADER(scHardwareEepromReadFirmwareID, mHardwareEepromReadFirmwareID);
                  if (server_header == SCOPEFUN_SUCCESS)
                  {
                     sendMessage->header.error = sfHardwareEepromReadFirmwareID(&pServer->ctx, &sendMessage->eeprom, recvMessage->size, recvMessage->address);
                     SERVER_SEND_MSG(scHardwareEepromReadFirmwareID);
                  }
               }
               continue;
            }
            if(recvHeader->message == mHardwareEepromWrite)
            {
                SERVER_RECV_MSG(csHardwareEepromWrite);
                if(server_recv_msg == SCOPEFUN_SUCCESS)
                {
                    SERVER_HEADER(scHardwareEepromWrite, mHardwareEepromWrite);
                    if(server_header == SCOPEFUN_SUCCESS)
                    {
                        sendMessage->header.error = sfHardwareEepromWrite(&pServer->ctx, &recvMessage->eeprom, recvMessage->size, recvMessage->address);
                        SERVER_SEND_MSG(scHardwareEepromWrite);
                    }
                }
                continue;
            }
            if(recvHeader->message == mHardwareEepromErase)
            {
                SERVER_RECV_MSG(csHardwareEepromErase);
                if(server_recv_msg == SCOPEFUN_SUCCESS)
                {
                    SERVER_HEADER(scHardwareEepromErase, mHardwareEepromErase);
                    if(server_header == SCOPEFUN_SUCCESS)
                    {
                        sendMessage->header.error = sfHardwareEepromErase(&pServer->ctx);
                        SERVER_SEND_MSG(scHardwareEepromErase);
                    }
                }
                continue;
            }
            if(recvHeader->message == mHardwareClose)
            {
                SERVER_RECV_MSG(csHardwareClose);
                if(server_recv_msg == SCOPEFUN_SUCCESS)
                {
                    SERVER_HEADER(scHardwareClose, mHardwareClose);
                    if(server_header == SCOPEFUN_SUCCESS)
                    {
                        sendMessage->header.error = sfHardwareClose(&pServer->ctx);
                        SERVER_SEND_MSG(scHardwareClose);
                    }
                }
                continue;
            }
        }
        // remove from list
        FORMAT("Client %d", pClient->id);
        msgListBoxRemove(getListBox2(), formatBuffer);
    }
    catch(...)
    {
    }
    return 1;
}

void* createServer(const char* serveraddr, int port)
{
    SDL_AtomicSet(&pServer->serverThreadActive,1);
    pServer->serverThread = SDL_CreateThread(ScopeFunServer, "scopefun_server", pServer);
    return pServer->serverThread;
}

void* createClient(ScopeFunClient* pClient)
{
    pClient->thread = (SDL_Thread*)SDL_CreateThread(ClientFunServer, "scopefun_client", pClient);
    return pClient->thread;
}

int closeServer()
{
     SDL_AtomicSet(&pServer->serverThreadActive,0);
    int status = 0;
    SDL_WaitThread(pServer->serverThread, &status);
    return 0;
}


////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////
