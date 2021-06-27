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
#ifndef __CORE__TIMER__
#define __CORE__TIMER__

#define MAX_THREAD 16U

////////////////////////////////////////////////////////////////////////////////
//
// ThreadProduceConsume
//
///////////////////////////////////////////////////////////////////////////////
class ThreadProduceConsume
{
private:
    SDL_atomic_t headPos;
    SDL_atomic_t tailPos;
    SDL_atomic_t lockProducer;
    SDL_atomic_t lockConsumer;
    SDL_atomic_t lock[MAX_THREAD];
    uint         elCount;
public:
    ThreadProduceConsume();
public:
    void setCount(uint value);
public:
    bool producerLock(uint& id, bool multiple);
    bool producerUnlock(uint id);
    bool consumerLock(uint& id, bool multiple);
    bool consumerUnlock(uint id);
public:
    bool consumerLockUndo(uint id);
    bool consumerLockId(uint id);
};

////////////////////////////////////////////////////////////////////////////////
//
// ConsumerThreadSync
//
///////////////////////////////////////////////////////////////////////////////
class ConsumerThreadLock
{
private:
    SDL_atomic_t lock;
public:
    ConsumerThreadLock();
public:
    bool producerLock();
    bool producerUnlock();
    bool consumerLock();
    bool consumerUnlock();
};

////////////////////////////////////////////////////////////////////////////////
//
// ThreadData
//
///////////////////////////////////////////////////////////////////////////////
class ThreadData
{
public:
    uint   frame;
public:
    double dt;
    double fpsTime;
public:
    uint   fps;
    uint   fpsCounter;
public:
    ularge frequency;
    ularge lasttime;
    ularge currenttime;
public:
    uint userData;
};

////////////////////////////////////////////////////////////////////////////////
//
// TimerManager
//
////////////////////////////////////////////////////////////////////////////////
class TimerManager  : public Manager
{
private:
    ThreadData thread[MAX_THREAD];
public:
    uint   getFrame(uint threadId);
    double getDelta(uint threadId);
    uint   getFps(uint threadId);
    uint   getFpsCounter(uint threadId);
public:
    uint   getUserData(uint threadId);
    void   setUserData(uint threadId, uint user);
public:
    void deltaTime(uint threadId);
public:
    void init(uint threadId);
};

MANAGER_POINTER(Timer);

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
