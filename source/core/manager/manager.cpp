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
//
// globals
//
////////////////////////////////////////////////////////////////////////////////
MANAGER_REGISTER(Manager);

////////////////////////////////////////////////////////////////////////////////
//
// manager manager
//
////////////////////////////////////////////////////////////////////////////////
ManagerManager::ManagerManager()
{
    eventLock = 0;
}

int ManagerManager::registerManager(const String name, Manager* mng)
{
    mManager.insert(name, mng);
    return 0;
}

void ManagerManager::addStart(const String name)
{
    int idx = mManager.find(name);
    if(idx < 0)
    {
        CORE_ABORT("manager not found", 0);
    }
    //
    Manager* pMng = mManager[idx];
    aStart.pushBack(pMng);
}

void ManagerManager::addUpdate(const String name)
{
    int idx = mManager.find(name);
    if(idx < 0)
    {
        CORE_ABORT("manager not found", 0);
    }
    //
    Manager* pMng = mManager[idx];
    aUpdate.pushBack(pMng);
}

void ManagerManager::addStop(const String name)
{
    int idx = mManager.find(name);
    if(idx < 0)
    {
        CORE_ABORT("manager not found", 0);
    }
    //
    Manager* pMng = mManager[idx];
    aStop.pushBack(pMng);
}

void ManagerManager::start()
{
    for(int a = 0; a < aStart; a++)
    {
        int ret = aStart[a]->start();
        if(ret)
        {
            CORE_ABORT("engine start failed", 0);
        }
    }
}

void ManagerManager::fireEvents()
{
    // event
    SDL_AtomicLock(&eventLock);
    int count = aEvent.getCount();
    for(int a = 0; a < aEvent; a++)
    {
        for(int u = 0; u < aUpdate; u++)
        {
            aUpdate[u]->onEvent(aEvent[a]);
        }
    }
    aEvent.clear();
    SDL_AtomicUnlock(&eventLock);
}

void ManagerManager::queueEvent(SDL_Event& event)
{
    SDL_AtomicLock(&eventLock);
    if(aEvent.getCount() < aEvent.getSize())
    {
        aEvent.pushBack(event);
    }
    SDL_AtomicUnlock(&eventLock);
}

void ManagerManager::update(float dt)
{
    // update
    SDL_AtomicLock(&eventLock);
    for(int a = 0; a < aUpdate; a++)
    {
        aUpdate[a]->update(dt);
    }
    SDL_AtomicUnlock(&eventLock);
}

void ManagerManager::stop()
{
    for(int a = 0; a < aStop; a++)
    {
        int ret = aStop[a]->stop();
        if(ret)
        {
            CORE_ABORT("engine stop failed", 0);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
