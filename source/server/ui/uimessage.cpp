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
#include "ScopeFunServerUI.h"
#include "wx/app.h"

#define MAX_LIST 65535

void* getListBox1()
{
    void* ret = 0;
    SDL_AtomicLock(&pServer->serverLockMsg);
    wxApp* pApp = (wxApp*)wxApp::GetInstance();
    if(pApp)
    {
        ScopeFunServerUI* pUI = (ScopeFunServerUI*)pApp->GetTopWindow();
        if(pUI)
        {
            ret = pUI->m_listBox1;
        }
    }
    SDL_AtomicUnlock(&pServer->serverLockMsg);
    return ret;
}

void* getListBox2()
{
    void* ret = 0;
    SDL_AtomicLock(&pServer->serverLockMsg);
    wxApp* pApp = (wxApp*)wxApp::GetInstance();
    if(pApp)
    {
        ScopeFunServerUI* pUI = (ScopeFunServerUI*)pApp->GetTopWindow();
        if(pUI)
        {
            ret = pUI->m_listBox2;
        }
    }
    SDL_AtomicUnlock(&pServer->serverLockMsg);
    return ret;
}

void msgListBoxClear(void* value)
{
    wxListBox* list = (wxListBox*)value;
    if(list)
    {
        SDL_AtomicLock(&pServer->serverLockMsg);
        if(list->IsEnabled())
        {
            list->Clear();
        }
        SDL_AtomicUnlock(&pServer->serverLockMsg);
    }
}

void msgListBoxAdd(void* value, const char* message)
{
    wxListBox* list = (wxListBox*)value;
    if(list)
    {
        SDL_AtomicLock(&pServer->serverLockMsg);
        if(list->IsEnabled() && list->GetCount() < MAX_LIST)
        {
            wxString* pString = new wxString(message);
            list->Append(message);
        }
        SDL_AtomicUnlock(&pServer->serverLockMsg);
    }
}

void msgListBoxRemove(void* value, const char* message)
{
    wxListBox* list = (wxListBox*)value;
    if(list)
    {
        SDL_AtomicLock(&pServer->serverLockMsg);
        if(list->IsEnabled())
        {
            uint idx = list->FindString(message);
            if(idx >= 0 && idx < list->GetCount())
            {
                list->Delete(idx);
            }
        }
        SDL_AtomicUnlock(&pServer->serverLockMsg);
    }
}
