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

#include <wx/app.h>

ScopeFunServerUI::ScopeFunServerUI(wxWindow* parent)
    :
    ServerUI(parent)
{
}

extern "C" {
    extern char* xpm16_xpm[];
    extern char* xpm32_xpm[];
    extern char* xpm64_xpm[];
    extern char* xpm96_xpm[];
    extern char* xpm128_xpm[];
    extern char* xpm256_xpm[];
    extern char* xpm512_xpm[];
};

void msgListBoxRemove(wxListBox* list, const char* message);

void ScopeFunServerUI::ServerUIOnActivate(wxActivateEvent& event)
{
    // TODO: Implement ServerUIOnActivate
    if(m_textCtrl1->IsEnabled() && m_textCtrl2->IsEnabled())
    {
        // icon
        wxIconBundle icoBundle;
        wxIcon icon16(xpm16_xpm);
        wxIcon icon32(xpm32_xpm);
        wxIcon icon64(xpm64_xpm);
        wxIcon icon96(xpm96_xpm);
        wxIcon icon128(xpm128_xpm);
        wxIcon icon256(xpm256_xpm);
        wxIcon icon512(xpm512_xpm);
        icoBundle.AddIcon(icon16);
        icoBundle.AddIcon(icon32);
        icoBundle.AddIcon(icon64);
        icoBundle.AddIcon(icon96);
        icoBundle.AddIcon(icon128);
        icoBundle.AddIcon(icon512);
        SetIcons(icoBundle);
        m_checkBox2->SetValue(SDL_AtomicGet(&pServer->updateSimulation));
        m_textCtrl1->SetValue(pServer->ip.asChar());
        m_textCtrl1->Disable();
        wxString portString = wxString::Format(wxT("%i"), pServer->port);
        m_textCtrl2->SetValue(portString);
        m_textCtrl2->Disable();
        m_listBox1->Disable();
        m_checkBox1->SetValue(false);
    }
}

void ScopeFunServerUI::ServerUIOnClose(wxCloseEvent& event)
{
    wxExit();
}

void ScopeFunServerUI::m_button1OnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_button1OnButtonClick
    // id
    uint id = 0;
    int selected = m_listBox2->GetSelection();
    if(selected >= 0 && selected < (int)m_listBox2->GetCount())
    {
        wxString line = m_listBox2->GetString(selected);
        line.Replace("Client ", "");
        line.ToULong((unsigned long*)&id);
        // kill client
        pServer->killClient(id);
    }
}

void ScopeFunServerUI::m_button2OnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_button2OnButtonClick
}

void ScopeFunServerUI::m_checkBox1OnCheckBox(wxCommandEvent& event)
{
    // TODO: Implement m_checkBox1OnCheckBox
    if(!m_checkBox1->IsChecked())
    {
        m_listBox1->Clear();
        m_listBox1->Disable();
    }
    else
    {
        m_listBox1->Enable();
    }
}

void ScopeFunServerUI::m_checkBox2OnCheckBox(wxCommandEvent& event)
{
    // TODO: Implement m_checkBox2OnCheckBox
    SDL_AtomicSet(&pServer->updateSimulation, !m_checkBox2->GetValue());
}

void ScopeFunServerUI::m_textCtrl3OnTextEnter(wxCommandEvent& event)
{
}


void ScopeFunServerUI::msgVersion()
{
}
