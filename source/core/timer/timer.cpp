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

////////////////////////////////////////////////////////////////////////////////
// Globals
////////////////////////////////////////////////////////////////////////////////
MANAGER_REGISTER(Timer);


////////////////////////////////////////////////////////////////////////////////
// ThreadProduceConsume
////////////////////////////////////////////////////////////////////////////////
ThreadProduceConsume::ThreadProduceConsume()
{
    SDL_AtomicSet(&headPos, 0);
    SDL_AtomicSet(&tailPos, 0);
    SDL_AtomicSet(&lockProducer, 0);
    SDL_AtomicSet(&lockConsumer, 0);
    for(int i = 0; i < MAX_THREAD; i++)
    {
        SDL_AtomicSet(&lock[i], 0);
    }
    elCount = MAX_THREAD;
}

void ThreadProduceConsume::setCount(uint value)
{
    elCount = value;
}

bool ThreadProduceConsume::producerLock(uint& id, bool multiple)
{
    SDL_MemoryBarrierAcquire();
    if(multiple)
    {
        if(SDL_AtomicCAS(&lockProducer, 0, 1) == SDL_FALSE)
        {
            SDL_MemoryBarrierRelease();
            return false;
        }
    }
    uint head = SDL_AtomicGet(&headPos);
    uint tail = SDL_AtomicGet(&tailPos);
    id = head % elCount;
    if(SDL_AtomicCAS(&lock[id], 0, 1) == SDL_FALSE)
    {
        if(multiple)
        {
            while(SDL_AtomicCAS(&lockProducer, 1, 0) == SDL_FALSE) {};
        }
        SDL_MemoryBarrierRelease();
        return false;
    };
    SDL_AtomicIncRef(&headPos);
    if(multiple)
    {
        while(SDL_AtomicCAS(&lockProducer, 1, 0) == SDL_FALSE) {};
    }
    SDL_MemoryBarrierRelease();
    return true;
}

bool ThreadProduceConsume::producerUnlock(uint id)
{
    while(SDL_AtomicCAS(&lock[id], 1, 2) == SDL_FALSE) {};
    return true;
}

bool ThreadProduceConsume::consumerLock(uint& id, bool multiple)
{
    SDL_MemoryBarrierAcquire();
    if(multiple)
    {
        if(SDL_AtomicCAS(&lockConsumer, 0, 1) == SDL_FALSE)
        {
            SDL_MemoryBarrierRelease();
            return false;
        }
    }
    uint head = SDL_AtomicGet(&headPos);
    uint tail = SDL_AtomicGet(&tailPos);
    id = tail % elCount;
    if(SDL_AtomicCAS(&lock[id], 2, 3) == SDL_FALSE)
    {
        if(multiple)
        {
            while(SDL_AtomicCAS(&lockConsumer, 1, 0) == SDL_FALSE) {};
        }
        SDL_MemoryBarrierRelease();
        return false;
    };
    SDL_AtomicIncRef(&tailPos);
    if(multiple)
    {
        while(SDL_AtomicCAS(&lockConsumer, 1, 0) == SDL_FALSE) {};
    }
    SDL_MemoryBarrierRelease();
    return true;
}

bool ThreadProduceConsume::consumerUnlock(uint id)
{
    while(SDL_AtomicCAS(&lock[id], 3, 0) == SDL_FALSE) {};
    return true;
}

bool ThreadProduceConsume::consumerLockId(uint id)
{
    if(SDL_AtomicCAS(&lock[id], 2, 3) == SDL_FALSE)
    {
        return false;
    }
    return true;
}

bool ThreadProduceConsume::consumerLockUndo(uint id)
{
    SDL_AtomicDecRef(&tailPos);
    while(SDL_AtomicCAS(&lock[id], 3, 2) == SDL_FALSE) {};
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// ConsumerThreadSync
////////////////////////////////////////////////////////////////////////////////

ConsumerThreadLock::ConsumerThreadLock()
{
    SDL_AtomicSet(&lock, 0);
}

bool ConsumerThreadLock::producerLock()
{
    if(SDL_AtomicCAS(&lock, 0, 1) == SDL_FALSE)
    {
        return false;
    }
    return true;
}

bool ConsumerThreadLock::producerUnlock()
{
    if(SDL_AtomicCAS(&lock, 1, 2) == SDL_FALSE)
    {
        return false;
    }
    return true;
}

bool ConsumerThreadLock::consumerLock()
{
    if(SDL_AtomicCAS(&lock, 2, 3) == SDL_FALSE)
    {
        return false;
    }
    return true;
}

bool ConsumerThreadLock::consumerUnlock()
{
    if(SDL_AtomicCAS(&lock, 3, 0) == SDL_FALSE)
    {
        return false;
    }
    return true;
}


////////////////////////////////////////////////////////////////////////////////
// TimerManager
////////////////////////////////////////////////////////////////////////////////
void TimerManager::init(uint id)
{
    thread[id].frequency   = SDL_GetPerformanceFrequency();
    thread[id].currenttime = 0;
    thread[id].lasttime    = SDL_GetPerformanceCounter();
    thread[id].frame       = 0;
    thread[id].fpsTime     = 0;
    thread[id].fpsCounter  = 0;
    thread[id].fps         = 0;
    thread[id].dt          = 0;
}

uint TimerManager::getFrame(uint id)
{
    return thread[id].frame;
}

double TimerManager::getDelta(uint id)
{
    return thread[id].dt;
}

uint TimerManager::getFps(uint id)
{
    return thread[id].fps;
}

uint TimerManager::getFpsCounter(uint id)
{
    return thread[id].fpsCounter;
}

uint TimerManager::getUserData(uint id)
{
    return thread[id].userData;
}

void TimerManager::setUserData(uint id, uint user)
{
    thread[id].userData = user;
}

void TimerManager::deltaTime(uint id)
{
    SDL_MemoryBarrierAcquire();
    thread[id].currenttime = SDL_GetPerformanceCounter();
    thread[id].dt = ((double)(thread[id].currenttime - thread[id].lasttime)) / (double)thread[id].frequency;
    thread[id].lasttime = thread[id].currenttime;
    thread[id].fpsTime += thread[id].dt;
    if(thread[id].fpsTime > 1.0)
    {
        thread[id].fps = thread[id].fpsCounter;
        thread[id].fpsCounter = 0;
        thread[id].fpsTime = 0;
    }
    thread[id].fpsCounter++;
    thread[id].frame++;
    SDL_MemoryBarrierRelease();
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
